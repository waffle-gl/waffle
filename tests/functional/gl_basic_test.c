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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(_WIN32)
#include <unistd.h>
#else
#include <windows.h>
#endif

#include <sys/types.h>
#if !defined(_WIN32)
#include <sys/wait.h>
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

    ASSERT_TRUE(window = waffle_window_create(config,
                                              WINDOW_WIDTH, WINDOW_HEIGHT));
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

    ASSERT_TRUE(waffle_make_current(dpy, window, ctx));

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

    // Draw.
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

//
// List of tests common to all platforms.
//

TEST(gl_basic, all_gl_rgb)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL);
}

TEST(gl_basic, all_gl_rgba)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .alpha=true);
}

TEST(gl_basic, all_gl10)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=10);
}

TEST(gl_basic, all_gl11)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=11);
}

TEST(gl_basic, all_gl12)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=12);
}

TEST(gl_basic, all_gl13)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=13);
}

TEST(gl_basic, all_gl14)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=14);
}

TEST(gl_basic, all_gl15)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=15);
}

TEST(gl_basic, all_gl20)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=20);
}

TEST(gl_basic, all_gl21)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=21);
}

TEST(gl_basic, all_gl21_fwdcompat_bad_attribute)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=21,
                  .forward_compatible=true,
                  .expect_error=WAFFLE_ERROR_BAD_ATTRIBUTE);
}


//
// List of linux (glx, wayland and x11_egl) and windows (wgl) specific tests.
//
#if defined(WAFFLE_HAS_GLX) || defined(WAFFLE_HAS_WAYLAND) || defined(WAFFLE_HAS_X11_EGL) || defined(WAFFLE_HAS_WGL)
TEST(gl_basic, all_but_cgl_gl_debug)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .debug=true);
}

TEST(gl_basic, all_but_cgl_gl_fwdcompat_bad_attribute)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .forward_compatible=true,
                  .expect_error=WAFFLE_ERROR_BAD_ATTRIBUTE);
}

TEST(gl_basic, all_but_cgl_gl30)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=30);
}

TEST(gl_basic, all_but_cgl_gl30_fwdcompat)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=30,
                  .forward_compatible=true);
}

TEST(gl_basic, all_but_cgl_gl31)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=31);
}

TEST(gl_basic, all_but_cgl_gl31_fwdcompat)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=31,
                  .forward_compatible=true);
}

TEST(gl_basic, all_but_cgl_gl32_core)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=32,
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE);
}

TEST(gl_basic, all_but_cgl_gl32_core_fwdcompat)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=32,
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE,
                  .forward_compatible=true);
}

TEST(gl_basic, all_but_cgl_gl33_core)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=33,
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE);
}

TEST(gl_basic, all_but_cgl_gl40_core)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=40,
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE);
}

TEST(gl_basic, all_but_cgl_gl41_core)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=41,
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE);
}

TEST(gl_basic, all_but_cgl_gl42_core)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=42,
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE);
}

TEST(gl_basic, all_but_cgl_gl43_core)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=43,
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE);
}

TEST(gl_basic, all_but_cgl_gl32_compat)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=32,
                  .profile=WAFFLE_CONTEXT_COMPATIBILITY_PROFILE);
}

TEST(gl_basic, all_but_cgl_gl33_compat)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=33,
                  .profile=WAFFLE_CONTEXT_COMPATIBILITY_PROFILE);
}

TEST(gl_basic, all_but_cgl_gl40_compat)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=40,
                  .profile=WAFFLE_CONTEXT_COMPATIBILITY_PROFILE);
}

TEST(gl_basic, all_but_cgl_gl41_compat)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=41,
                  .profile=WAFFLE_CONTEXT_COMPATIBILITY_PROFILE);
}

TEST(gl_basic, all_but_cgl_gl42_compat)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=42,
                  .profile=WAFFLE_CONTEXT_COMPATIBILITY_PROFILE);
}

TEST(gl_basic, all_but_cgl_gl43_compat)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=43,
                  .profile=WAFFLE_CONTEXT_COMPATIBILITY_PROFILE);
}

TEST(gl_basic, all_but_cgl_gles1_rgb)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL_ES1);
}

TEST(gl_basic, all_but_cgl_gles1_rgba)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL_ES1,
                  .alpha=true);
}

TEST(gl_basic, all_but_cgl_gles10)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL_ES1,
                  .version=10,
                  .alpha=true);
}

TEST(gl_basic, all_but_cgl_gles11)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL_ES1,
                  .version=11,
                  .alpha=true);
}

TEST(gl_basic, all_but_cgl_gles1_fwdcompat_bad_attribute)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL_ES1,
                  .forward_compatible=true,
                  .expect_error=WAFFLE_ERROR_BAD_ATTRIBUTE);
}

TEST(gl_basic, all_but_cgl_gles2_rgb)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL_ES2);
}

TEST(gl_basic, all_but_cgl_gles2_rgba)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL_ES2,
                  .alpha=true);
}

TEST(gl_basic, all_but_cgl_gles20)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL_ES2,
                  .version=20);
}

TEST(gl_basic, all_but_cgl_gles2_fwdcompat_bad_attribute)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL_ES2,
                  .forward_compatible=true,
                  .expect_error=WAFFLE_ERROR_BAD_ATTRIBUTE);
}

TEST(gl_basic, all_but_cgl_gles3_rgb)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL_ES3);
}

TEST(gl_basic, all_but_cgl_gles3_rgba)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL_ES3,
                  .alpha=true);
}

TEST(gl_basic, all_but_cgl_gles30)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL_ES3,
                  .version=30);
}

TEST(gl_basic, all_but_cgl_gles3_fwdcompat_bad_attribute)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL_ES3,
                  .forward_compatible=true,
                  .expect_error=WAFFLE_ERROR_BAD_ATTRIBUTE);
}
#endif

#ifdef WAFFLE_HAS_CGL
TEST(gl_basic, cgl_init)
{
    gl_basic_init(WAFFLE_PLATFORM_CGL);
}

TEST(gl_basic, cgl_gles1_unsupported)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL_ES1,
                  .expect_error=WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
}

TEST(gl_basic, cgl_gles2_unsupported)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL_ES2,
                  .expect_error=WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
}

TEST(gl_basic, cgl_gl_debug_is_unsupported)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .debug=true,
                  .expect_error=WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
}

TEST(gl_basic, cgl_gl_fwdcompat_bad_attribute)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .forward_compatible=true,
                  .expect_error=WAFFLE_ERROR_BAD_ATTRIBUTE);
}

TEST(gl_basic, cgl_gl30)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=30,
                  .expect_error=WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
}

TEST(gl_basic, cgl_gl31)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=31);
}

TEST(gl_basic, cgl_gl32_core)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=32,
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE);
}

TEST(gl_basic, cgl_gl33_core)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=33,
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE,
                  .expect_error=WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
}

TEST(gl_basic, cgl_gl40_core)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=40,
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE,
                  .expect_error=WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
}

TEST(gl_basic, cgl_gl41_core)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=41,
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE,
                  .expect_error=WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
}

TEST(gl_basic, cgl_gl42_core)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=42,
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE,
                  .expect_error=WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
}

TEST(gl_basic, cgl_gl43_core)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=43,
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE,
                  .expect_error=WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
}

TEST(gl_basic, cgl_gl32_compat)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=32,
                  .profile=WAFFLE_CONTEXT_COMPATIBILITY_PROFILE,
                  .expect_error=WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
}

TEST(gl_basic, cgl_gl33_compat)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=33,
                  .profile=WAFFLE_CONTEXT_COMPATIBILITY_PROFILE,
                  .expect_error=WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
}

TEST(gl_basic, cgl_gl40_compat)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=40,
                  .profile=WAFFLE_CONTEXT_COMPATIBILITY_PROFILE,
                  .expect_error=WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
}

TEST(gl_basic, cgl_gl41_compat)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=41,
                  .profile=WAFFLE_CONTEXT_COMPATIBILITY_PROFILE,
                  .expect_error=WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
}

TEST(gl_basic, cgl_gl42_compat)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=42,
                  .profile=WAFFLE_CONTEXT_COMPATIBILITY_PROFILE,
                  .expect_error=WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
}

TEST(gl_basic, cgl_gl43_compat)
{
    gl_basic_draw(.api=WAFFLE_CONTEXT_OPENGL,
                  .version=43,
                  .profile=WAFFLE_CONTEXT_COMPATIBILITY_PROFILE,
                  .expect_error=WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
}

static void
testsuite_cgl(void)
{
    TEST_RUN(gl_basic, cgl_init);

    TEST_RUN(gl_basic, cgl_gles1_unsupported);
    TEST_RUN(gl_basic, cgl_gles2_unsupported);

    TEST_RUN2(gl_basic, cgl_gl_rgb, all_gl_rgb);
    TEST_RUN2(gl_basic, cgl_gl_rgba, all_gl_rgba);

    TEST_RUN(gl_basic, cgl_gl_debug_is_unsupported);
    TEST_RUN(gl_basic, cgl_gl_fwdcompat_bad_attribute);

    TEST_RUN2(gl_basic, cgl_gl10, all_gl10);
    TEST_RUN2(gl_basic, cgl_gl11, all_gl11);
    TEST_RUN2(gl_basic, cgl_gl12, all_gl12);
    TEST_RUN2(gl_basic, cgl_gl13, all_gl13);
    TEST_RUN2(gl_basic, cgl_gl14, all_gl14);
    TEST_RUN2(gl_basic, cgl_gl15, all_gl15);
    TEST_RUN2(gl_basic, cgl_gl20, all_gl20);
    TEST_RUN2(gl_basic, cgl_gl21, all_gl21);
    TEST_RUN2(gl_basic, cgl_gl21_fwdcompat_bad_attribute, all_gl21_fwdcompat_bad_attribute);
    TEST_RUN(gl_basic, cgl_gl30);
    TEST_RUN(gl_basic, cgl_gl31);

    TEST_RUN(gl_basic, cgl_gl32_core);
    TEST_RUN(gl_basic, cgl_gl33_core);
    TEST_RUN(gl_basic, cgl_gl40_core);
    TEST_RUN(gl_basic, cgl_gl41_core);
    TEST_RUN(gl_basic, cgl_gl42_core);
    TEST_RUN(gl_basic, cgl_gl43_core);

    TEST_RUN(gl_basic, cgl_gl32_compat);
    TEST_RUN(gl_basic, cgl_gl33_compat);
    TEST_RUN(gl_basic, cgl_gl40_compat);
    TEST_RUN(gl_basic, cgl_gl41_compat);
    TEST_RUN(gl_basic, cgl_gl42_compat);
    TEST_RUN(gl_basic, cgl_gl43_compat);
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

    TEST_RUN2(gl_basic, glx_gl_rgb, all_gl_rgb);
    TEST_RUN2(gl_basic, glx_gl_rgba, all_gl_rgb);
    TEST_RUN2(gl_basic, glx_gl_debug, all_but_cgl_gl_debug);
    TEST_RUN2(gl_basic, glx_gl_fwdcompat_bad_attribute, all_but_cgl_gl_fwdcompat_bad_attribute);

    TEST_RUN2(gl_basic, glx_gl10, all_gl10);
    TEST_RUN2(gl_basic, glx_gl11, all_gl11);
    TEST_RUN2(gl_basic, glx_gl12, all_gl12);
    TEST_RUN2(gl_basic, glx_gl13, all_gl13);
    TEST_RUN2(gl_basic, glx_gl14, all_gl14);
    TEST_RUN2(gl_basic, glx_gl15, all_gl15);
    TEST_RUN2(gl_basic, glx_gl20, all_gl20);
    TEST_RUN2(gl_basic, glx_gl21, all_gl21);
    TEST_RUN2(gl_basic, glx_gl21_fwdcompat_bad_attribute, all_gl21_fwdcompat_bad_attribute);

    TEST_RUN2(gl_basic, glx_gl30, all_but_cgl_gl30);
    TEST_RUN2(gl_basic, glx_gl30_fwdcompat, all_but_cgl_gl30_fwdcompat);
    TEST_RUN2(gl_basic, glx_gl31, all_but_cgl_gl31);
    TEST_RUN2(gl_basic, glx_gl31_fwdcompat, all_but_cgl_gl31_fwdcompat);

    TEST_RUN2(gl_basic, glx_gl32_core, all_but_cgl_gl32_core);
    TEST_RUN2(gl_basic, glx_gl32_core_fwdcompat, all_but_cgl_gl32_core_fwdcompat);
    TEST_RUN2(gl_basic, glx_gl33_core, all_but_cgl_gl33_core);
    TEST_RUN2(gl_basic, glx_gl40_core, all_but_cgl_gl40_core);
    TEST_RUN2(gl_basic, glx_gl41_core, all_but_cgl_gl41_core);
    TEST_RUN2(gl_basic, glx_gl42_core, all_but_cgl_gl42_core);
    TEST_RUN2(gl_basic, glx_gl43_core, all_but_cgl_gl43_core);

    TEST_RUN2(gl_basic, glx_gl32_compat, all_but_cgl_gl32_compat);
    TEST_RUN2(gl_basic, glx_gl33_compat, all_but_cgl_gl33_compat);
    TEST_RUN2(gl_basic, glx_gl40_compat, all_but_cgl_gl40_compat);
    TEST_RUN2(gl_basic, glx_gl41_compat, all_but_cgl_gl41_compat);
    TEST_RUN2(gl_basic, glx_gl42_compat, all_but_cgl_gl42_compat);
    TEST_RUN2(gl_basic, glx_gl43_compat, all_but_cgl_gl43_compat);

    TEST_RUN2(gl_basic, glx_gles1_rgb, all_but_cgl_gles1_rgb);
    TEST_RUN2(gl_basic, glx_gles1_rgba, all_but_cgl_gles1_rgba);
    TEST_RUN2(gl_basic, glx_gles1_fwdcompat_bad_attribute, all_but_cgl_gles1_fwdcompat_bad_attribute);

    TEST_RUN2(gl_basic, glx_gles10, all_but_cgl_gles10);
    TEST_RUN2(gl_basic, glx_gles11, all_but_cgl_gles10);

    TEST_RUN2(gl_basic, glx_gles2_rgb, all_but_cgl_gles2_rgb);
    TEST_RUN2(gl_basic, glx_gles2_rgba, all_but_cgl_gles2_rgba);
    TEST_RUN2(gl_basic, glx_gles2_fwdcompat_bad_attribute, all_but_cgl_gles2_fwdcompat_bad_attribute);

    TEST_RUN2(gl_basic, glx_gles20, all_but_cgl_gles20);

    TEST_RUN2(gl_basic, glx_gles3_rgb, all_but_cgl_gles3_rgb);
    TEST_RUN2(gl_basic, glx_gles3_rgba, all_but_cgl_gles3_rgba);
    TEST_RUN2(gl_basic, glx_gles3_fwdcompat_bad_attribute, all_but_cgl_gles3_fwdcompat_bad_attribute);

    TEST_RUN2(gl_basic, glx_gles30, all_but_cgl_gles30);
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

    TEST_RUN2(gl_basic, wayland_gl_rgb, all_gl_rgb);
    TEST_RUN2(gl_basic, wayland_gl_rgba, all_gl_rgba);

    TEST_RUN2(gl_basic, wayland_gl_debug, all_but_cgl_gl_debug);
    TEST_RUN2(gl_basic, wayland_gl_fwdcompat_bad_attribute, all_but_cgl_gl_fwdcompat_bad_attribute);

    TEST_RUN2(gl_basic, wayland_gl10, all_gl10);
    TEST_RUN2(gl_basic, wayland_gl11, all_gl11);
    TEST_RUN2(gl_basic, wayland_gl12, all_gl12);
    TEST_RUN2(gl_basic, wayland_gl13, all_gl13);
    TEST_RUN2(gl_basic, wayland_gl14, all_gl14);
    TEST_RUN2(gl_basic, wayland_gl15, all_gl15);
    TEST_RUN2(gl_basic, wayland_gl20, all_gl20);
    TEST_RUN2(gl_basic, wayland_gl21, all_gl21);
    TEST_RUN2(gl_basic, wayland_gl21_fwdcompat_bad_attribute, all_gl21_fwdcompat_bad_attribute);

    TEST_RUN2(gl_basic, wayland_gl30, all_but_cgl_gl30);
    TEST_RUN2(gl_basic, wayland_gl30_fwdcompat, all_but_cgl_gl30_fwdcompat);
    TEST_RUN2(gl_basic, wayland_gl31, all_but_cgl_gl31);
    TEST_RUN2(gl_basic, wayland_gl31_fwdcompat, all_but_cgl_gl31_fwdcompat);

    TEST_RUN2(gl_basic, wayland_gl32_core, all_but_cgl_gl32_core);
    TEST_RUN2(gl_basic, wayland_gl32_core_fwdcompat, all_but_cgl_gl32_core_fwdcompat);
    TEST_RUN2(gl_basic, wayland_gl33_core, all_but_cgl_gl33_core);
    TEST_RUN2(gl_basic, wayland_gl40_core, all_but_cgl_gl40_core);
    TEST_RUN2(gl_basic, wayland_gl41_core, all_but_cgl_gl41_core);
    TEST_RUN2(gl_basic, wayland_gl42_core, all_but_cgl_gl42_core);
    TEST_RUN2(gl_basic, wayland_gl43_core, all_but_cgl_gl43_core);

    TEST_RUN2(gl_basic, wayland_gl32_compat, all_but_cgl_gl32_compat);
    TEST_RUN2(gl_basic, wayland_gl33_compat, all_but_cgl_gl33_compat);
    TEST_RUN2(gl_basic, wayland_gl40_compat, all_but_cgl_gl40_compat);
    TEST_RUN2(gl_basic, wayland_gl41_compat, all_but_cgl_gl41_compat);
    TEST_RUN2(gl_basic, wayland_gl42_compat, all_but_cgl_gl42_compat);
    TEST_RUN2(gl_basic, wayland_gl43_compat, all_but_cgl_gl43_compat);

    TEST_RUN2(gl_basic, wayland_gles1_rgb, all_but_cgl_gles1_rgb);
    TEST_RUN2(gl_basic, wayland_gles1_rgba, all_but_cgl_gles1_rgba);
    TEST_RUN2(gl_basic, wayland_gles1_fwdcompat_bad_attribute, all_but_cgl_gles1_fwdcompat_bad_attribute);

    TEST_RUN2(gl_basic, wayland_gles10, all_but_cgl_gles10);
    TEST_RUN2(gl_basic, wayland_gles11, all_but_cgl_gles11);

    TEST_RUN2(gl_basic, wayland_gles1_rgb, all_but_cgl_gles1_rgb);
    TEST_RUN2(gl_basic, wayland_gles1_rgba, all_but_cgl_gles1_rgba);
    TEST_RUN2(gl_basic, wayland_gles1_fwdcompat_bad_attribute, all_but_cgl_gles1_fwdcompat_bad_attribute);

    TEST_RUN2(gl_basic, wayland_gles20, all_but_cgl_gles20);

    TEST_RUN2(gl_basic, wayland_gles3_rgb, all_but_cgl_gles3_rgb);
    TEST_RUN2(gl_basic, wayland_gles3_rgba, all_but_cgl_gles3_rgba);
    TEST_RUN2(gl_basic, wayland_gles3_fwdcompat_bad_attribute, all_but_cgl_gles3_fwdcompat_bad_attribute);

    TEST_RUN2(gl_basic, wayland_gles30, all_but_cgl_gles30);
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

    TEST_RUN2(gl_basic, x11_egl_gl_rgb, all_gl_rgb);
    TEST_RUN2(gl_basic, x11_egl_gl_rgba, all_gl_rgba);
    TEST_RUN2(gl_basic, x11_egl_gl_debug, all_but_cgl_gl_debug);
    TEST_RUN2(gl_basic, x11_egl_gl_fwdcompat_bad_attribute, all_but_cgl_gl_fwdcompat_bad_attribute);

    TEST_RUN2(gl_basic, x11_egl_gl10, all_gl10);
    TEST_RUN2(gl_basic, x11_egl_gl11, all_gl11);
    TEST_RUN2(gl_basic, x11_egl_gl12, all_gl12);
    TEST_RUN2(gl_basic, x11_egl_gl13, all_gl13);
    TEST_RUN2(gl_basic, x11_egl_gl14, all_gl14);
    TEST_RUN2(gl_basic, x11_egl_gl15, all_gl15);
    TEST_RUN2(gl_basic, x11_egl_gl20, all_gl20);
    TEST_RUN2(gl_basic, x11_egl_gl21, all_gl21);
    TEST_RUN2(gl_basic, x11_egl_gl21_fwdcompat_bad_attribute, all_gl21_fwdcompat_bad_attribute);

    TEST_RUN2(gl_basic, x11_egl_gl30, all_but_cgl_gl30);
    TEST_RUN2(gl_basic, x11_egl_gl30_fwdcompat, all_but_cgl_gl30_fwdcompat);
    TEST_RUN2(gl_basic, x11_egl_gl31, all_but_cgl_gl31);
    TEST_RUN2(gl_basic, x11_egl_gl31_fwdcompat, all_but_cgl_gl31_fwdcompat);

    TEST_RUN2(gl_basic, x11_egl_gl32_core, all_but_cgl_gl32_core);
    TEST_RUN2(gl_basic, x11_egl_gl32_core_fwdcompat, all_but_cgl_gl32_core_fwdcompat);
    TEST_RUN2(gl_basic, x11_egl_gl33_core, all_but_cgl_gl33_core);
    TEST_RUN2(gl_basic, x11_egl_gl40_core, all_but_cgl_gl40_core);
    TEST_RUN2(gl_basic, x11_egl_gl41_core, all_but_cgl_gl41_core);
    TEST_RUN2(gl_basic, x11_egl_gl42_core, all_but_cgl_gl42_core);
    TEST_RUN2(gl_basic, x11_egl_gl43_core, all_but_cgl_gl43_core);

    TEST_RUN2(gl_basic, x11_egl_gl32_compat, all_but_cgl_gl32_compat);
    TEST_RUN2(gl_basic, x11_egl_gl33_compat, all_but_cgl_gl33_compat);
    TEST_RUN2(gl_basic, x11_egl_gl40_compat, all_but_cgl_gl40_compat);
    TEST_RUN2(gl_basic, x11_egl_gl41_compat, all_but_cgl_gl41_compat);
    TEST_RUN2(gl_basic, x11_egl_gl42_compat, all_but_cgl_gl42_compat);
    TEST_RUN2(gl_basic, x11_egl_gl43_compat, all_but_cgl_gl43_compat);

    TEST_RUN2(gl_basic, x11_egl_gles1_rgb, all_but_cgl_gles1_rgb);
    TEST_RUN2(gl_basic, x11_egl_gles1_rgba, all_but_cgl_gles1_rgba);
    TEST_RUN2(gl_basic, x11_egl_gles1_fwdcompat_bad_attribute, all_but_cgl_gles1_fwdcompat_bad_attribute);

    TEST_RUN2(gl_basic, x11_egl_gles10, all_but_cgl_gles10);
    TEST_RUN2(gl_basic, x11_egl_gles11, all_but_cgl_gles11);

    TEST_RUN2(gl_basic, x11_egl_gles2_rgb, all_but_cgl_gles2_rgb);
    TEST_RUN2(gl_basic, x11_egl_gles2_rgba, all_but_cgl_gles2_rgba);
    TEST_RUN2(gl_basic, x11_egl_gles2_fwdcompat_bad_attribute, all_but_cgl_gles2_fwdcompat_bad_attribute);

    TEST_RUN2(gl_basic, x11_egl_gles20, all_but_cgl_gles20);

    TEST_RUN2(gl_basic, x11_egl_gles3_rgb, all_but_cgl_gles3_rgb);
    TEST_RUN2(gl_basic, x11_egl_gles3_rgba, all_but_cgl_gles3_rgba);
    TEST_RUN2(gl_basic, x11_egl_gles3_fwdcompat_bad_attribute, all_but_cgl_gles3_fwdcompat_bad_attribute);

    TEST_RUN2(gl_basic, x11_egl_gles30, all_but_cgl_gles30);
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

    TEST_RUN(gl_basic, all_gl_rgb);
    TEST_RUN(gl_basic, all_gl_rgba);
    TEST_RUN(gl_basic, all_but_cgl_gl_debug);
    TEST_RUN(gl_basic, all_but_cgl_gl_fwdcompat_bad_attribute);

    TEST_RUN(gl_basic, all_gl10);
    TEST_RUN(gl_basic, all_gl11);
    TEST_RUN(gl_basic, all_gl12);
    TEST_RUN(gl_basic, all_gl13);
    TEST_RUN(gl_basic, all_gl14);
    TEST_RUN(gl_basic, all_gl15);
    TEST_RUN(gl_basic, all_gl20);
    TEST_RUN(gl_basic, all_gl21);
    TEST_RUN(gl_basic, all_gl21_fwdcompat_bad_attribute);

    TEST_RUN(gl_basic, all_but_cgl_gl30);
    TEST_RUN(gl_basic, all_but_cgl_gl30_fwdcompat);
    TEST_RUN(gl_basic, all_but_cgl_gl31);
    TEST_RUN(gl_basic, all_but_cgl_gl31_fwdcompat);

    TEST_RUN(gl_basic, all_but_cgl_gl32_core);
    TEST_RUN(gl_basic, all_but_cgl_gl32_core_fwdcompat);
    TEST_RUN(gl_basic, all_but_cgl_gl33_core);
    TEST_RUN(gl_basic, all_but_cgl_gl40_core);
    TEST_RUN(gl_basic, all_but_cgl_gl41_core);
    TEST_RUN(gl_basic, all_but_cgl_gl42_core);
    TEST_RUN(gl_basic, all_but_cgl_gl43_core);

    TEST_RUN(gl_basic, all_but_cgl_gl32_compat);
    TEST_RUN(gl_basic, all_but_cgl_gl33_compat);
    TEST_RUN(gl_basic, all_but_cgl_gl40_compat);
    TEST_RUN(gl_basic, all_but_cgl_gl41_compat);
    TEST_RUN(gl_basic, all_but_cgl_gl42_compat);
    TEST_RUN(gl_basic, all_but_cgl_gl43_compat);

    TEST_RUN(gl_basic, all_but_cgl_gles1_rgb);
    TEST_RUN(gl_basic, all_but_cgl_gles1_rgba);
    TEST_RUN(gl_basic, all_but_cgl_gles1_fwdcompat_bad_attribute);

    TEST_RUN(gl_basic, all_but_cgl_gles10);
    TEST_RUN(gl_basic, all_but_cgl_gles11);

    TEST_RUN(gl_basic, all_but_cgl_gles2_rgb);
    TEST_RUN(gl_basic, all_but_cgl_gles2_rgba);
    TEST_RUN(gl_basic, all_but_cgl_gles2_fwdcompat_bad_attribute);

    TEST_RUN(gl_basic, all_but_cgl_gles20);

    TEST_RUN(gl_basic, all_but_cgl_gles3_rgb);
    TEST_RUN(gl_basic, all_but_cgl_gles3_rgba);
    TEST_RUN(gl_basic, all_but_cgl_gles3_fwdcompat_bad_attribute);

    TEST_RUN(gl_basic, all_but_cgl_gles30);
}
#endif // WAFFLE_HAS_WGL

static void
usage_error(void)
{
    fprintf(stderr, "gl_basic_test: usage error\n");
    exit(1);
}

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
                                " failed (%d)\n", GetLastError());
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
#ifdef WAFFLE_HAS_WGL
    run_testsuite(testsuite_wgl);
#endif

   return 0;
}
