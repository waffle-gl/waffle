// Copyright 2012 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <waffle/waffle.h>

static const char *usage_message =
    "usage:\n"
    "    gl_basic <platform> <gl_api>\n"
    "\n"
    "arguments:\n"
    "    platform: One of android, cocoa, glx, wayland, x11_egl.\n"
    "    gl_api: One of gl, gles1, gles2.\n"
    "\n"
    "example:\n"
    "    gl_basic glx gl\n"
    "\n"
    "description:\n"
    "    Create a window. Fill it with red, green, then blue.\n";

/// @defgroup Error handlers
/// @{
///
/// All error handlers exit.
///

static void
error_usage(void)
{
    fprintf(stderr, "usage error\n\n");
    fprintf(stderr, usage_message);
    exit(EXIT_FAILURE);
}


static void
error_waffle(void)
{
    int32_t code;
    char buf[1024];
    const size_t buf_size = 1024;
    size_t message_length;

    code = waffle_get_error_m(buf, buf_size, &message_length);

    fflush(stdout);
    fprintf(stderr, "waffle: error: %s", waffle_error_to_string(code));
    if (message_length > 0)
        fprintf(stderr, ": %s", buf);
    fprintf(stderr, "\n");

    exit(EXIT_FAILURE);
}

static void
error_get_gl_symbol(const char *name)
{
    fflush(stdout);
    fprintf(stderr,
            "gl_basic: error: failed to get function pointer for %s\n", name);
    exit(EXIT_FAILURE);
}

/// @}
/// @defgroup GL decalrations
/// @{

typedef float GLclampf;
typedef unsigned int GLbitfield;

enum {
    // Copied for <GL/gl.h>.
    GL_COLOR_BUFFER_BIT = 0x00004000,
};

static void (*glClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
static void (*glClear)(GLbitfield mask);

/// @}
/// @defgroup Parsing Options
/// @{

struct options {
    int platform;
    int gl_api;
    const char *display_name;
};

struct enum_map {
    int i;
    const char *s;
};

static const struct enum_map platform_map[] = {
    {WAFFLE_PLATFORM_ANDROID,   "android"       },
    {WAFFLE_PLATFORM_COCOA,     "cocoa",        },
    {WAFFLE_PLATFORM_GLX,       "glx"           },
    {WAFFLE_PLATFORM_WAYLAND,   "wayland"       },
    {WAFFLE_PLATFORM_X11_EGL,   "x11_egl"       },
    {0,                         0               },
};

static const struct enum_map gl_api_map[] = {
    {WAFFLE_OPENGL,             "gl"            },
    {WAFFLE_OPENGL_ES1,         "gles1"         },
    {WAFFLE_OPENGL_ES2,         "gles2"         },
    {0,                         0               },
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
    const struct enum_map *i;

    for (i = self; i->i != 0; ++i) {
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

    if (argc != 3)
        error_usage();

    // Set platform.
    arg = argv[1];
    ok = enum_map_translate_str(platform_map, arg, &opts->platform);
    if (!ok) {
        fprintf(stderr, "error: '%s' is not a valid platform", arg);
        return false;
    }

    // Set gl_api.
    arg = argv[2];
    ok = enum_map_translate_str(gl_api_map, arg, &opts->gl_api);
    if (!ok) {
        fprintf(stderr, "error: '%s' is not a valid GL API", arg);
        return false;
    }

    // Set display_name.
    switch (opts->platform) {
        case WAFFLE_PLATFORM_GLX:
        case WAFFLE_PLATFORM_X11_EGL:
            opts->display_name = getenv("DISPLAY");
            break;
        case WAFFLE_PLATFORM_WAYLAND:
            opts->display_name = getenv("WAYLAND_DISPLAY");
            break;
        default:
            opts->display_name = NULL;
            break;
    }

    return true;
}

/// @}

/// Call free() to release result.
static int32_t*
make_waffle_attrib_list(int platform, int gl_api)
{
    int32_t *result = malloc(5 * sizeof(int32_t));
    if (!result) {
        fprintf(stderr, "error: out of memory\n");
        return false;
    }

    result[0] = WAFFLE_PLATFORM;
    result[1] = platform;
    result[2] = WAFFLE_OPENGL_API;
    result[3] = gl_api;
    result[4] = 0;

    return result;
}

static bool
draw(struct waffle_window *window)
{
    int i;
    bool ok;

    static const struct timespec sleep_time = {
         // 0.5 sec
        .tv_sec = 0,
        .tv_nsec = 500000000,
    };

    for (i = 0; i < 3; ++i) {
        switch (i) {
            case 0: glClearColor(1, 0, 0, 1); break;
            case 1: glClearColor(0, 1, 0, 1); break;
            case 2: glClearColor(0, 0, 1, 1); break;
            case 3: abort(); break;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        ok = waffle_window_swap_buffers(window);
        if (!ok)
            return false;
        nanosleep(&sleep_time, NULL);
    }

    return true;
}

int
main(int argc, char **argv)
{
    bool ok;
    struct options opts;
    int32_t *init_attribs;
    struct waffle_display *dpy;
    const int32_t config_attribs[] = {
        WAFFLE_RED_SIZE,        8,
        WAFFLE_GREEN_SIZE,      8,
        WAFFLE_BLUE_SIZE,       8,
        WAFFLE_DOUBLE_BUFFERED, 1,
        0,
    };
    struct waffle_config *config;
    struct waffle_context *ctx;
    struct waffle_window *window;

    ok = parse_args(argc, argv, &opts);
    if (!ok)
        exit(EXIT_FAILURE);

    init_attribs = make_waffle_attrib_list(opts.platform, opts.gl_api);
    if (!init_attribs)
        error_waffle();

    ok = waffle_init(init_attribs);
    free(init_attribs);
    if (!ok)
        error_waffle();

    glClear = waffle_dlsym_gl("glClear");
    if (!glClear)
        error_get_gl_symbol("glClear");

    glClearColor = waffle_dlsym_gl("glClearColor");
    if (!glClearColor)
        error_get_gl_symbol("glClearColor");

    dpy = waffle_display_connect(opts.display_name);
    if (!dpy)
        error_waffle();

    config = waffle_config_choose(dpy, config_attribs);
    if (!config)
        error_waffle();

    ctx = waffle_context_create(config, NULL);
    if (!ctx)
        error_waffle();

    window = waffle_window_create(config, 320, 240);
    if (!window)
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

    ok = waffle_finish();
    if (!ok)
        error_waffle();

    return EXIT_SUCCESS;
}
