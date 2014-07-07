// Copyright 2012 - 2014 Intel Corporation
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/// @file
/// @brief Do some basic OpenGL rendering using Waffle.
///
/// This example does the following:
///     1. Dynamically choose the platform and OpenGL API according to
///        command line arguments.
///     2. Create a window and OpenGL context.
///     3. Fill the window with red, then green, then blue, sleeping between
///        each buffer swap.

#define _POSIX_C_SOURCE 199309L // glibc feature macro for nanosleep.
#define WAFFLE_API_VERSION 0x0103
#define WAFFLE_API_EXPERIMENTAL

#include <getopt.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(_WIN32)
#include <time.h>
#else
#include <windows.h>
#endif

#ifdef __APPLE__
#    import <Foundation/NSAutoreleasePool.h>
#    import <Appkit/NSApplication.h>

static void
removeXcodeArgs(int *argc, char **argv);
#endif

#include "waffle.h"

static const char *usage_message =
    "usage:\n"
    "    gl_basic --platform=android|cgl|gbm|glx|wayland|wgl|x11_egl\n"
    "             --api=gl|gles1|gles2|gles3\n"
    "             [--version=MAJOR.MINOR]\n"
    "             [--profile=core|compat|none]\n"
    "             [--forward-compatible]\n"
    "             [--debug]\n"
    "             [--resize-window]\n"
    "\n"
    "examples:\n"
    "    gl_basic --platform=glx --api=gl\n"
    "    gl_basic --platform=x11_egl --api=gl --version=3.2 --profile=core\n"
    "    gl_basic --platform=wayland --api=gles3\n"
    "\n"
    "description:\n"
    "    Create a window. Fill it with red, green, then blue.\n"
    "\n"
    "options:\n"
    "    --forward-compatible\n"
    "        Create a forward-compatible context.\n"
    "\n"
    "    --debug\n"
    "        Create a debug context.\n"
    "\n"
    "    --resize-window\n"
    "        Resize the window between each draw call.\n"
    ;

enum {
    OPT_PLATFORM = 1,
    OPT_API,
    OPT_VERSION,
    OPT_PROFILE,
    OPT_DEBUG,
    OPT_FORWARD_COMPATIBLE,
    OPT_RESIZE_WINDOW,
};

static const struct option get_opts[] = {
    { .name = "platform",       .has_arg = required_argument,     .val = OPT_PLATFORM },
    { .name = "api",            .has_arg = required_argument,     .val = OPT_API },
    { .name = "version",        .has_arg = required_argument,     .val = OPT_VERSION },
    { .name = "profile",        .has_arg = required_argument,     .val = OPT_PROFILE },
    { .name = "debug",          .has_arg = no_argument,           .val = OPT_DEBUG },
    { .name = "forward-compatible", .has_arg = no_argument,       .val = OPT_FORWARD_COMPATIBLE },
    { .name = "resize-window",  .has_arg = no_argument,           .val = OPT_RESIZE_WINDOW },
    { 0 },
};

#if defined(__GNUC__)
#define NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define NORETURN __declspec(noreturn)
#else
#define NORETURN
#endif

static void NORETURN
error_printf(const char *fmt, ...)
{
    va_list ap;

    fflush(stdout);

    va_start(ap, fmt);
    fprintf(stderr, "gl_basic: error: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    exit(EXIT_FAILURE);
}

static void NORETURN
usage_error_printf(const char *fmt, ...)
{
    fflush(stdout);
    fprintf(stderr, "gl_basic: usage error");

    if (fmt) {
        va_list ap;
        va_start(ap, fmt);
        fprintf(stderr, ": ");
        vfprintf(stderr, fmt, ap);
        va_end(ap);
    }

    fprintf(stderr, "\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "%s", usage_message);

    exit(EXIT_FAILURE);
}

static void
error_waffle(void)
{
    const struct waffle_error_info *info = waffle_error_get_info();
    const char *code = waffle_error_to_string(info->code);

    if (info->message_length > 0)
        error_printf("%s: %s", code, info->message);
    else
        error_printf("%s", code);
}

static void
error_get_gl_symbol(const char *name)
{
    error_printf("failed to get function pointer for %s", name);
}

typedef float GLclampf;
typedef unsigned int GLbitfield;
typedef unsigned int GLint;
typedef int GLsizei;
typedef unsigned int GLenum;
typedef void GLvoid;

enum {
    // Copied from <GL/gl*.h>.
    GL_UNSIGNED_BYTE =    0x00001401,
    GL_RGBA =             0x00001908,
    GL_COLOR_BUFFER_BIT = 0x00004000,

    GL_CONTEXT_FLAGS = 0x821e,
    GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT = 0x00000001,
    GL_CONTEXT_FLAG_DEBUG_BIT              = 0x00000002,
};

#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 240

#ifndef _WIN32
#define APIENTRY
#else
#ifndef APIENTRY
#define APIENTRY __stdcall
#endif
#endif

static void (APIENTRY *glClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
static void (APIENTRY *glClear)(GLbitfield mask);
static void (APIENTRY *glGetIntegerv)(GLenum pname, GLint *params);
static void (APIENTRY *glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height,
                                     GLenum format, GLenum type, GLvoid* data);
static void (APIENTRY *glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);

/// @brief Command line options.
struct options {
    /// @brief One of `WAFFLE_PLATFORM_*`.
    int platform;

    /// @brief One of `WAFFLE_CONTEXT_OPENGL_*`.
    int context_api;

    /// @brief One of `WAFFLE_CONTEXT_PROFILE_*` or `WAFFLE_NONE`.
    int context_profile;

    int context_version;

    bool context_forward_compatible;
    bool context_debug;

    bool resize_window;

    /// @brief One of `WAFFLE_DL_*`.
    int dl;
};

struct enum_map {
    int i;
    const char *s;
};

static const struct enum_map platform_map[] = {
    {WAFFLE_PLATFORM_ANDROID,   "android"       },
    {WAFFLE_PLATFORM_CGL,       "cgl",          },
    {WAFFLE_PLATFORM_GBM,       "gbm"           },
    {WAFFLE_PLATFORM_GLX,       "glx"           },
    {WAFFLE_PLATFORM_WAYLAND,   "wayland"       },
    {WAFFLE_PLATFORM_WGL,       "wgl"           },
    {WAFFLE_PLATFORM_X11_EGL,   "x11_egl"       },
    {0,                         0               },
};

static const struct enum_map context_api_map[] = {
    {WAFFLE_CONTEXT_OPENGL,         "gl"        },
    {WAFFLE_CONTEXT_OPENGL_ES1,     "gles1"     },
    {WAFFLE_CONTEXT_OPENGL_ES2,     "gles2"     },
    {WAFFLE_CONTEXT_OPENGL_ES3,     "gles3"     },
    {0,                             0           },
};

/// @brief Translate string to `enum waffle_enum`.
///
/// @param self is a list of map items. The last item must be zero-filled.
/// @param result is altered only if @a s if found.
/// @return true if @a s was found in @a map.
static bool
enum_map_translate_str(
        const struct enum_map *self,
        const char *s,
        int *result)
{
    for (const struct enum_map *i = self; i->i != 0; ++i) {
        if (!strncmp(s, i->s, strlen(i->s) + 1)) {
            *result = i->i;
            return true;
        }
    }

    return false;
}

/// @return true on success.
static bool
parse_args(int argc, char *argv[], struct options *opts)
{
    bool ok;
    bool loop_get_opt = true;

#ifdef __APPLE__
    removeXcodeArgs(&argc, argv);
#endif

    // Set some context attrs to invalid values.
    opts->context_profile = -1;
    opts->context_version = -1;

    while (loop_get_opt) {
        int opt = getopt_long(argc, argv, "", get_opts, NULL);
        switch (opt) {
            case -1:
                loop_get_opt = false;
                break;
            case '?':
                goto error_unrecognized_arg;
            case OPT_PLATFORM:
                ok = enum_map_translate_str(platform_map, optarg,
                                            &opts->platform);
                if (!ok) {
                    usage_error_printf("'%s' is not a valid platform",
                                       optarg);
                }
                break;
            case OPT_API:
                ok = enum_map_translate_str(context_api_map, optarg,
                                            &opts->context_api);
                if (!ok) {
                    usage_error_printf("'%s' is not a valid API for an OpenGL "
                                       "context", optarg);
                }
                break;
            case OPT_VERSION: {
                int major;
                int minor;
                int match_count;

                match_count = sscanf(optarg, "%d.%d", &major, &minor);
                if (match_count != 2) {
                    usage_error_printf("'%s' is not a valid OpenGL version",
                                       optarg);
                }
                opts->context_version = 10 * major + minor;
                break;
            }
            case OPT_PROFILE:
                if (strcmp(optarg, "none") == 0) {
                    opts->context_profile = WAFFLE_NONE;
                } else if (strcmp(optarg, "core") == 0) {
                    opts->context_profile = WAFFLE_CONTEXT_CORE_PROFILE;
                } else if (strcmp(optarg, "compat") == 0) {
                    opts->context_profile = WAFFLE_CONTEXT_COMPATIBILITY_PROFILE;
                } else {
                    usage_error_printf("'%s' is not a valid OpenGL profile",
                                       optarg);
                }
                break;
            case OPT_FORWARD_COMPATIBLE:
                opts->context_forward_compatible = true;
                break;
            case OPT_DEBUG:
                opts->context_debug = true;
                break;
            case OPT_RESIZE_WINDOW:
                opts->resize_window = true;
                break;
            default:
                abort();
                loop_get_opt = false;
                break;
        }
    }

    if (optind < argc) {
        goto error_unrecognized_arg;
    }

    if (!opts->platform) {
        usage_error_printf("--platform is required");
    }

    if (!opts->context_api) {
        usage_error_printf("--api is required");
    }

    // Set dl.
    switch (opts->context_api) {
        case WAFFLE_CONTEXT_OPENGL:     opts->dl = WAFFLE_DL_OPENGL;      break;
        case WAFFLE_CONTEXT_OPENGL_ES1: opts->dl = WAFFLE_DL_OPENGL_ES1;  break;
        case WAFFLE_CONTEXT_OPENGL_ES2: opts->dl = WAFFLE_DL_OPENGL_ES2;  break;
        case WAFFLE_CONTEXT_OPENGL_ES3: opts->dl = WAFFLE_DL_OPENGL_ES3;  break;
        default:
            abort();
            break;
    }

    return true;

error_unrecognized_arg:
    usage_error_printf("unrecognized option '%s'", optarg);
}

static bool
draw(struct waffle_window *window, bool resize)
{
    bool ok;
    unsigned char *colors;
    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;

#if !defined(_WIN32)
    static const struct timespec sleep_time = {
         // 0.5 sec
        .tv_sec = 0,
        .tv_nsec = 500000000,
    };
#endif

    for (int i = 0; i < 3; ++i) {
        switch (i) {
            case 0: glClearColor(1, 0, 0, 1); break;
            case 1: glClearColor(0, 1, 0, 1); break;
            case 2: glClearColor(0, 0, 1, 1); break;
            case 3: abort(); break;
        }

        if (resize) {
            width = (i + 2) * 40;
            height = width;
            waffle_window_resize(window, width, height);
            glViewport(0, 0, width, height);
        }

        glClear(GL_COLOR_BUFFER_BIT);

        colors = calloc(width * height * 4, sizeof(*colors));
        glReadPixels(0, 0,
                     width, height,
                     GL_RGBA, GL_UNSIGNED_BYTE,
                     colors);
        for (int j = 0; j < width * height * 4; j += 4) {
           if ((colors[j]   != (i == 0 ? 0xff : 0)) ||
               (colors[j+1] != (i == 1 ? 0xff : 0)) ||
               (colors[j+2] != (i == 2 ? 0xff : 0)) ||
               (colors[j+3] != 0xff)) {
              fprintf(stderr, "glReadPixels returned unexpected result\n");
              break;
           }
        }
        free(colors);

        if (i == 0) {
            ok = waffle_window_show(window);
            if (!ok)
                return false;
        }

        ok = waffle_window_swap_buffers(window);
        if (!ok)
            return false;

#if !defined(_WIN32)
        nanosleep(&sleep_time, NULL);
#else
        Sleep(500);
#endif
    }

    return true;
}

#ifdef __APPLE__

static NSAutoreleasePool *pool;

static void
cocoa_init(void)
{
    // From the NSApplication Class Reference:
    //     [...] if you do need to use Cocoa classes within the main()
    //     function itself (other than to load nib files or to instantiate
    //     NSApplication), you should create an autorelease pool before using
    //     the classes and then release the pool when you’re done.
    pool = [[NSAutoreleasePool alloc] init];

    // From the NSApplication Class Reference:
    //     The sharedApplication class method initializes the display
    //     environment and connects your program to the window server and the
    //     display server.
    //
    // It also creates the singleton NSApp if it does not yet exist.
    [NSApplication sharedApplication];
}

static void
cocoa_finish(void)
{
    [pool drain];
}

static void
removeArg(int index, int *argc, char **argv)
{
    --*argc;
    for (; index < *argc; ++index)
        argv[index] = argv[index + 1];
}

static void
removeXcodeArgs(int *argc, char **argv)
{
    // Xcode sometimes adds additional arguments.
    for (int i = 1; i < *argc; )
    {
        if (strcmp(argv[i], "-NSDocumentRevisionsDebugMode") == 0 ||
            strcmp(argv[i], "-ApplePersistenceIgnoreState" ) == 0)
        {
            removeArg(i, argc, argv);
            removeArg(i, argc, argv);
        } else
            ++i;
    }
}

#endif // __APPLE__

int
main(int argc, char **argv)
{
    bool ok;
    int i;

    struct options opts = {0};

    int32_t init_attrib_list[3];
    int32_t config_attrib_list[64];

    struct waffle_display *dpy;
    struct waffle_config *config;
    struct waffle_context *ctx;
    struct waffle_window *window;

    GLint context_flags = 0;

    #ifdef __APPLE__
        cocoa_init();
    #endif

    ok = parse_args(argc, argv, &opts);
    if (!ok)
        exit(EXIT_FAILURE);

    i = 0;
    init_attrib_list[i++] = WAFFLE_PLATFORM;
    init_attrib_list[i++] = opts.platform;
    init_attrib_list[i++] = WAFFLE_NONE;

    ok = waffle_init(init_attrib_list);
    if (!ok)
        error_waffle();

    dpy = waffle_display_connect(NULL);
    if (!dpy)
        error_waffle();

    if (!waffle_display_supports_context_api(dpy, opts.context_api)) {
        error_printf("Display does not support %s",
                     waffle_enum_to_string(opts.context_api));
    }

    glClear = waffle_dl_sym(opts.dl, "glClear");
    if (!glClear)
        error_get_gl_symbol("glClear");

    glClearColor = waffle_dl_sym(opts.dl, "glClearColor");
    if (!glClearColor)
        error_get_gl_symbol("glClearColor");

    glGetIntegerv = waffle_dl_sym(opts.dl, "glGetIntegerv");
    if (!glGetIntegerv)
        error_get_gl_symbol("glGetIntegerv");

    glReadPixels = waffle_dl_sym(opts.dl, "glReadPixels");
    if (!glReadPixels)
        error_get_gl_symbol("glReadPixels");

    glViewport = waffle_dl_sym(opts.dl, "glViewport");
    if (!glViewport)
        error_get_gl_symbol("glViewport");

    i = 0;
    config_attrib_list[i++] = WAFFLE_CONTEXT_API;
    config_attrib_list[i++] = opts.context_api;

    if (opts.context_profile != -1) {
        config_attrib_list[i++] = WAFFLE_CONTEXT_PROFILE;
        config_attrib_list[i++] = opts.context_profile;
    }

    if (opts.context_version != -1) {
        config_attrib_list[i++] = WAFFLE_CONTEXT_MAJOR_VERSION;
        config_attrib_list[i++] = opts.context_version / 10;
        config_attrib_list[i++] = WAFFLE_CONTEXT_MINOR_VERSION;
        config_attrib_list[i++] = opts.context_version % 10;
    }

    if (opts.context_forward_compatible) {
        config_attrib_list[i++] = WAFFLE_CONTEXT_FORWARD_COMPATIBLE;
        config_attrib_list[i++] = true;
    }

    if (opts.context_debug) {
        config_attrib_list[i++] = WAFFLE_CONTEXT_DEBUG;
        config_attrib_list[i++] = true;
    }

    config_attrib_list[i++] = WAFFLE_RED_SIZE;
    config_attrib_list[i++] = 8;
    config_attrib_list[i++] = WAFFLE_GREEN_SIZE;
    config_attrib_list[i++] = 8;
    config_attrib_list[i++] = WAFFLE_BLUE_SIZE;
    config_attrib_list[i++] = 8;
    config_attrib_list[i++] = WAFFLE_DOUBLE_BUFFERED;
    config_attrib_list[i++] = true;
    config_attrib_list[i++] = 0;

    config = waffle_config_choose(dpy, config_attrib_list);
    if (!config)
        error_waffle();

    ctx = waffle_context_create(config, NULL);
    if (!ctx)
        error_waffle();

    window = waffle_window_create(config, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!window)
        error_waffle();

    ok = waffle_make_current(dpy, window, ctx);
    if (!ok)
        error_waffle();

    if (opts.context_forward_compatible || opts.context_debug) {
        glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
    }

    if (opts.context_forward_compatible
        && !(context_flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT)) {
        error_printf("context is not forward-compatible");
    }

    if (opts.context_debug
        && !(context_flags & GL_CONTEXT_FLAG_DEBUG_BIT)) {
        error_printf("context is not a debug context");
    }

    ok = draw(window, opts.resize_window);
    if (!ok)
        error_waffle();

    ok = waffle_make_current(dpy, NULL, NULL);
    if (!ok)
        error_waffle();

    ok = waffle_window_destroy(window);
    if (!ok)
        error_waffle();

    ok = waffle_context_destroy(ctx);
    if (!ok)
        error_waffle();

    ok = waffle_config_destroy(config);
    if (!ok)
        error_waffle();

    ok = waffle_display_disconnect(dpy);
    if (!ok)
        error_waffle();

    #ifdef __APPLE__
        cocoa_finish();
    #endif

    printf("gl_basic: run was successful\n");
    return EXIT_SUCCESS;
}
