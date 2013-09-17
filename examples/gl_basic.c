// Copyright 2012 Intel Corporation
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

/// @file gl_basic.c
/// @brief Do some basic GL rendering using Waffle.
///
/// This example does the following:
///     1. Dynamically choose the platform and GL API according to command
///        line arguments.
///     2. Create a window and GL context.
///     3. Fill the window with red, then green, then blue, sleeping between
///        each buffer swap.

#define _POSIX_C_SOURCE 199309L // glibc feature macro for nanosleep.

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __APPLE__
#    import <Foundation/NSAutoreleasePool.h>
#    import <Appkit/NSApplication.h>

static void
removeXcodeArgs(int *argc, char **argv);
#endif

#include "waffle.h"

static const char *usage_message =
    "usage:\n"
    "    gl_basic <platform> <context_api> [version] [profile]\n"
    "\n"
    "arguments:\n"
    "    platform: One of android, cgl, gbm, glx, wayland, x11_egl.\n"
    "    context_api: One of gl, gles1, gles2, gles3.\n"
    "    version: In form \"major.minor\".\n"
    "    profile: One of core, compat, or none.\n"
    "\n"
    "examples:\n"
    "    gl_basic glx gl\n"
    "    gl_basic x11_egl gl 3.2 core\n"
    "    gl_basic wayland gles3\n"
    "\n"
    "description:\n"
    "    Create a window. Fill it with red, green, then blue.\n";

/// @defgroup Error handlers
/// @{
///
/// All error handlers exit.
///

static void __attribute__((noreturn))
gl_basic_error(const char *fmt, ...)
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
static void __attribute__((noreturn))
error_usage(void)
{
    fprintf(stderr, "usage error\n\n");
    fprintf(stderr, "%s", usage_message);
    exit(EXIT_FAILURE);
}


static void
error_waffle(void)
{
    const struct waffle_error_info *info = waffle_error_get_info();
    const char *code = waffle_error_to_string(info->code);

    if (info->message_length > 0)
        gl_basic_error("%s: %s", code, info->message);
    else
        gl_basic_error("%s", code);
}

static void
error_get_gl_symbol(const char *name)
{
    gl_basic_error("failed to get function pointer for %s", name);
}

/// @}
/// @defgroup GL decalrations
/// @{

typedef float GLclampf;
typedef unsigned int GLbitfield;
typedef unsigned int GLint;
typedef int GLsizei;
typedef unsigned int GLenum;
typedef void GLvoid;

enum {
    // Copied for <GL/gl.h>.
    GL_UNSIGNED_BYTE =    0x00001401,
    GL_RGBA =             0x00001908,
    GL_COLOR_BUFFER_BIT = 0x00004000,
};

#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 240

static void (*glClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
static void (*glClear)(GLbitfield mask);
static void (*glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height,
                            GLenum format, GLenum type, GLvoid* data);

/// @}
/// @defgroup Parsing Options
/// @{

struct options {
    /// @brief One of `WAFFLE_PLATFORM_*`.
    int platform;

    /// @brief One of `WAFFLE_CONTEXT_OPENGL_*`.
    int context_api;

    /// @brief One of `WAFFLE_CONTEXT_PROFILE_*` or `WAFFLE_NONE`.
    int context_profile;

    int context_version;

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
    const char *arg;
    bool ok;

#ifdef __APPLE__
    removeXcodeArgs(&argc, argv);
#endif

    if (argc < 3)
        error_usage();

    // Set some context attrs to invalid values.
    opts->context_profile = -1;
    opts->context_version = -1;

    // Set platform.
    arg = argv[1];
    ok = enum_map_translate_str(platform_map, arg, &opts->platform);
    if (!ok) {
        fprintf(stderr, "error: '%s' is not a valid platform\n", arg);
        error_usage();
    }

    // Set context_api.
    arg = argv[2];
    ok = enum_map_translate_str(context_api_map, arg, &opts->context_api);
    if (!ok) {
        fprintf(stderr, "error: '%s' is not a valid API for a GL context\n", arg);
        error_usage();
    }

    // Set context_version.
    if (argc >= 4) {
	int major;
	int minor;
	int match_count;

	arg = argv[3];
        match_count = sscanf(arg, "%d.%d", &major, &minor);
        if (match_count != 2) {
            fprintf(stderr, "error: '%s' is not a valid GL version\n", arg);
            error_usage();
        }
        opts->context_version = 10 * major + minor;
    }

    // Set context_profile.
    if (argc >= 5) {
        arg = argv[4];
        if (strcmp(arg, "none") == 0)
            opts->context_profile = WAFFLE_NONE;
        else if (strcmp(arg, "core") == 0)
            opts->context_profile = WAFFLE_CONTEXT_CORE_PROFILE;
        else if (strcmp(arg, "compat") == 0)
            opts->context_profile = WAFFLE_CONTEXT_COMPATIBILITY_PROFILE;
        else {
            fprintf(stderr, "error: '%s' is not a valid GL profile\n", arg);
            error_usage();
        }
    }

    if (argc >= 6)
        error_usage();

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
}

/// @}

static bool
draw(struct waffle_window *window)
{
    bool ok;
    unsigned char *colors;

    static const struct timespec sleep_time = {
         // 0.5 sec
        .tv_sec = 0,
        .tv_nsec = 500000000,
    };

    for (int i = 0; i < 3; ++i) {
        switch (i) {
            case 0: glClearColor(1, 0, 0, 1); break;
            case 1: glClearColor(0, 1, 0, 1); break;
            case 2: glClearColor(0, 0, 1, 1); break;
            case 3: abort(); break;
        }

        glClear(GL_COLOR_BUFFER_BIT);

        colors = calloc(WINDOW_WIDTH * WINDOW_HEIGHT * 4, sizeof(*colors));
        glReadPixels(0, 0,
                     WINDOW_WIDTH, WINDOW_HEIGHT,
                     GL_RGBA, GL_UNSIGNED_BYTE,
                     colors);
        for (int j = 0; j < WINDOW_WIDTH * WINDOW_HEIGHT * 4; j += 4) {
           if ((colors[j]   != (i == 0 ? 0xff : 0)) ||
               (colors[j+1] != (i == 1 ? 0xff : 0)) ||
               (colors[j+2] != (i == 2 ? 0xff : 0)) ||
               (colors[j+3] != 0xff)) {
              free(colors);
              gl_basic_error("glReadPixels returned unexpected result");
           }
        }
        free(colors);

        ok = waffle_window_swap_buffers(window);
        if (!ok)
            return false;
        nanosleep(&sleep_time, NULL);
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

    struct options opts;

    int32_t init_attrib_list[3];
    int32_t config_attrib_list[64];

    struct waffle_display *dpy;
    struct waffle_config *config;
    struct waffle_context *ctx;
    struct waffle_window *window;

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
        gl_basic_error("Display does not support %s",
                       waffle_enum_to_string(opts.context_api));
    }

    glClear = waffle_dl_sym(opts.dl, "glClear");
    if (!glClear)
        error_get_gl_symbol("glClear");

    glClearColor = waffle_dl_sym(opts.dl, "glClearColor");
    if (!glClearColor)
        error_get_gl_symbol("glClearColor");

    glReadPixels = waffle_dl_sym(opts.dl, "glReadPixels");
    if (!glReadPixels)
        error_get_gl_symbol("glReadPixels");

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

    ok = waffle_window_show(window);
    if (!ok)
        error_waffle();

    ok = waffle_make_current(dpy, window, ctx);
    if (!ok)
        error_waffle();

    ok = draw(window);
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
