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

/// @file
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
    "        One of: android, cgl, gbm, glx, wayland, wgl or x11_egl\n"
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

static bool
strneq(const char *a, const char *b, size_t n)
{
    return strncmp(a, b, n) == 0;
}

#if defined(__GNUC__)
#define NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define NORETURN __declspec(noreturn)
#else
#define NORETURN
#endif

static void NORETURN
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

static void NORETURN
write_usage_and_exit(FILE *f, int exit_code)
{
    fprintf(f, "%s", usage_message);
    exit(exit_code);
}

static void NORETURN
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

#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_CONTEXT_PROFILE_MASK           0x9126
#define GL_CONTEXT_CORE_PROFILE_BIT       0x00000001
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002

#ifndef _WIN32
#define APIENTRY
#else
#ifndef APIENTRY
#define APIENTRY __stdcall
#endif
#endif

static GLenum (APIENTRY *glGetError)(void);
static void (APIENTRY *glGetIntegerv)(GLenum pname, GLint *params);
static const GLubyte * (APIENTRY *glGetString)(GLenum name);
static const GLubyte * (APIENTRY *glGetStringi)(GLenum name, GLint i);

/// @brief Command line options.
struct options {
    /// @brief One of `WAFFLE_PLATFORM_*`.
    int platform;

    /// @brief One of `WAFFLE_CONTEXT_OPENGL_*`.
    int context_api;

    /// @brief One of `WAFFLE_CONTEXT_PROFILE_*` or `WAFFLE_NONE`.
    int context_profile;

    int context_major;
    int context_minor;

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
    opts->context_major = WAFFLE_DONT_CARE;
    opts->context_minor = WAFFLE_DONT_CARE;

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
                if (match_count != 2 || major < 0 || minor < 0) {
                    usage_error_printf("'%s' is not a valid OpenGL version",
                                       optarg);
                }
                opts->context_major = major;
                opts->context_minor = minor;
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
print_wflinfo(const struct options *opts)
{
    while(glGetError() != GL_NO_ERROR) {
        /* Clear all errors */
    }

    const char *vendor = (const char *) glGetString(GL_VENDOR);
    if (glGetError() != GL_NO_ERROR || vendor == NULL) {
        vendor = "WFLINFO_GL_ERROR";
    }

    const char *renderer = (const char *) glGetString(GL_RENDERER);
    if (glGetError() != GL_NO_ERROR || renderer == NULL) {
        renderer = "WFLINFO_GL_ERROR";
    }

    const char *version_str = (const char *) glGetString(GL_VERSION);
    if (glGetError() != GL_NO_ERROR || version_str == NULL) {
        version_str = "WFLINFO_GL_ERROR";
    }

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

    // OpenGL and OpenGL ES >= 3.0 support glGetStringi(GL_EXTENSION, i).
    const bool use_getstringi = version >= 30;

    if (!glGetStringi && use_getstringi)
        error_get_gl_symbol("glGetStringi");

    if (opts->verbose)
        print_extensions(use_getstringi);

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

/// @brief Attributes for waffle_choose_config().
struct wflinfo_config_attrs {
    /// @brief One of `WAFFLE_CONTEXT_OPENGL_*`.
    enum waffle_enum api;

    /// @brief One of `WAFFLE_CONTEXT_PROFILE_*` or `WAFFLE_NONE`.
    enum waffle_enum profile;

    /// @brief The version major number.
    int32_t major;

    /// @brief The version minor number.
    int32_t minor;

    /// @brief Create a forward-compatible context.
    bool forward_compat;

    /// @brief Create a debug context.
    bool debug;
};

static bool
wflinfo_try_create_context(struct waffle_display *dpy,
                           struct wflinfo_config_attrs attrs,
                           struct waffle_context **out_ctx,
                           struct waffle_config **out_config,
                           bool exit_on_fail)
{
    int i;
    int32_t config_attrib_list[64];
    struct waffle_context *ctx = NULL;
    struct waffle_config *config = NULL;

    i = 0;
    config_attrib_list[i++] = WAFFLE_CONTEXT_API;
    config_attrib_list[i++] = attrs.api;

    if (attrs.profile != WAFFLE_DONT_CARE) {
        config_attrib_list[i++] = WAFFLE_CONTEXT_PROFILE;
        config_attrib_list[i++] = attrs.profile;
    }

    if (attrs.major != WAFFLE_DONT_CARE && attrs.minor != WAFFLE_DONT_CARE) {
        config_attrib_list[i++] = WAFFLE_CONTEXT_MAJOR_VERSION;
        config_attrib_list[i++] = attrs.major;
        config_attrib_list[i++] = WAFFLE_CONTEXT_MINOR_VERSION;
        config_attrib_list[i++] = attrs.minor;
    }

    if (attrs.forward_compat) {
        config_attrib_list[i++] = WAFFLE_CONTEXT_FORWARD_COMPATIBLE;
        config_attrib_list[i++] = true;
    }

    if (attrs.debug) {
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

    config = waffle_config_choose(dpy, config_attrib_list);
    if (!config) {
        goto fail;
    }

    ctx = waffle_context_create(config, NULL);
    if (!ctx) {
        goto fail;
    }

    *out_ctx = ctx;
    *out_config = config;
    return true;

fail:
    if (exit_on_fail) {
        error_waffle();
    }
    if (ctx) {
        waffle_context_destroy(ctx);
    }
    if (config) {
        waffle_config_destroy(config);
    }

    return false;
}

/// @brief Return 10 * version of the current OpenGL context.
static int
gl_get_version(void)
{
    GLint major_version = 0;
    GLint minor_version = 0;

    glGetIntegerv(GL_MAJOR_VERSION, &major_version);
    if (glGetError()) {
        error_printf("Wflinfo", "glGetIntegerv(GL_MAJOR_VERSION) failed");
    }

    glGetIntegerv(GL_MINOR_VERSION, &minor_version);
    if (glGetError()) {
        error_printf("Wflinfo", "glGetIntegerv(GL_MINOR_VERSION) failed");
    }
    return 10 * major_version + minor_version;
}

/// @brief Check if current context has an extension using glGetString().
static bool
gl_has_extension_GetString(const char *name)
{
#define BUF_LEN 4096
    char exts[BUF_LEN];

    const uint8_t *exts_orig = glGetString(GL_EXTENSIONS);
    if (glGetError()) {
        error_printf("Wflinfo", "glGetInteger(GL_EXTENSIONS) failed");
    }

    memcpy(exts, exts_orig, BUF_LEN);
    exts[BUF_LEN - 1] = 0;

    char *ext = strtok(exts, " ");
    do {
        if (strneq(ext, name, BUF_LEN)) {
            return true;
        }
        ext = strtok(NULL, " ");
    } while (ext);

    return false;
#undef BUF_LEN
}

/// @brief Check if current context has an extension using glGetStringi().
static bool
gl_has_extension_GetStringi(const char *name)
{
    const size_t max_ext_len = 128;
    uint32_t num_exts = 0;

    glGetIntegerv(GL_NUM_EXTENSIONS, &num_exts);
    if (glGetError()) {
        error_printf("Wflinfo", "glGetIntegerv(GL_NUM_EXTENSIONS) failed");
    }

    for (uint32_t i = 0; i < num_exts; i++) {
        const uint8_t *ext = glGetStringi(GL_EXTENSIONS, i);
        if (!ext || glGetError()) {
            error_printf("Wflinfo", "glGetStringi(GL_EXTENSIONS) failed");
        } else if (strneq((const char*) ext, name, max_ext_len)) {
            return true;
        }
    }

    return false;
}

/// @brief Check if current context has an extension.
static bool
gl_has_extension(const char *name)
{
    if (gl_get_version() >= 30) {
        return gl_has_extension_GetStringi(name);
    } else {
        return gl_has_extension_GetString(name);
    }
}

/// @brief Get the profile of a desktop OpenGL context.
///
/// Return one of WAFFLE_CONTEXT_CORE_PROFILE,
/// WAFFLE_CONTEXT_COMPATIBILITY_PROFILE, or WAFFLE_NONE.
///
/// Even though an OpenGL 3.1 context strictly has no profile, according to
/// this function a 3.1 context belongs to the core profile if and only if it
/// lacks the GL_ARB_compatibility extension.
///
/// According to this function, a context has no profile if and only if its
/// version is 3.0 or lower.
static enum waffle_enum
gl_get_profile(void)
{
    int version = gl_get_version();

    if (version >= 32) {
        uint32_t profile_mask = 0;
        glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile_mask);
        if (glGetError()) {
            error_printf("Wflinfo", "glGetIntegerv(GL_CONTEXT_PROFILE_MASK) "
                        "failed");
        } else if (profile_mask & GL_CONTEXT_CORE_PROFILE_BIT) {
            return WAFFLE_CONTEXT_CORE_PROFILE;
        } else if (profile_mask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) {
            return WAFFLE_CONTEXT_COMPATIBILITY_PROFILE;
        } else {
            error_printf("Wflinfo", "glGetIntegerv(GL_CONTEXT_PROFILE_MASK) "
                         "return a mask with no profile bit: 0x%x",
                         profile_mask);
        }
    } else if (version == 31) {
        if (gl_has_extension("GL_ARB_compatibility")) {
            return WAFFLE_CONTEXT_CORE_PROFILE;
        } else {
            return WAFFLE_CONTEXT_COMPATIBILITY_PROFILE;
        }
    } else {
        return WAFFLE_NONE;
    }
}

/// @brief Create an OpenGL >= 3.1 context.
///
/// If the requested profile is WAFFLE_NONE or WAFFLE_DONT_CARE and context
/// creation succeeds, then return true.
///
/// If a specific profile of OpenGL 3.1 is requested, then this function tries
/// to honor the intent of that request even though, strictly speaking, an
/// OpenGL 3.1 context has no profile.  (See gl_get_profile() for a description
/// of how wflinfo determines the profile of a context). If context creation
/// succeeds but its profile is incorrect, then return false.
///
/// On failure, @a out_ctx and @out_config remain unmodified.
///
static bool
wflinfo_try_create_context_gl31(struct waffle_display *dpy,
                                struct wflinfo_config_attrs attrs,
                                struct waffle_context **out_ctx,
                                struct waffle_config **out_config,
                                bool exit_if_ctx_creation_fails)
{
    struct waffle_config *config = NULL;
    struct waffle_context *ctx = NULL;
    bool ok;

    // It's illegal to request a waffle_config with WAFFLE_CONTEXT_PROFILE
    // != WAFFLE_NONE. Therefore, request an OpenGL 3.1 config without
    // a profile and later verify that the desired and actual profile
    // agree.
    const enum waffle_enum desired_profile = attrs.profile;
    attrs.major = 3;
    attrs.minor = 1;
    attrs.profile = WAFFLE_NONE;
    wflinfo_try_create_context(dpy, attrs, &ctx, &config,
                               exit_if_ctx_creation_fails);

    if (desired_profile == WAFFLE_NONE ||
        desired_profile == WAFFLE_DONT_CARE) {
        goto success;
    }

    // The user cares about the profile. We must bind the context to inspect
    // its profile.
    //
    // Skip window creation. No window is needed when binding an OpenGL >= 3.0
    // context.
    ok = waffle_make_current(dpy, NULL, ctx);
    if (!ok) {
        error_waffle();
    }

    const enum waffle_enum actual_profile = gl_get_profile();
    waffle_make_current(dpy, NULL, NULL);
    if (actual_profile == desired_profile) {
        goto success;
    }

    return false;

success:
    *out_ctx = ctx;
    *out_config = config;
    return true;
}

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

/// Exit on failure.
static void
wflinfo_create_context(struct waffle_display *dpy,
                       struct wflinfo_config_attrs attrs,
                       struct waffle_context **out_ctx,
                       struct waffle_config **out_config)
{
    bool ok = false;

    if (attrs.api == WAFFLE_CONTEXT_OPENGL &&
        attrs.profile != WAFFLE_NONE &&
        attrs.major == WAFFLE_DONT_CARE) {

        // If the user requested OpenGL and a CORE or COMPAT profile,
        // but they didn't specify a version, then we'll try a set
        // of known versions from highest to lowest.

        static int known_gl_profile_versions[] =
            { 32, 33, 40, 41, 42, 43, 44 };

        for (int i = ARRAY_SIZE(known_gl_profile_versions) - 1; i >= 0; i--) {
            attrs.major = known_gl_profile_versions[i] / 10;
            attrs.minor = known_gl_profile_versions[i] % 10;
            ok = wflinfo_try_create_context(dpy, attrs,
                                            out_ctx, out_config, false);
            if (ok) {
                return;
            }
        }

        // Handle OpenGL 3.1 separately because profiles are weird in 3.1.
        ok = wflinfo_try_create_context_gl31(
                dpy, attrs, out_ctx, out_config,
                /*exit_if_ctx_creation_fails*/ false);
        if (ok) {
            return;
        }

        error_printf("Wflinfo", "Failed to create context; Try choosing a "
                     "specific context version with --version");
    } else if (attrs.api == WAFFLE_CONTEXT_OPENGL &&
               attrs.major == 3 &&
               attrs.minor == 1) {
        // The user requested a specific profile of an OpenGL 3.1 context.
        // Strictly speaking, an OpenGL 3.1 context has no profile, but let's
        // do what the user wants.
        ok = wflinfo_try_create_context_gl31(
                dpy, attrs, out_ctx, out_config,
                /*exit_if_ctx_creation_fails*/ true);
        if (ok) {
            return;
        }

        printf("Wflinfo warn: Succesfully requested an OpenGL 3.1 context, but returned\n"
               "Wflinfo warn: context had the wrong profile.  Fallback to requesting an\n"
               "Wflinfo warn: OpenGL 3.2 context, which is guaranteed to have the correct\n"
               "Wflinfo warn: profile if context creation succeeds.\n");
        attrs.major = 3;
        attrs.minor = 2;
        assert(attrs.profile == WAFFLE_CONTEXT_CORE_PROFILE ||
               attrs.profile == WAFFLE_CONTEXT_COMPATIBILITY_PROFILE);
        ok = wflinfo_try_create_context(dpy, attrs, out_ctx, out_config,
                                        /*exit_on_fail*/ false);
        if (ok) {
            return;
        }

        error_printf("Wflinfo", "Failed to create an OpenGL 3.1 or later "
                     "context with requested profile");
    } else {
        wflinfo_try_create_context(dpy, attrs, out_ctx, out_config,
                                  /*exit_on_fail*/ true);
    }
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

    glGetError = waffle_dl_sym(opts.dl, "glGetError");
    if (!glGetError)
        error_get_gl_symbol("glGetError");

    glGetIntegerv = waffle_dl_sym(opts.dl, "glGetIntegerv");
    if (!glGetIntegerv)
        error_get_gl_symbol("glGetIntegerv");

    glGetString = waffle_dl_sym(opts.dl, "glGetString");
    if (!glGetString)
        error_get_gl_symbol("glGetString");

    const struct wflinfo_config_attrs config_attrs = {
        .api = opts.context_api,
        .profile = opts.context_profile,
        .major = opts.context_major,
        .minor = opts.context_minor,
        .forward_compat = opts.context_forward_compatible,
        .debug = opts.context_debug,
    };

    wflinfo_create_context(dpy, config_attrs, &ctx, &config);

    window = waffle_window_create(config, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!window)
        error_waffle();

    ok = waffle_make_current(dpy, window, ctx);
    if (!ok)
        error_waffle();

    glGetStringi = waffle_get_proc_address("glGetStringi");

    ok = print_wflinfo(&opts);
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

    return EXIT_SUCCESS;
}
