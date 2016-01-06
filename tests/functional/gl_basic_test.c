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
/// @brief Test basic OpenGL rendering with all platform/gl_api combinations.
///
/// Each test does the following:
///     1. Initialize waffle with a platform and gl_api.
///     2. Create a context and window.
///     3. On the window call waffle_make_current, glClear,
///        and waffle_swap_buffers.
///     4. Verify the window contents with glReadPixels.
///     5. Tear down all waffle state.

#include <stdarg.h> // for va_start, va_end
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#if !defined(_WIN32)
#include <unistd.h>
#include <sys/wait.h>
#else
#include <windows.h>
#endif

#include "waffle.h"
#include "waffle_test/waffle_test.h"

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

static uint8_t actual_pixels[4 * WINDOW_WIDTH * WINDOW_HEIGHT];
static uint8_t expect_pixels[4 * WINDOW_WIDTH * WINDOW_HEIGHT];

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

#define GL_VERSION                  0x1F02
#define GL_UNSIGNED_BYTE            0x1401
#define GL_UNSIGNED_INT             0x1405
#define GL_FLOAT                    0x1406
#define GL_RGB                      0x1907
#define GL_RGBA                     0x1908
#define GL_COLOR_BUFFER_BIT         0x00004000
#define GL_CONTEXT_FLAGS            0x821e

#define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT 0x00000001
#define GL_CONTEXT_FLAG_DEBUG_BIT              0x00000002

#ifndef _WIN32
#define APIENTRY
#else
#ifndef APIENTRY
#define APIENTRY __stdcall
#endif
#endif

static GLenum (APIENTRY *glGetError)(void);
static const GLubyte *(APIENTRY *glGetString)(GLenum name);
static void (APIENTRY *glGetIntegerv)(GLenum pname, GLint *params);
static void (APIENTRY *glClearColor)(GLclampf red,
                                     GLclampf green,
                                     GLclampf blue,
                                     GLclampf alpha);
static void (APIENTRY *glClear)(GLbitfield mask);
static void (APIENTRY *glReadPixels)(GLint x, GLint y,
                                     GLsizei width, GLsizei height,
                                     GLenum format, GLenum type,
                                     GLvoid *pixels );

static void
testgroup_gl_basic_setup(void)
{
    for (int y = 0 ; y < WINDOW_HEIGHT; ++y) {
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            uint8_t *p = &expect_pixels[4 * (y * WINDOW_WIDTH + x)];
            p[0] = RED_UB;
            p[1] = GREEN_UB;
            p[2] = BLUE_UB;
            p[3] = ALPHA_UB;
        }
    }

    memset(actual_pixels, 0x99, sizeof(actual_pixels));
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
        case WAFFLE_CONTEXT_OPENGL_ES3: return WAFFLE_DL_OPENGL_ES3;

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

#define gl_basic_draw(...) \
    \
    gl_basic_draw__((struct gl_basic_draw_args__) { \
        .api = 0, \
        .version = WAFFLE_DONT_CARE, \
        .profile = WAFFLE_DONT_CARE, \
        .forward_compatible = false, \
        .debug = false, \
        .alpha = false, \
        .expect_error = WAFFLE_NO_ERROR, \
        __VA_ARGS__ \
        })

struct gl_basic_draw_args__ {
    int32_t api;
    int32_t version;
    int32_t profile;
    int32_t expect_error;
    bool forward_compatible;
    bool debug;
    bool alpha;
};

static void
gl_basic_draw__(struct gl_basic_draw_args__ args)
{
    int32_t waffle_context_api = args.api;
    int32_t context_version = args.version;
    int32_t context_profile = args.profile;
    int32_t expect_error = args.expect_error;
    bool context_forward_compatible = args.forward_compatible;
    bool context_debug = args.debug;
    bool alpha = args.alpha;

    int32_t libgl;

    int32_t config_attrib_list[64];
    int i;

    struct waffle_display *dpy = NULL;
    struct waffle_config *config = NULL;
    struct waffle_window *window = NULL;
    struct waffle_context *ctx = NULL;

    const intptr_t window_attrib_list[] = {
        WAFFLE_WINDOW_WIDTH,    WINDOW_WIDTH,
        WAFFLE_WINDOW_HEIGHT,   WINDOW_HEIGHT,
        0,
    };

    libgl = libgl_from_context_api(waffle_context_api);

    i = 0;
    config_attrib_list[i++] = WAFFLE_CONTEXT_API;
    config_attrib_list[i++] = waffle_context_api;
    if (context_version != WAFFLE_DONT_CARE) {
        config_attrib_list[i++] = WAFFLE_CONTEXT_MAJOR_VERSION;
        config_attrib_list[i++] = context_version / 10;
        config_attrib_list[i++] = WAFFLE_CONTEXT_MINOR_VERSION;
        config_attrib_list[i++] = context_version % 10;
    }
    if (context_profile != WAFFLE_DONT_CARE) {
        config_attrib_list[i++] = WAFFLE_CONTEXT_PROFILE;
        config_attrib_list[i++] = context_profile;
    }
    if (context_forward_compatible) {
        config_attrib_list[i++] = WAFFLE_CONTEXT_FORWARD_COMPATIBLE;
        config_attrib_list[i++] = true;
    }
    if (context_debug) {
        config_attrib_list[i++] = WAFFLE_CONTEXT_DEBUG;
        config_attrib_list[i++] = true;
    }
    config_attrib_list[i++] = WAFFLE_RED_SIZE;
    config_attrib_list[i++] = 8;
    config_attrib_list[i++] = WAFFLE_GREEN_SIZE;
    config_attrib_list[i++] = 8;
    config_attrib_list[i++] = WAFFLE_BLUE_SIZE;
    config_attrib_list[i++] = 8;
    config_attrib_list[i++] = WAFFLE_ALPHA_SIZE;
    config_attrib_list[i++] = alpha;
    config_attrib_list[i++] = 0;

    // Create objects.
    ASSERT_TRUE(dpy = waffle_display_connect(NULL));

    config = waffle_config_choose(dpy, config_attrib_list);
    if (expect_error) {
        ASSERT_TRUE(config == NULL);
        ASSERT_TRUE(waffle_error_get_code() == expect_error);
        TEST_PASS();
    } else if (config == NULL) {
        if (waffle_error_get_code() == WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM) {
            TEST_SKIP();
        }
        else if (waffle_error_get_code() == WAFFLE_ERROR_UNKNOWN) {
            // Assume that the native platform rejected the requested
            // context flavor.
            TEST_SKIP();
        }
        else {
            TEST_FAIL();
        }
    }

    ASSERT_TRUE(window = waffle_window_create2(config, window_attrib_list));
    ASSERT_TRUE(waffle_window_show(window));

    ctx = waffle_context_create(config, NULL);
    if (!ctx) {
        if (waffle_error_get_code() == WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM) {
            TEST_SKIP();
        }
        else if (waffle_error_get_code() == WAFFLE_ERROR_UNKNOWN) {
            // Assume that the native platform rejected the requested
            // context flavor.
            TEST_SKIP();
        }
        else {
            TEST_FAIL();
        }
    }

    // Get OpenGL functions.
    ASSERT_TRUE(glClear         = waffle_dl_sym(libgl, "glClear"));
    ASSERT_TRUE(glClearColor    = waffle_dl_sym(libgl, "glClearColor"));
    ASSERT_TRUE(glGetError      = waffle_dl_sym(libgl, "glGetError"));
    ASSERT_TRUE(glGetIntegerv   = waffle_dl_sym(libgl, "glGetIntegerv"));
    ASSERT_TRUE(glReadPixels    = waffle_dl_sym(libgl, "glReadPixels"));
    ASSERT_TRUE(glGetString     = waffle_dl_sym(libgl, "glGetString"));

    ASSERT_TRUE(waffle_make_current(dpy, window, ctx));

    ASSERT_TRUE(waffle_get_current_display() == dpy);
    ASSERT_TRUE(waffle_get_current_window() == window);
    ASSERT_TRUE(waffle_get_current_context() == ctx);

    const char *version_str;
    int major, minor, count;

    ASSERT_GL(version_str = (const char *) glGetString(GL_VERSION));
    ASSERT_TRUE(version_str != NULL);

    while (*version_str != '\0' && !isdigit(*version_str))
        version_str++;

    count = sscanf(version_str, "%d.%d", &major, &minor);
    ASSERT_TRUE(count == 2);
    ASSERT_TRUE(major >= 0);
    ASSERT_TRUE(minor >= 0 && minor < 10);

    int version_10x = 10 * major + minor;

    if ((waffle_context_api == WAFFLE_CONTEXT_OPENGL && version_10x >= 30) ||
        (waffle_context_api != WAFFLE_CONTEXT_OPENGL && version_10x >= 32)) {
        GLint context_flags = 0;
        if (context_forward_compatible || context_debug) {
            glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
        }

        if (context_forward_compatible) {
            ASSERT_TRUE(context_flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT);
        }

        if (context_debug) {
            ASSERT_TRUE(context_flags & GL_CONTEXT_FLAG_DEBUG_BIT);
        }
    }

    // Draw.
    ASSERT_GL(glClearColor(RED_F, GREEN_F, BLUE_F, ALPHA_F));
    ASSERT_GL(glClear(GL_COLOR_BUFFER_BIT));
    ASSERT_GL(glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                           GL_RGBA, GL_UNSIGNED_BYTE,
                           actual_pixels));
    ASSERT_TRUE(waffle_window_swap_buffers(window));

    // Probe color buffer.
    //
    // Fail at first failing pixel. If the draw fails, we don't want a terminal
    // filled with error messages.
    for (i = 0 ; i < sizeof(actual_pixels); ++i) {
        ASSERT_TRUE(actual_pixels[i] == expect_pixels[i]);
    }

    // Teardown.
    ABORT_IF(!waffle_make_current(dpy, NULL, NULL));

    ASSERT_TRUE(waffle_get_current_display() == dpy);
    ASSERT_TRUE(waffle_get_current_window() == NULL);
    ASSERT_TRUE(waffle_get_current_context() == NULL);

    ASSERT_TRUE(waffle_window_destroy(window));
    ASSERT_TRUE(waffle_context_destroy(ctx));
    ASSERT_TRUE(waffle_config_destroy(config));
    ASSERT_TRUE(waffle_display_disconnect(dpy));
}

//
// List of tests common to all platforms.
//

#define test_XX_rgb(context_api, waffle_api, error)                     \
static void test_gl_basic_##context_api##_rgb(void)                     \
{                                                                       \
    gl_basic_draw(.api=WAFFLE_CONTEXT_##waffle_api,                     \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_XX_rgba(context_api, waffle_api, error)                    \
static void test_gl_basic_##context_api##_rgba(void)                    \
{                                                                       \
    gl_basic_draw(.api=WAFFLE_CONTEXT_##waffle_api,                     \
                  .alpha=true,                                          \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_XX_fwdcompat(context_api, waffle_api, error)               \
static void test_gl_basic_##context_api##_fwdcompat(void)               \
{                                                                       \
    gl_basic_draw(.api=WAFFLE_CONTEXT_##waffle_api,                     \
                  .forward_compatible=true,                             \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_gl_debug(error)                                            \
static void test_gl_basic_gl_debug(void)                                \
{                                                                       \
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,                           \
                  .debug=true,                                          \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_glXX(waffle_version, error)                                \
static void test_gl_basic_gl##waffle_version(void)                      \
{                                                                       \
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,                           \
                  .version=waffle_version,                              \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_glXX_fwdcompat(waffle_version, error)                      \
static void test_gl_basic_gl##waffle_version##_fwdcompat(void)          \
{                                                                       \
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,                           \
                  .version=waffle_version,                              \
                  .forward_compatible=true,                             \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_glXX_core(waffle_version, error)                           \
static void test_gl_basic_gl##waffle_version##_core(void)               \
{                                                                       \
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,                           \
                  .version=waffle_version,                              \
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE,                 \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_glXX_core_fwdcompat(waffle_version, error)                 \
static void test_gl_basic_gl##waffle_version##_core_fwdcompat(void)     \
{                                                                       \
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,                           \
                  .version=waffle_version,                              \
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE,                 \
                  .forward_compatible=true,                             \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_glXX_compat(waffle_version, error)                         \
static void test_gl_basic_gl##waffle_version##_compat(void)             \
{                                                                       \
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,                           \
                  .version=waffle_version,                              \
                  .profile=WAFFLE_CONTEXT_COMPATIBILITY_PROFILE,        \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_glesXX(api_version, waffle_version, error)                 \
static void test_gl_basic_gles##waffle_version(void)                    \
{                                                                       \
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL_ES##api_version,           \
                  .version=waffle_version,                              \
                  .expect_error=WAFFLE_##error);                        \
}

test_XX_rgb(gl, OPENGL, NO_ERROR)
test_XX_rgba(gl, OPENGL, NO_ERROR)

test_glXX(10, NO_ERROR)
test_glXX(11, NO_ERROR)
test_glXX(12, NO_ERROR)
test_glXX(13, NO_ERROR)
test_glXX(14, NO_ERROR)
test_glXX(15, NO_ERROR)
test_glXX(20, NO_ERROR)
test_glXX(21, NO_ERROR)
test_glXX_fwdcompat(21, ERROR_BAD_ATTRIBUTE)


//
// List of linux (glx, wayland and x11_egl) and windows (wgl) specific tests.
//
#if defined(WAFFLE_HAS_GLX) || defined(WAFFLE_HAS_WAYLAND) || defined(WAFFLE_HAS_X11_EGL) || defined(WAFFLE_HAS_WGL)

test_XX_fwdcompat(gl, OPENGL, ERROR_BAD_ATTRIBUTE)
test_gl_debug(NO_ERROR)

test_glXX(30, NO_ERROR)
test_glXX_fwdcompat(30, NO_ERROR)
test_glXX(31, NO_ERROR)
test_glXX_fwdcompat(31, NO_ERROR)

test_glXX_core(32, NO_ERROR)
test_glXX_core_fwdcompat(32, NO_ERROR)
test_glXX_core(33, NO_ERROR)
test_glXX_core(40, NO_ERROR)
test_glXX_core(41, NO_ERROR)
test_glXX_core(42, NO_ERROR)
test_glXX_core(43, NO_ERROR)

test_glXX_compat(32, NO_ERROR)
test_glXX_compat(33, NO_ERROR)
test_glXX_compat(40, NO_ERROR)
test_glXX_compat(41, NO_ERROR)
test_glXX_compat(42, NO_ERROR)
test_glXX_compat(43, NO_ERROR)

test_XX_rgb(gles1, OPENGL_ES1, NO_ERROR)
test_XX_rgba(gles1, OPENGL_ES1, NO_ERROR)
test_XX_fwdcompat(gles1, OPENGL_ES1, ERROR_BAD_ATTRIBUTE)
test_glesXX(1, 10, NO_ERROR)
test_glesXX(1, 11, NO_ERROR)

test_XX_rgb(gles2, OPENGL_ES2, NO_ERROR)
test_XX_rgba(gles2, OPENGL_ES2, NO_ERROR)
test_XX_fwdcompat(gles2, OPENGL_ES2, ERROR_BAD_ATTRIBUTE)
test_glesXX(2, 20, NO_ERROR)

test_XX_rgb(gles3, OPENGL_ES3, NO_ERROR)
test_XX_rgba(gles3, OPENGL_ES3, NO_ERROR)
test_XX_fwdcompat(gles3, OPENGL_ES3, ERROR_BAD_ATTRIBUTE)
test_glesXX(3, 30, NO_ERROR)
#endif

#ifdef WAFFLE_HAS_CGL
TEST(gl_basic, cgl_init)
{
    gl_basic_init(WAFFLE_PLATFORM_CGL);
}

test_XX_fwdcompat(gl, OPENGL, ERROR_BAD_ATTRIBUTE)
test_gl_debug(ERROR_UNSUPPORTED_ON_PLATFORM)

test_glXX(30, ERROR_UNSUPPORTED_ON_PLATFORM)
test_glXX_fwdcompat(30, ERROR_UNSUPPORTED_ON_PLATFORM)
test_glXX(31, NO_ERROR)
test_glXX_fwdcompat(31, ERROR_UNSUPPORTED_ON_PLATFORM)

test_glXX_core(32, NO_ERROR)
test_glXX_core_fwdcompat(32, ERROR_UNSUPPORTED_ON_PLATFORM)
test_glXX_core(33, ERROR_UNSUPPORTED_ON_PLATFORM)
test_glXX_core(40, ERROR_UNSUPPORTED_ON_PLATFORM)
test_glXX_core(41, ERROR_UNSUPPORTED_ON_PLATFORM)
test_glXX_core(42, ERROR_UNSUPPORTED_ON_PLATFORM)
test_glXX_core(43, ERROR_UNSUPPORTED_ON_PLATFORM)

test_glXX_compat(32, ERROR_UNSUPPORTED_ON_PLATFORM)
test_glXX_compat(33, ERROR_UNSUPPORTED_ON_PLATFORM)
test_glXX_compat(40, ERROR_UNSUPPORTED_ON_PLATFORM)
test_glXX_compat(41, ERROR_UNSUPPORTED_ON_PLATFORM)
test_glXX_compat(42, ERROR_UNSUPPORTED_ON_PLATFORM)
test_glXX_compat(43, ERROR_UNSUPPORTED_ON_PLATFORM)

test_glesXX(1, 10, ERROR_UNSUPPORTED_ON_PLATFORM)
test_glesXX(2, 20, ERROR_UNSUPPORTED_ON_PLATFORM)

static void
testsuite_cgl(void)
{
    TEST_RUN(gl_basic, cgl_init);

    TEST_RUN(gl_basic, gl_rgb);
    TEST_RUN(gl_basic, gl_rgba);
    TEST_RUN(gl_basic, gl_fwdcompat);
    TEST_RUN(gl_basic, gl_debug);

    TEST_RUN(gl_basic, gl10);
    TEST_RUN(gl_basic, gl11);
    TEST_RUN(gl_basic, gl12);
    TEST_RUN(gl_basic, gl13);
    TEST_RUN(gl_basic, gl14);
    TEST_RUN(gl_basic, gl15);
    TEST_RUN(gl_basic, gl20);
    TEST_RUN(gl_basic, gl21);
    TEST_RUN(gl_basic, gl21_fwdcompat);

    TEST_RUN(gl_basic, gl30);
    TEST_RUN(gl_basic, gl30_fwdcompat);
    TEST_RUN(gl_basic, gl31);
    TEST_RUN(gl_basic, gl31_fwdcompat);

    TEST_RUN(gl_basic, gl32_core);
    TEST_RUN(gl_basic, gl32_core_fwdcompat);
    TEST_RUN(gl_basic, gl33_core);
    TEST_RUN(gl_basic, gl40_core);
    TEST_RUN(gl_basic, gl41_core);
    TEST_RUN(gl_basic, gl42_core);
    TEST_RUN(gl_basic, gl43_core);

    TEST_RUN(gl_basic, gl32_compat);
    TEST_RUN(gl_basic, gl33_compat);
    TEST_RUN(gl_basic, gl40_compat);
    TEST_RUN(gl_basic, gl41_compat);
    TEST_RUN(gl_basic, gl42_compat);
    TEST_RUN(gl_basic, gl43_compat);

    TEST_RUN(gl_basic, gles10);
    TEST_RUN(gl_basic, gles20);
}
#endif // WAFFLE_HAS_CGL

#ifdef WAFFLE_HAS_GLX
TEST(gl_basic, glx_init)
{
    gl_basic_init(WAFFLE_PLATFORM_GLX);
}

static void
testsuite_glx(void)
{
    TEST_RUN(gl_basic, glx_init);

    TEST_RUN(gl_basic, gl_rgb);
    TEST_RUN(gl_basic, gl_rgba);
    TEST_RUN(gl_basic, gl_fwdcompat);
    TEST_RUN(gl_basic, gl_debug);

    TEST_RUN(gl_basic, gl10);
    TEST_RUN(gl_basic, gl11);
    TEST_RUN(gl_basic, gl12);
    TEST_RUN(gl_basic, gl13);
    TEST_RUN(gl_basic, gl14);
    TEST_RUN(gl_basic, gl15);
    TEST_RUN(gl_basic, gl20);
    TEST_RUN(gl_basic, gl21);
    TEST_RUN(gl_basic, gl21_fwdcompat);

    TEST_RUN(gl_basic, gl30);
    TEST_RUN(gl_basic, gl30_fwdcompat);
    TEST_RUN(gl_basic, gl31);
    TEST_RUN(gl_basic, gl31_fwdcompat);

    TEST_RUN(gl_basic, gl32_core);
    TEST_RUN(gl_basic, gl32_core_fwdcompat);
    TEST_RUN(gl_basic, gl33_core);
    TEST_RUN(gl_basic, gl40_core);
    TEST_RUN(gl_basic, gl41_core);
    TEST_RUN(gl_basic, gl42_core);
    TEST_RUN(gl_basic, gl43_core);

    TEST_RUN(gl_basic, gl32_compat);
    TEST_RUN(gl_basic, gl33_compat);
    TEST_RUN(gl_basic, gl40_compat);
    TEST_RUN(gl_basic, gl41_compat);
    TEST_RUN(gl_basic, gl42_compat);
    TEST_RUN(gl_basic, gl43_compat);

    TEST_RUN(gl_basic, gles1_rgb);
    TEST_RUN(gl_basic, gles1_rgba);
    TEST_RUN(gl_basic, gles1_fwdcompat);
    TEST_RUN(gl_basic, gles10);
    TEST_RUN(gl_basic, gles11);

    TEST_RUN(gl_basic, gles2_rgb);
    TEST_RUN(gl_basic, gles2_rgba);
    TEST_RUN(gl_basic, gles2_fwdcompat);
    TEST_RUN(gl_basic, gles20);

    TEST_RUN(gl_basic, gles3_rgb);
    TEST_RUN(gl_basic, gles3_rgba);
    TEST_RUN(gl_basic, gles3_fwdcompat);
    TEST_RUN(gl_basic, gles30);
}
#endif // WAFFLE_HAS_GLX

#ifdef WAFFLE_HAS_WAYLAND
TEST(gl_basic, wayland_init)
{
    gl_basic_init(WAFFLE_PLATFORM_WAYLAND);
}

static void
testsuite_wayland(void)
{
    TEST_RUN(gl_basic, wayland_init);

    TEST_RUN(gl_basic, gl_rgb);
    TEST_RUN(gl_basic, gl_rgba);
    TEST_RUN(gl_basic, gl_fwdcompat);
    TEST_RUN(gl_basic, gl_debug);

    TEST_RUN(gl_basic, gl10);
    TEST_RUN(gl_basic, gl11);
    TEST_RUN(gl_basic, gl12);
    TEST_RUN(gl_basic, gl13);
    TEST_RUN(gl_basic, gl14);
    TEST_RUN(gl_basic, gl15);
    TEST_RUN(gl_basic, gl20);
    TEST_RUN(gl_basic, gl21);
    TEST_RUN(gl_basic, gl21_fwdcompat);

    TEST_RUN(gl_basic, gl30);
    TEST_RUN(gl_basic, gl30_fwdcompat);
    TEST_RUN(gl_basic, gl31);
    TEST_RUN(gl_basic, gl31_fwdcompat);

    TEST_RUN(gl_basic, gl32_core);
    TEST_RUN(gl_basic, gl32_core_fwdcompat);
    TEST_RUN(gl_basic, gl33_core);
    TEST_RUN(gl_basic, gl40_core);
    TEST_RUN(gl_basic, gl41_core);
    TEST_RUN(gl_basic, gl42_core);
    TEST_RUN(gl_basic, gl43_core);

    TEST_RUN(gl_basic, gl32_compat);
    TEST_RUN(gl_basic, gl33_compat);
    TEST_RUN(gl_basic, gl40_compat);
    TEST_RUN(gl_basic, gl41_compat);
    TEST_RUN(gl_basic, gl42_compat);
    TEST_RUN(gl_basic, gl43_compat);

    TEST_RUN(gl_basic, gles1_rgb);
    TEST_RUN(gl_basic, gles1_rgba);
    TEST_RUN(gl_basic, gles1_fwdcompat);
    TEST_RUN(gl_basic, gles10);
    TEST_RUN(gl_basic, gles11);

    TEST_RUN(gl_basic, gles2_rgb);
    TEST_RUN(gl_basic, gles2_rgba);
    TEST_RUN(gl_basic, gles2_fwdcompat);
    TEST_RUN(gl_basic, gles20);

    TEST_RUN(gl_basic, gles3_rgb);
    TEST_RUN(gl_basic, gles3_rgba);
    TEST_RUN(gl_basic, gles3_fwdcompat);
    TEST_RUN(gl_basic, gles30);
}
#endif // WAFFLE_HAS_WAYLAND

#ifdef WAFFLE_HAS_X11_EGL
TEST(gl_basic, x11_egl_init)
{
    gl_basic_init(WAFFLE_PLATFORM_X11_EGL);
}

static void
testsuite_x11_egl(void)
{
    TEST_RUN(gl_basic, x11_egl_init);

    TEST_RUN(gl_basic, gl_rgb);
    TEST_RUN(gl_basic, gl_rgba);
    TEST_RUN(gl_basic, gl_fwdcompat);
    TEST_RUN(gl_basic, gl_debug);

    TEST_RUN(gl_basic, gl10);
    TEST_RUN(gl_basic, gl11);
    TEST_RUN(gl_basic, gl12);
    TEST_RUN(gl_basic, gl13);
    TEST_RUN(gl_basic, gl14);
    TEST_RUN(gl_basic, gl15);
    TEST_RUN(gl_basic, gl20);
    TEST_RUN(gl_basic, gl21);
    TEST_RUN(gl_basic, gl21_fwdcompat);

    TEST_RUN(gl_basic, gl30);
    TEST_RUN(gl_basic, gl30_fwdcompat);
    TEST_RUN(gl_basic, gl31);
    TEST_RUN(gl_basic, gl31_fwdcompat);

    TEST_RUN(gl_basic, gl32_core);
    TEST_RUN(gl_basic, gl32_core_fwdcompat);
    TEST_RUN(gl_basic, gl33_core);
    TEST_RUN(gl_basic, gl40_core);
    TEST_RUN(gl_basic, gl41_core);
    TEST_RUN(gl_basic, gl42_core);
    TEST_RUN(gl_basic, gl43_core);

    TEST_RUN(gl_basic, gl32_compat);
    TEST_RUN(gl_basic, gl33_compat);
    TEST_RUN(gl_basic, gl40_compat);
    TEST_RUN(gl_basic, gl41_compat);
    TEST_RUN(gl_basic, gl42_compat);
    TEST_RUN(gl_basic, gl43_compat);

    TEST_RUN(gl_basic, gles1_rgb);
    TEST_RUN(gl_basic, gles1_rgba);
    TEST_RUN(gl_basic, gles1_fwdcompat);
    TEST_RUN(gl_basic, gles10);
    TEST_RUN(gl_basic, gles11);

    TEST_RUN(gl_basic, gles2_rgb);
    TEST_RUN(gl_basic, gles2_rgba);
    TEST_RUN(gl_basic, gles2_fwdcompat);
    TEST_RUN(gl_basic, gles20);

    TEST_RUN(gl_basic, gles3_rgb);
    TEST_RUN(gl_basic, gles3_rgba);
    TEST_RUN(gl_basic, gles3_fwdcompat);
    TEST_RUN(gl_basic, gles30);
}
#endif // WAFFLE_HAS_X11_EGL

#ifdef WAFFLE_HAS_WGL
TEST(gl_basic, wgl_init)
{
    gl_basic_init(WAFFLE_PLATFORM_WGL);
}

static void
testsuite_wgl(void)
{
    TEST_RUN(gl_basic, wgl_init);

    TEST_RUN(gl_basic, gl_rgb);
    TEST_RUN(gl_basic, gl_rgba);
    TEST_RUN(gl_basic, gl_fwdcompat);
    TEST_RUN(gl_basic, gl_debug);

    TEST_RUN(gl_basic, gl10);
    TEST_RUN(gl_basic, gl11);
    TEST_RUN(gl_basic, gl12);
    TEST_RUN(gl_basic, gl13);
    TEST_RUN(gl_basic, gl14);
    TEST_RUN(gl_basic, gl15);
    TEST_RUN(gl_basic, gl20);
    TEST_RUN(gl_basic, gl21);
    TEST_RUN(gl_basic, gl21_fwdcompat);

    TEST_RUN(gl_basic, gl30);
    TEST_RUN(gl_basic, gl30_fwdcompat);
    TEST_RUN(gl_basic, gl31);
    TEST_RUN(gl_basic, gl31_fwdcompat);

    TEST_RUN(gl_basic, gl32_core);
    TEST_RUN(gl_basic, gl32_core_fwdcompat);
    TEST_RUN(gl_basic, gl33_core);
    TEST_RUN(gl_basic, gl40_core);
    TEST_RUN(gl_basic, gl41_core);
    TEST_RUN(gl_basic, gl42_core);
    TEST_RUN(gl_basic, gl43_core);

    TEST_RUN(gl_basic, gl32_compat);
    TEST_RUN(gl_basic, gl33_compat);
    TEST_RUN(gl_basic, gl40_compat);
    TEST_RUN(gl_basic, gl41_compat);
    TEST_RUN(gl_basic, gl42_compat);
    TEST_RUN(gl_basic, gl43_compat);

    TEST_RUN(gl_basic, gles1_rgb);
    TEST_RUN(gl_basic, gles1_rgba);
    TEST_RUN(gl_basic, gles1_fwdcompat);
    TEST_RUN(gl_basic, gles10);
    TEST_RUN(gl_basic, gles11);

    TEST_RUN(gl_basic, gles2_rgb);
    TEST_RUN(gl_basic, gles2_rgba);
    TEST_RUN(gl_basic, gles2_fwdcompat);
    TEST_RUN(gl_basic, gles20);

    TEST_RUN(gl_basic, gles3_rgb);
    TEST_RUN(gl_basic, gles3_rgba);
    TEST_RUN(gl_basic, gles3_fwdcompat);
    TEST_RUN(gl_basic, gles30);
}
#endif // WAFFLE_HAS_WGL

#undef test_glesXX

#undef test_glXX_compat
#undef test_glXX_core_fwdcompat
#undef test_glXX_core
#undef test_glXX_fwdcompat
#undef test_glXX

#undef test_gl_debug
#undef test_XX_fwdcompat
#undef test_XX_rgba
#undef test_XX_rgb

static const char *usage_message =
    "Usage:\n"
    "    gl_basic_test <Required Parameter> [Options]\n"
    "\n"
    "Description:\n"
    "    Run the basic functionality tests.\n"
    "\n"
    "Required Parameter:\n"
    "    -p, --platform\n"
    "        One of: cgl, glx, wayland, wgl or x11_egl\n"
    "\n"
    "Options:\n"
    "    -h, --help\n"
    "        Print gl_basic_test usage information.\n"
    ;

#if defined(__GNUC__)
#define NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define NORETURN __declspec(noreturn)
#else
#define NORETURN
#endif

static void NORETURN
write_usage_and_exit(FILE *f, int exit_code)
{
    fprintf(f, "%s", usage_message);
    exit(exit_code);
}

enum {
    OPT_PLATFORM = 'p',
    OPT_HELP = 'h',
};

static const struct option get_opts[] = {
    { .name = "platform",       .has_arg = required_argument,     .val = OPT_PLATFORM },
    { .name = "help",           .has_arg = no_argument,           .val = OPT_HELP },
};

#ifdef _WIN32
static DWORD __stdcall
worker_thread(LPVOID lpParam)
{
    void (__stdcall *testsuite)(void) = lpParam;
    void (__stdcall *testsuites[])(void) = {testsuite, 0};
    int argc = 0;
    DWORD num_failures = wt_main(&argc, NULL, testsuites);

    return num_failures;
}

/// Run the testsuite in a separate thread. If the testsuite fails, then exit
/// immediately.
static void
run_testsuite(void (*testsuite)(void))
{
    HANDLE hThread;
    DWORD dwThreadId;

    hThread = CreateThread(NULL, 0, worker_thread, testsuite, 0, &dwThreadId);
    if (hThread != NULL) {
        // XXX: Add a decent timeout interval
        if (WaitForSingleObject(hThread, INFINITE) == WAIT_OBJECT_0) {
            DWORD exit_status;

            if (GetExitCodeThread(hThread, &exit_status)) {
                // exit_status is number of failures.
                if (exit_status == 0) {
                    // All tests passed.
                    CloseHandle(hThread);
                    return;
                }
                else {
                    // Some tests failed. Don't run any more tests.
                    exit(exit_status);
                    // or exit process ?
                }
            }
            else {
                fprintf(stderr, "gl_basic_test: error: get thread exit status"
                                " failed (%lu)\n", GetLastError());
                abort();
            }
        }
        else {
            fprintf(stderr, "gl_basic_test: error: wait for thread failed\n");
            abort();
        }
    }
    else {
        fprintf(stderr, "gl_basic_test: error: CreateThread failed\n");
        abort();
    }
}

#else

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

#endif // _WIN32

static void NORETURN
usage_error_printf(const char *fmt, ...)
{
    fprintf(stderr, "gl_basic_test usage error: ");

    if (fmt) {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        fprintf(stderr, " ");
    }

    fprintf(stderr, "(see gl_basic_test --help)\n");
    exit(EXIT_FAILURE);
}

struct enum_map {
    int i;
    const char *s;
};

static const struct enum_map platform_map[] = {
    {WAFFLE_PLATFORM_CGL,       "cgl",          },
    {WAFFLE_PLATFORM_GLX,       "glx"           },
    {WAFFLE_PLATFORM_WAYLAND,   "wayland"       },
    {WAFFLE_PLATFORM_WGL,       "wgl"           },
    {WAFFLE_PLATFORM_X11_EGL,   "x11_egl"       },
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
parse_args(int argc, char *argv[], int *platform)
{
    bool ok;
    bool loop_get_opt = true;

#ifdef __APPLE__
    removeXcodeArgs(&argc, argv);
#endif

    // prevent getopt_long from printing an error message
    opterr = 0;

    while (loop_get_opt) {
        int opt = getopt_long(argc, argv, "hp:", get_opts, NULL);
        switch (opt) {
            case -1:
                loop_get_opt = false;
                break;
            case '?':
                goto error_unrecognized_arg;
            case OPT_PLATFORM:
                ok = enum_map_translate_str(platform_map, optarg, platform);
                if (!ok) {
                    usage_error_printf("'%s' is not a valid platform",
                                       optarg);
                }
                break;
            case OPT_HELP:
                write_usage_and_exit(stdout, EXIT_SUCCESS);
                break;
            default:
                loop_get_opt = false;
                break;
        }
    }

    if (optind < argc) {
        goto error_unrecognized_arg;
    }

    if (!*platform) {
        usage_error_printf("--platform is required");
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

int
main(int argc, char *argv[])
{
    int platform;
    bool ok;

    ok = parse_args(argc, argv, &platform);
    if (!ok)
        exit(EXIT_FAILURE);

    switch (platform) {
#ifdef WAFFLE_HAS_CGL
    case WAFFLE_PLATFORM_CGL:
        run_testsuite(testsuite_cgl);
        break;
#endif
#ifdef WAFFLE_HAS_GLX
    case WAFFLE_PLATFORM_GLX:
        run_testsuite(testsuite_glx);
        break;
#endif
#ifdef WAFFLE_HAS_WAYLAND
    case WAFFLE_PLATFORM_WAYLAND:
        run_testsuite(testsuite_wayland);
        break;
#endif
#ifdef WAFFLE_HAS_WGL
    case WAFFLE_PLATFORM_WGL:
        run_testsuite(testsuite_wgl);
        break;
#endif
#ifdef WAFFLE_HAS_X11_EGL
    case WAFFLE_PLATFORM_X11_EGL:
        run_testsuite(testsuite_x11_egl);
        break;
#endif
    default:
        abort();
        break;
    }

    return 0;
}
