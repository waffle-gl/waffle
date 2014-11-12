// Copyright 2012, 2014 Intel Corporation
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
///
/// This little example program renders to an X11 window with EGL and an OpenGL
/// ES2 context.  The program does not include any OpenGL headers and does not
/// need to link to libGL. It declares the needed OpenGL symbols itself and
/// obtains the OpenGL functions with waffle_dl_sym().
///
/// This example is too simple; it does not perform proper error checking. For
/// a complete example with error checking, see gl_basic.c.

#include <stdlib.h>
#include <unistd.h>

#include <waffle.h>

// ---------------------------------------------
// OpenGL symbols
// ---------------------------------------------

typedef float GLclampf;
typedef unsigned int GLbitfield;

enum {
    GL_COLOR_BUFFER_BIT = 0x00004000,
};

#ifndef _WIN32
#define APIENTRY
#else
#ifndef APIENTRY
#define APIENTRY __stdcall
#endif
#endif

static void (APIENTRY *glClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
static void (APIENTRY *glClear)(GLbitfield mask);

// ---------------------------------------------

int
main()
{
    struct waffle_display *dpy;
    struct waffle_config *config;
    struct waffle_window *window;
    struct waffle_context *ctx;

    const int32_t init_attrs[] = {
        WAFFLE_PLATFORM, WAFFLE_PLATFORM_X11_EGL,
        0,
    };

    const int32_t config_attrs[] = {
        WAFFLE_CONTEXT_API,         WAFFLE_CONTEXT_OPENGL_ES2,

        WAFFLE_RED_SIZE,            8,
        WAFFLE_BLUE_SIZE,           8,
        WAFFLE_GREEN_SIZE,          8,

        0,
    };

    const int32_t window_width = 320;
    const int32_t window_height = 240;

    waffle_init(init_attrs);
    dpy = waffle_display_connect(NULL);

    // Exit if OpenGL ES2 is unsupported.
    if (!waffle_display_supports_context_api(dpy, WAFFLE_CONTEXT_OPENGL_ES2)
        || !waffle_dl_can_open(WAFFLE_DL_OPENGL_ES2))
    {
        exit(EXIT_FAILURE);
    }

    // Get OpenGL functions.
    glClearColor = waffle_dl_sym(WAFFLE_DL_OPENGL_ES2, "glClearColor");
    glClear = waffle_dl_sym(WAFFLE_DL_OPENGL_ES2, "glClear");

    config = waffle_config_choose(dpy, config_attrs);
    window = waffle_window_create(config, window_width, window_height);
    ctx = waffle_context_create(config, NULL);

    waffle_window_show(window);
    waffle_make_current(dpy, window, ctx);

    // Draw red.
    glClearColor(1.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    waffle_window_swap_buffers(window);
    sleep(1); // Sleep so that user can see window.
    
    // Draw green.
    glClearColor(0.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    waffle_window_swap_buffers(window);
    sleep(1);

    // Draw blue.
    glClearColor(0.0, 0.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    waffle_window_swap_buffers(window);
    sleep(1);

    // Clean up.
    waffle_make_current(dpy, NULL, NULL);
    waffle_window_destroy(window);
    waffle_context_destroy(ctx);
    waffle_config_destroy(config);
    waffle_display_disconnect(dpy);

    return 0;
}
