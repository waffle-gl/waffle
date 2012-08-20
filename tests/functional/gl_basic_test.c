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

/// @file
/// @brief Test basic GL rendering with all platform/gl_api combinations.
///
/// Each test does the following:
///     1. Initialize waffle with a platform and gl_api.
///     2. Create a context and window.
///     3. On the window call waffle_make_current, glClear,
///        and waffle_swap_buffers.
///     4. Verify the window contents with glReadPixels.
///     5. Tear down all waffle state.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "waffle.h"
#include <waffle_test/waffle_test.h>

#include "gl_basic_cocoa.h"

enum {
    // Choosing a smaller window would shorten the execution time of pixel
    // validation, but Windows 7 enforces a minimum size.
    WINDOW_WIDTH    = 320,
    WINDOW_HEIGHT   = 240,
};

static const float      RED_F       = 1.00;
static const float      GREEN_F     = 0.00;
static const float      BLUE_F      = 1.00;
static const float      ALPHA_F     = 1.00;

static const uint8_t    RED_UB      = 0xff;
static const uint8_t    GREEN_UB    = 0x00;
static const uint8_t    BLUE_UB     = 0xff;
static const uint8_t    ALPHA_UB    = 0xff;

static uint8_t pixels[4 * WINDOW_WIDTH * WINDOW_HEIGHT];

#define ASSERT_GL(statement) \
    do { \
        statement; \
        ASSERT_TRUE(!glGetError()); \
    } while (0)

/// Use this when @a cond will corrupt future tests.
#define ABORT_IF(cond) \
    do { \
        if (cond) { \
            TEST_ERROR_PRINTF("expect %x; aborting...", #cond); \
            abort(); \
        } \
    } while (0)

/// @}
/// @defgroup GL declarations
/// @{

typedef unsigned int        GLenum;
typedef unsigned char       GLboolean;
typedef unsigned int        GLbitfield;
typedef void                GLvoid;
typedef signed char         GLbyte;     /* 1-byte signed */
typedef short               GLshort;    /* 2-byte signed */
typedef int                 GLint;      /* 4-byte signed */
typedef unsigned char       GLubyte;    /* 1-byte unsigned */
typedef unsigned short      GLushort;   /* 2-byte unsigned */
typedef unsigned int        GLuint;     /* 4-byte unsigned */
typedef int                 GLsizei;    /* 4-byte signed */
typedef float               GLfloat;    /* single precision float */
typedef float               GLclampf;   /* single precision float in [0,1] */
typedef double              GLdouble;   /* double precision float */
typedef double              GLclampd;   /* double precision float in [0,1] */

#define GL_UNSIGNED_BYTE            0x1401
#define GL_UNSIGNED_INT             0x1405
#define GL_FLOAT                    0x1406
#define GL_RGB                      0x1907
#define GL_RGBA                     0x1908
#define GL_COLOR_BUFFER_BIT         0x00004000


static GLenum (*glGetError)(void);
static void (*glClearColor)(GLclampf red,
                            GLclampf green,
                            GLclampf blue,
                            GLclampf alpha);
static void (*glClear)(GLbitfield mask);
static void (*glReadPixels)(GLint x, GLint y,
                            GLsizei width, GLsizei height,
                            GLenum format, GLenum type,
                            GLvoid *pixels );

/// @}

static void
testgroup_gl_basic_setup(void)
{
    memset(pixels, 0, sizeof(pixels));
}

static void
testgroup_gl_basic_teardown(void)
{
    // empty
}

static int32_t
libgl_from_context_api(int32_t waffle_context_api)
{
    switch (waffle_context_api) {
        case WAFFLE_CONTEXT_OPENGL:     return WAFFLE_DL_OPENGL;
        case WAFFLE_CONTEXT_OPENGL_ES1: return WAFFLE_DL_OPENGL_ES1;
        case WAFFLE_CONTEXT_OPENGL_ES2: return WAFFLE_DL_OPENGL_ES2;

        default:
            TEST_FAIL();
            return 0;
    }
}

static void
gl_basic_init(int32_t waffle_platform)
{
    const int32_t init_attrib_list[] = {
        WAFFLE_PLATFORM, waffle_platform,
        0,
    };

    ASSERT_TRUE(waffle_init(init_attrib_list));
}

static void
gl_basic_draw(int32_t waffle_context_api, int32_t alpha)
{
    int32_t libgl;

    static const int config_context_api_index = 1;

    int32_t config_attrib_list[] = {
        WAFFLE_CONTEXT_API,     3141592653,

        WAFFLE_RED_SIZE,        8,
        WAFFLE_GREEN_SIZE,      8,
        WAFFLE_BLUE_SIZE,       8,
        WAFFLE_ALPHA_SIZE,      alpha,
        0,
    };

    struct waffle_display *dpy = NULL;
    struct waffle_config *config = NULL;
    struct waffle_window *window = NULL;
    struct waffle_context *ctx = NULL;

    libgl = libgl_from_context_api(waffle_context_api);
    config_attrib_list[config_context_api_index] = waffle_context_api;

    // Check that we've set the EGL_PLATFORM environment variable for Mesa.
    //
    // If Mesa's libEGL is built with support for multiple platforms, then the
    // environment variable EGL_PLATFORM must be set before the first EGL
    // call. Otherwise, libEGL may initialize itself with the incorrect
    // platform. In my experiments, first calling eglGetProcAddress will
    // produce a segfault in eglInitialize.
    waffle_get_proc_address("glClear");

    // Create objects.
    ASSERT_TRUE(dpy = waffle_display_connect(NULL));

    config = waffle_config_choose(dpy, config_attrib_list);
    if (!config) {
        if (waffle_error_get_code() == WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM)
            TEST_SKIP();
        else
            TEST_FAIL();
    }

    ASSERT_TRUE(window = waffle_window_create(config,
                                              WINDOW_WIDTH, WINDOW_HEIGHT));
    ASSERT_TRUE(waffle_window_show(window));

    ctx = waffle_context_create(config, NULL);
    if (!ctx) {
        if (waffle_error_get_code() == WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM)
            TEST_SKIP();
        else
            TEST_FAIL();
    }

    // Get GL functions.
    ASSERT_TRUE(glClear         = waffle_dl_sym(libgl, "glClear"));
    ASSERT_TRUE(glClearColor    = waffle_dl_sym(libgl, "glClearColor"));
    ASSERT_TRUE(glGetError      = waffle_dl_sym(libgl, "glGetError"));
    ASSERT_TRUE(glReadPixels    = waffle_dl_sym(libgl, "glReadPixels"));

    // Draw.
    ASSERT_TRUE(waffle_make_current(dpy, window, ctx));
    ASSERT_GL(glClearColor(RED_F, GREEN_F, BLUE_F, ALPHA_F));
    ASSERT_GL(glClear(GL_COLOR_BUFFER_BIT));
    ASSERT_GL(glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                           GL_RGBA, GL_UNSIGNED_BYTE,
                           pixels));
    ASSERT_TRUE(waffle_window_swap_buffers(window));

    // Probe color buffer.
    //
    // Fail at first failing pixel. If the draw fails, we don't want a terminal
    // filled with error messages.
    for (int y = 0 ; y < WINDOW_HEIGHT; ++y) {
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            uint8_t *p = &pixels[4 * (y * WINDOW_WIDTH + x)];
            ASSERT_TRUE(p[0] == RED_UB);
            ASSERT_TRUE(p[1] == GREEN_UB);
            ASSERT_TRUE(p[2] == BLUE_UB);
            ASSERT_TRUE(p[3] == ALPHA_UB);
        }
    }

    // Teardown.
    ABORT_IF(!waffle_make_current(dpy, NULL, NULL));
    ASSERT_TRUE(waffle_window_destroy(window));
    ASSERT_TRUE(waffle_context_destroy(ctx));
    ASSERT_TRUE(waffle_config_destroy(config));
    ASSERT_TRUE(waffle_display_disconnect(dpy));
}

#ifdef WAFFLE_HAS_CGL
TEST(gl_basic, cgl_init)
{
    gl_basic_init(WAFFLE_PLATFORM_CGL);
}

TEST(gl_basic, cgl_gl_rgb)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL, 0);
}

TEST(gl_basic, cgl_gl_rgba)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL, 1);
}

static void
testsuite_cgl(void)
{
    TEST_RUN(gl_basic, cgl_init);
    TEST_RUN(gl_basic, cgl_gl_rgb);
    TEST_RUN(gl_basic, cgl_gl_rgba);
}
#endif // WAFFLE_HAS_CGL

#ifdef WAFFLE_HAS_GLX
TEST(gl_basic, glx_init)
{
    gl_basic_init(WAFFLE_PLATFORM_GLX);
}

TEST(gl_basic, glx_gl_rgb)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL, 0);
}

TEST(gl_basic, glx_gles1_rgb)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL_ES1, 0);
}

TEST(gl_basic, glx_gles2_rgb)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL_ES2, 0);
}

TEST(gl_basic, glx_gl_rgba)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL, 1);
}

TEST(gl_basic, glx_gles1_rgba)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL_ES1, 1);
}

TEST(gl_basic, glx_gles2_rgba)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL_ES2, 1);
}

static void
testsuite_glx(void)
{
    TEST_RUN(gl_basic, glx_init);
    TEST_RUN(gl_basic, glx_gl_rgb);
    TEST_RUN(gl_basic, glx_gles1_rgb);
    TEST_RUN(gl_basic, glx_gles2_rgb);
    TEST_RUN(gl_basic, glx_gl_rgba);
    TEST_RUN(gl_basic, glx_gles1_rgba);
    TEST_RUN(gl_basic, glx_gles2_rgba);
}
#endif // WAFFLE_HAS_GLX

#ifdef WAFFLE_HAS_WAYLAND
TEST(gl_basic, wayland_init)
{
    gl_basic_init(WAFFLE_PLATFORM_WAYLAND);
}

TEST(gl_basic, wayland_gl_rgb)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL, 0);
}

TEST(gl_basic, wayland_gles1_rgb)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL_ES1, 0);
}

TEST(gl_basic, wayland_gles2_rgb)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL_ES2, 0);
}

TEST(gl_basic, wayland_gl_rgba)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL, 1);
}

TEST(gl_basic, wayland_gles1_rgba)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL_ES1, 1);
}

TEST(gl_basic, wayland_gles2_rgba)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL_ES2, 1);
}

static void
testsuite_wayland(void)
{
    TEST_RUN(gl_basic, wayland_init);
    TEST_RUN(gl_basic, wayland_gl_rgb);
    TEST_RUN(gl_basic, wayland_gles1_rgb);
    TEST_RUN(gl_basic, wayland_gles2_rgb);
    TEST_RUN(gl_basic, wayland_gl_rgba);
    TEST_RUN(gl_basic, wayland_gles1_rgba);
    TEST_RUN(gl_basic, wayland_gles2_rgba);
}
#endif // WAFFLE_HAS_WAYLAND

#ifdef WAFFLE_HAS_X11_EGL
TEST(gl_basic, x11_egl_init)
{
    gl_basic_init(WAFFLE_PLATFORM_X11_EGL);
}

TEST(gl_basic, x11_egl_gl_rgb)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL, 0);
}

TEST(gl_basic, x11_egl_gles1_rgb)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL_ES1, 0);
}

TEST(gl_basic, x11_egl_gles2_rgb)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL_ES2, 0);
}

TEST(gl_basic, x11_egl_gl_rgba)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL, 1);
}

TEST(gl_basic, x11_egl_gles1_rgba)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL_ES1, 1);
}

TEST(gl_basic, x11_egl_gles2_rgba)
{
    gl_basic_draw(WAFFLE_CONTEXT_OPENGL_ES2, 1);
}

static void
testsuite_x11_egl(void)
{
    TEST_RUN(gl_basic, x11_egl_init);
    TEST_RUN(gl_basic, x11_egl_gl_rgb);
    TEST_RUN(gl_basic, x11_egl_gles1_rgb);
    TEST_RUN(gl_basic, x11_egl_gles2_rgb);
    TEST_RUN(gl_basic, x11_egl_gl_rgba);
    TEST_RUN(gl_basic, x11_egl_gles1_rgba);
    TEST_RUN(gl_basic, x11_egl_gles2_rgba);
}
#endif // WAFFLE_HAS_X11_EGL


static void
usage_error(void)
{
    fprintf(stderr, "gl_basic_test: usage error\n");
    exit(1);
}

/// Run the testsuite in a separate process. If the testsuite fails, then exit
/// immediately.
static void
run_testsuite(void (*testsuite)(void))
{
    pid_t pid = fork();
    int status;

    if (pid > 0) {
        waitpid(pid, &status, 0);
        int exit_status = WEXITSTATUS(status);
        // exit_status is number of failures.
        if (exit_status == 0) {
            // All tests passed.
            return;
        }
        else if (exit_status > 0) {
            // Some tests failed. Don't run any more tests.
            exit(exit_status);
        }
        else {
            fprintf(stderr, "gl_basic_test: error: child process aborted\n");
            abort();
        }
    }
    else if (pid == 0) {
        #ifdef __APPLE__
            gl_basic_cocoa_init();
        #endif

        void (*testsuites[])(void) = {testsuite, 0};
        int argc = 0;
        int num_failures = wt_main(&argc, NULL, testsuites);

        #ifdef __APPLE__
            gl_basic_cocoa_finish();
        #endif

        exit(num_failures);
    }
    else {
        fprintf(stderr, "gl_basic_test: error: fork failed\n");
        abort();
    }
}

int
main(int argc, char *argv[])
{
    if (argc != 1)
        usage_error();

#ifdef WAFFLE_HAS_CGL
    run_testsuite(testsuite_cgl);
#endif
#ifdef WAFFLE_HAS_GLX
    run_testsuite(testsuite_glx);
#endif
#ifdef WAFFLE_HAS_WAYLAND
    run_testsuite(testsuite_wayland);
#endif
#ifdef WAFFLE_HAS_X11_EGL
    run_testsuite(testsuite_x11_egl);
#endif

   return 0;
}
