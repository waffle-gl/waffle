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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <waffle/waffle.h>
#include <waffle_test/waffle_test.h>

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

static const int32_t config_attrib_list[] = {
    WAFFLE_RED_SIZE,        8,
    WAFFLE_GREEN_SIZE,      8,
    WAFFLE_BLUE_SIZE,       8,
    WAFFLE_ALPHA_SIZE,      0,
    0,
};

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

    // If an earlier test failed, then we need to clean up after it here.
    ABORT_IF(!waffle_finish());
}

static void
testgroup_gl_basic_teardown(void)
{
    // empty
}

static void
gl_basic(int32_t platform, int32_t gl_api)
{
    const int32_t init_attrib_list[] = {
        WAFFLE_PLATFORM,        platform,
        WAFFLE_GL_API,          gl_api,
        0,
    };

    struct waffle_display *dpy = NULL;
    struct waffle_config *config = NULL;
    struct waffle_window *window = NULL;
    struct waffle_context *ctx = NULL;

    ASSERT_TRUE(waffle_init(init_attrib_list));

    // Create objects.
    ASSERT_TRUE(dpy = waffle_display_connect(NULL));
    ASSERT_TRUE(config = waffle_config_choose(dpy, config_attrib_list));
    ASSERT_TRUE(window = waffle_window_create(config,
                                              WINDOW_WIDTH, WINDOW_HEIGHT));
    ASSERT_TRUE(ctx = waffle_context_create(config, NULL));

    // Get GL functions.
    ASSERT_TRUE(glClear = waffle_dlsym_gl("glClear"));
    ASSERT_TRUE(glClearColor = waffle_dlsym_gl("glClearColor"));
    ASSERT_TRUE(glGetError = waffle_dlsym_gl("glGetError"));
    ASSERT_TRUE(glReadPixels = waffle_dlsym_gl("glReadPixels"));

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
    ABORT_IF(!waffle_finish());
}

#ifdef WAFFLE_HAS_X11_EGL
TEST(gl_basic, x11_egl_gl)
{
    gl_basic(WAFFLE_PLATFORM_X11_EGL, WAFFLE_GL);
}

TEST(gl_basic, x11_egl_gles1)
{
    gl_basic(WAFFLE_PLATFORM_X11_EGL, WAFFLE_GLES1);
}

TEST(gl_basic, x11_egl_gles2)
{
    gl_basic(WAFFLE_PLATFORM_X11_EGL, WAFFLE_GLES1);
}
#endif

static void
testsuite_gl_basic(void)
{
#ifdef WAFFLE_HAS_X11_EGL
    TEST_RUN(gl_basic, x11_egl_gl);
    TEST_RUN(gl_basic, x11_egl_gles1);
    TEST_RUN(gl_basic, x11_egl_gles2);
#endif
}

static void (*test_suites[])(void) = {
    testsuite_gl_basic,
    0,
};

int
main(int argc, char *argv[])
{
    return wt_main(&argc, argv, test_suites);
}