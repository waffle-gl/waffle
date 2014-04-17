// Copyright 2014 Intel Corporation
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

/// @file wflinfo.c
/// @brief Print OpenGL info using Waffle.
///
/// This program does the following:
///     1. Dynamically choose the platform and OpenGL API according to
///        command line arguments.
///     2. Create an OpenGL context.
///     3. Print information about the context.

#define WAFFLE_API_VERSION 0x0103

#include <assert.h>
#include <ctype.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#    import <Foundation/NSAutoreleasePool.h>
#    import <Appkit/NSApplication.h>

static void
removeXcodeArgs(int *argc, char **argv);
#endif

#include "waffle.h"

static const char *usage_message =
    "Usage:\n"
    "    wflinfo <Required Parameters> [Options]\n"
    "\n"
    "Description:\n"
    "    Create an OpenGL or OpenGL ES context and print information about it.\n"
    "\n"
    "Required Parameters:\n"
    "    -p, --platform\n"
    "        One of: android, cgl, gbm, glx, wayland or x11_egl\n"
    "\n"
    "    -a, --api\n"
    "        One of: gl, gles1, gles2 or gles3\n"
    "\n"
    "Options:\n"
    "    -V, --version\n"
    "        For example --api=gl --version=3.2 would request OpenGL 3.2.\n"
    "\n"
    "    --profile\n"
    "        One of: core, compat or none\n"
    "\n"
    "    -v, --verbose\n"
    "        Print more information.\n"
    "\n"
    "    --forward-compatible\n"
    "        Create a forward-compatible context.\n"
    "\n"
    "    --debug-context\n"
    "        Create a debug context.\n"
    "\n"
    "    -h, --help\n"
    "        Print wflinfo usage information.\n"
    "\n"
    "Examples:\n"
    "    wflinfo --platform=glx --api=gl\n"
    "    wflinfo --platform=x11_egl --api=gl --version=3.2 --profile=core\n"
    "    wflinfo --platform=wayland --api=gles3\n"
    "    wflinfo --platform=gbm --api=gl --version=3.2 --verbose\n"
    "    wflinfo -p gbm -a gl -V 3.2 -v\n"
    ;

enum {
    OPT_PLATFORM = 'p',
    OPT_API = 'a',
    OPT_VERSION = 'V',
    OPT_PROFILE,
    OPT_VERBOSE = 'v',
    OPT_DEBUG_CONTEXT,
    OPT_FORWARD_COMPATIBLE,
    OPT_HELP = 'h',
};

static const struct option get_opts[] = {
    { .name = "platform",       .has_arg = required_argument,     .val = OPT_PLATFORM },
    { .name = "api",            .has_arg = required_argument,     .val = OPT_API },
    { .name = "version",        .has_arg = required_argument,     .val = OPT_VERSION },
    { .name = "profile",        .has_arg = required_argument,     .val = OPT_PROFILE },
    { .name = "verbose",        .has_arg = no_argument,           .val = OPT_VERBOSE },
    { .name = "debug-context",  .has_arg = no_argument,           .val = OPT_DEBUG_CONTEXT },
    { .name = "forward-compatible", .has_arg = no_argument,       .val = OPT_FORWARD_COMPATIBLE },
    { .name = "help",           .has_arg = no_argument,           .val = OPT_HELP },
    { 0 },
};

/// @defgroup Error handlers
/// @{
///
/// All error handlers exit.
///

static void __attribute__((noreturn))
error_printf(const char *module, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    fprintf(stderr, "%s error: ", module);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    exit(EXIT_FAILURE);
}

static void __attribute__((noreturn))
write_usage_and_exit(FILE *f, int exit_code)
{
    fprintf(f, "%s", usage_message);
    exit(exit_code);
}

static void __attribute__((noreturn))
usage_error_printf(const char *fmt, ...)
{
    fprintf(stderr, "Wflinfo usage error: ");

    if (fmt) {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        fprintf(stderr, " ");
    }

    fprintf(stderr, "(see wflinfo --help)\n");
    exit(EXIT_FAILURE);
}

static void
error_waffle(void)
{
    const struct waffle_error_info *info = waffle_error_get_info();
    const char *code = waffle_error_to_string(info->code);

    if (info->message_length > 0)
        error_printf("Waffle", "0x%x %s: %s", info->code, code, info->message);
    else
        error_printf("Waffle", "0x%x %s", info->code, code);
}

static void
error_get_gl_symbol(const char *name)
{
    error_printf("Wflinfo", "failed to get function pointer for %s", name);
}

/// @}
/// @defgroup OpenGL decalrations
/// @{

typedef float GLclampf;
typedef unsigned int GLbitfield;
typedef unsigned int GLint;
typedef int GLsizei;
typedef unsigned int GLenum;
typedef void GLvoid;
typedef unsigned char GLubyte;

enum {
    // Copied from <GL/gl*.h>.
    GL_NO_ERROR = 0,

    GL_CONTEXT_FLAGS = 0x821e,
    GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT = 0x00000001,
    GL_CONTEXT_FLAG_DEBUG_BIT              = 0x00000002,
    GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB  = 0x00000004,

    GL_VENDOR                              = 0x1F00,
    GL_RENDERER                            = 0x1F01,
    GL_VERSION                             = 0x1F02,
    GL_EXTENSIONS                          = 0x1F03,
    GL_NUM_EXTENSIONS                      = 0x821D,
};

#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 240

static GLenum (*glGetError)(void);
static void (*glGetIntegerv)(GLenum pname, GLint *params);
static const GLubyte * (*glGetString)(GLenum name);
static const GLubyte * (*glGetStringi)(GLenum name, GLint i);

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

    bool verbose;

    bool context_forward_compatible;
    bool context_debug;

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

static const char *
enum_map_to_str(const struct enum_map *self,
                int val)
{
    for (const struct enum_map *i = self; i->i != 0; ++i) {
        if (i->i == val) {
            return i->s;
        }
    }

    return NULL;
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

    // Set options to default values.
    opts->context_profile = WAFFLE_NONE;
    opts->context_version = -1;

    // prevent getopt_long from printing an error message
    opterr = 0;

    while (loop_get_opt) {
        int opt = getopt_long(argc, argv, "a:hp:vV:", get_opts, NULL);
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
            case OPT_VERBOSE:
                opts->verbose = true;
                break;
            case OPT_FORWARD_COMPATIBLE:
                opts->context_forward_compatible = true;
                break;
            case OPT_DEBUG_CONTEXT:
                opts->context_debug = true;
                break;
            case OPT_HELP:
                write_usage_and_exit(stdout, EXIT_SUCCESS);
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
    if (optarg)
        usage_error_printf("unrecognized option '%s'", optarg);
    else if (optopt)
        usage_error_printf("unrecognized option '-%c'", optopt);
    else
        usage_error_printf("unrecognized option");
}

/// @}

static int
parse_version(const char *version)
{
    int count, major, minor;

    if (version == NULL)
        return 0;

    while (*version != '\0' && !isdigit(*version))
        version++;

    count = sscanf(version, "%d.%d", &major, &minor);
    if (count != 2)
        return 0;

    if (minor > 9)
        return 0;

    return (major * 10) + minor;
}

static void
print_extensions(bool use_stringi)
{
    GLint count = 0, i;
    const char *ext;

    printf("OpenGL extensions: ");
    if (use_stringi) {
        glGetIntegerv(GL_NUM_EXTENSIONS, &count);
        if (glGetError() != GL_NO_ERROR) {
            printf("WFLINFO_GL_ERROR");
        } else {
            for (i = 0; i < count; i++) {
              ext = (const char *) glGetStringi(GL_EXTENSIONS, i);
              if (glGetError() != GL_NO_ERROR)
                  ext = "WFLINFO_GL_ERROR";
              printf("%s%s", ext, (i + 1) < count ? " " : "");
            }
        }
    } else {
        const char *extensions = (const char *) glGetString(GL_EXTENSIONS);
        if (glGetError() != GL_NO_ERROR)
            printf("WFLINFO_GL_ERROR");
        else
            printf("%s", extensions);
    }
    printf("\n");
}

static void
print_context_flags(void)
{
    static struct {
        GLint flag;
        char *str;
    } flags[] = {
        { GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT, "FORWARD_COMPATIBLE" },
        { GL_CONTEXT_FLAG_DEBUG_BIT, "DEBUG" },
        { GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB, "ROBUST_ACCESS" },
    };
    int flag_count = sizeof(flags) / sizeof(flags[0]);
    GLint context_flags = 0;

    printf("OpenGL context flags:");

    glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
    if (glGetError() != GL_NO_ERROR) {
        printf(" WFLINFO_GL_ERROR\n");
        return;
    }

    if (context_flags == 0) {
        printf(" 0x0\n");
        return;
    }

    for (int i = 0; i < flag_count; i++) {
        if ((flags[i].flag & context_flags) != 0) {
            printf(" %s", flags[i].str);
            context_flags = context_flags & ~flags[i].flag;
        }
    }
    for (int i = 0; context_flags != 0; context_flags >>= 1, i++) {
        if ((context_flags & 1) != 0) {
            printf(" 0x%x", 1 << i);
        }
    }
    printf("\n");
}

/// @brief Print out information about the context that was created.
static bool
print_wflinfo(struct options *opts)
{
    while(glGetError() != GL_NO_ERROR) {
        /* Clear all errors */
    }

    const char *vendor = (const char *) glGetString(GL_VENDOR);
    if (glGetError() != GL_NO_ERROR || vendor == NULL)
        vendor = "WFLINFO_GL_ERROR";

    const char *renderer = (const char *) glGetString(GL_RENDERER);
    if (glGetError() != GL_NO_ERROR || renderer == NULL)
        renderer = "WFLINFO_GL_ERROR";

    const char *version_str = (const char *) glGetString(GL_VERSION);
    if (glGetError() != GL_NO_ERROR || version_str == NULL)
        version_str = "WFLINFO_GL_ERROR";

    const char *platform = enum_map_to_str(platform_map, opts->platform);
    assert(platform != NULL);
    printf("Waffle platform: %s\n", platform);

    const char *api = enum_map_to_str(context_api_map, opts->context_api);
    assert(api != NULL);
    printf("Waffle api: %s\n", api);

    printf("OpenGL vendor string: %s\n", vendor);
    printf("OpenGL renderer string: %s\n", renderer);
    printf("OpenGL version string: %s\n", version_str);

    int version = parse_version(version_str);

    if (opts->context_api == WAFFLE_CONTEXT_OPENGL && version >= 31) {
        print_context_flags();
    }

    bool need_getstringi =
            opts->context_api == WAFFLE_CONTEXT_OPENGL &&
            version >= 30;

    if (!glGetStringi && need_getstringi)
        error_get_gl_symbol("glGetStringi");

    if (opts->verbose)
        print_extensions(need_getstringi);

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

static struct waffle_context *
wflinfo_try_create_context(struct options *opts,
                           struct waffle_config **config,
                           struct waffle_display *dpy,
                           bool exit_on_fail)
{
    int i;
    int32_t config_attrib_list[64];
    struct waffle_context *ctx;

    // Initialize outputs.
    ctx = NULL;
    *config = NULL;

    i = 0;
    config_attrib_list[i++] = WAFFLE_CONTEXT_API;
    config_attrib_list[i++] = opts->context_api;

    if (opts->context_profile != -1) {
        config_attrib_list[i++] = WAFFLE_CONTEXT_PROFILE;
        config_attrib_list[i++] = opts->context_profile;
    }

    if (opts->context_version != -1) {
        config_attrib_list[i++] = WAFFLE_CONTEXT_MAJOR_VERSION;
        config_attrib_list[i++] = opts->context_version / 10;
        config_attrib_list[i++] = WAFFLE_CONTEXT_MINOR_VERSION;
        config_attrib_list[i++] = opts->context_version % 10;
    }

    if (opts->context_forward_compatible) {
        config_attrib_list[i++] = WAFFLE_CONTEXT_FORWARD_COMPATIBLE;
        config_attrib_list[i++] = true;
    }

    if (opts->context_debug) {
        config_attrib_list[i++] = WAFFLE_CONTEXT_DEBUG;
        config_attrib_list[i++] = true;
    }

    static int32_t dont_care_attribs[] = {
        WAFFLE_RED_SIZE,
        WAFFLE_GREEN_SIZE,
        WAFFLE_BLUE_SIZE,
        WAFFLE_ALPHA_SIZE,
        WAFFLE_DEPTH_SIZE,
        WAFFLE_STENCIL_SIZE,
        WAFFLE_DOUBLE_BUFFERED,
    };
    int dont_care_attribs_count =
        sizeof(dont_care_attribs) / sizeof(dont_care_attribs[0]);

    for (int j = 0; j < dont_care_attribs_count; j++) {
        config_attrib_list[i++] = dont_care_attribs[j];
        config_attrib_list[i++] = WAFFLE_DONT_CARE;
    }

    config_attrib_list[i++] = 0;

    *config = waffle_config_choose(dpy, config_attrib_list);
    if (!*config) {
        goto fail;
    }

    ctx = waffle_context_create(*config, NULL);
    if (!ctx) {
        goto fail;
    }

    return ctx;

fail:
    if (exit_on_fail) {
        error_waffle();
    }
    if (ctx) {
        waffle_context_destroy(ctx);
    }
    if (*config) {
        waffle_config_destroy(*config);
    }

    return NULL;
}

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

static struct waffle_context *
wflinfo_create_context(struct options *opts,
                       struct waffle_config **config,
                       struct waffle_display *dpy)
{
    struct waffle_context *ctx;

    if (opts->context_profile != WAFFLE_NONE &&
        opts->context_api == WAFFLE_CONTEXT_OPENGL &&
        opts->context_version == -1) {

        // If the user requested OpenGL and a CORE or COMPAT profile,
        // but they didn't specify a version, then we'll try a set
        // of known versions from highest to lowest.

        static int known_gl_profile_versions[] =
            { 32, 33, 40, 41, 42, 43, 44 };

        for (int i = ARRAY_SIZE(known_gl_profile_versions) - 1; i >= 0; i--) {
            opts->context_version = known_gl_profile_versions[i];
            ctx = wflinfo_try_create_context(opts, config, dpy, false);
            opts->context_version = -1;
            if (ctx)
                break;
        }
    } else {
        ctx = wflinfo_try_create_context(opts, config, dpy, true);

    }

    return ctx;
}

int
main(int argc, char **argv)
{
    bool ok;
    int i;

    struct options opts = {0};

    int32_t init_attrib_list[3];

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
        error_printf("Wflinfo", "Display does not support %s",
                     waffle_enum_to_string(opts.context_api));
    }

    glGetError = waffle_get_proc_address("glGetError");
    if (!glGetError)
        error_get_gl_symbol("glGetError");

    glGetIntegerv = waffle_get_proc_address("glGetIntegerv");
    if (!glGetIntegerv)
        error_get_gl_symbol("glGetIntegerv");

    glGetString = waffle_get_proc_address("glGetString");
    if (!glGetString)
        error_get_gl_symbol("glGetString");

    glGetStringi = waffle_get_proc_address("glGetStringi");

    ctx = wflinfo_create_context(&opts, &config, dpy);
    if (!ctx) {
        error_printf("Wflinfo", "Failed to create context; Try choosing a "
                     "specific context with --version and/or --profile");
    }

    window = waffle_window_create(config, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!window)
        error_waffle();

    ok = waffle_make_current(dpy, window, ctx);
    if (!ok)
        error_waffle();

    ok = print_wflinfo(&opts);
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

    return EXIT_SUCCESS;
}
