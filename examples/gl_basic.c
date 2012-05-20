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
    "    gl_basic <platform> <context_api>\n"
    "\n"
    "arguments:\n"
    "    platform: One of android, cocoa, glx, wayland, x11_egl.\n"
    "    context_api: One of gl, gles1, gles2.\n"
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
    fprintf(stderr, "%s", usage_message);
    exit(EXIT_FAILURE);
}


static void
error_waffle(void)
{
    const struct waffle_error_info *info = waffle_error_get_info();

    fflush(stdout);
    fprintf(stderr, "waffle: error: %s", waffle_error_to_string(info->code));
    if (info->message_length > 0)
        fprintf(stderr, ": %s", info->message);
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
    /// @brief One of `WAFFLE_PLATFORM_*`.
    int platform;

    /// @brief One of `WAFFLE_CONTEXT_OPENGL_*`.
    int context_api;

    /// @brief One of `WAFFLE_DL_*`.
    int dl;

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

static const struct enum_map context_api_map[] = {
    {WAFFLE_CONTEXT_OPENGL,         "gl"        },
    {WAFFLE_CONTEXT_OPENGL_ES1,     "gles1"     },
    {WAFFLE_CONTEXT_OPENGL_ES2,     "gles2"     },
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

    // Set context_api.
    arg = argv[2];
    ok = enum_map_translate_str(context_api_map, arg, &opts->context_api);
    if (!ok) {
        fprintf(stderr, "error: '%s' is not a valid API for a GL context", arg);
        return false;
    }

    // Set dl.
    switch (opts->context_api) {
        case WAFFLE_CONTEXT_OPENGL:     opts->dl = WAFFLE_DL_OPENGL;      break;
        case WAFFLE_CONTEXT_OPENGL_ES1: opts->dl = WAFFLE_DL_OPENGL_ES1;  break;
        case WAFFLE_CONTEXT_OPENGL_ES2: opts->dl = WAFFLE_DL_OPENGL_ES2;  break;
        default:
            abort();
            break;
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
    int i;

    struct options opts;

    int32_t init_attrib_list[3];
    int32_t config_attrib_list[11];

    struct waffle_display *dpy;
    struct waffle_config *config;
    struct waffle_context *ctx;
    struct waffle_window *window;

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

    glClear = waffle_dl_sym(opts.dl, "glClear");
    if (!glClear)
        error_get_gl_symbol("glClear");

    glClearColor = waffle_dl_sym(opts.dl, "glClearColor");
    if (!glClearColor)
        error_get_gl_symbol("glClearColor");

    dpy = waffle_display_connect(opts.display_name);
    if (!dpy)
        error_waffle();

    i = 0;
    config_attrib_list[i++] = WAFFLE_CONTEXT_API;
    config_attrib_list[i++] = opts.context_api;
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

    window = waffle_window_create(config, 320, 240);
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

    return EXIT_SUCCESS;
}
