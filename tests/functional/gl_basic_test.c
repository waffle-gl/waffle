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
#include <setjmp.h> // for cmocka.h
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

#include <cmocka.h>
#include "waffle.h"

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

struct test_state_gl_basic {
    bool initialized;
    struct waffle_display *dpy;
    struct waffle_config *config;
    struct waffle_window *window;
    struct waffle_context *ctx;

    uint8_t actual_pixels[4 * WINDOW_WIDTH * WINDOW_HEIGHT];
    uint8_t expect_pixels[4 * WINDOW_WIDTH * WINDOW_HEIGHT];
};

#define ASSERT_GL(statement) \
    do { \
        statement; \
        assert_false(glGetError()); \
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

#define GL_NO_ERROR                 0x0000
#define GL_VERSION                  0x1F02
#define GL_UNSIGNED_BYTE            0x1401
#define GL_UNSIGNED_INT             0x1405
#define GL_FLOAT                    0x1406
#define GL_RGB                      0x1907
#define GL_RGBA                     0x1908
#define GL_COLOR_BUFFER_BIT         0x00004000
#define GL_CONTEXT_FLAGS            0x821e
#define GL_CONTEXT_ROBUST_ACCESS    0x90F3

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

static int
setup(void **state)
{
    struct test_state_gl_basic *ts;

    ts = calloc(1, sizeof(*ts));
    if (!ts)
        return -1;

    for (int y = 0 ; y < WINDOW_HEIGHT; ++y) {
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            uint8_t *p = &ts->expect_pixels[4 * (y * WINDOW_WIDTH + x)];
            p[0] = RED_UB;
            p[1] = GREEN_UB;
            p[2] = BLUE_UB;
            p[3] = ALPHA_UB;
        }
    }
    // Fill actual_pixels with canaries.
    memset(&ts->actual_pixels, 0x99, sizeof(ts->actual_pixels));

    *state = ts;
    return 0;
}

static int
teardown(void **state)
{
    free(*state);
    return 0;
}

// The rules that dictate how to properly query a GL symbol are complex. The
// rules depend on the OS, on the winsys API, and even on the particular driver
// being used. The rules differ between EGL 1.4 and EGL 1.5; differ between
// Linux, Windows, and Mac; and differ between Mesa and Mali.
//
// This function hides that complexity with a naive heuristic: try, then try
// again.
static void *
get_gl_symbol(enum waffle_enum context_api, const char *name)
{
    void *sym = NULL;
    enum waffle_enum dl = 0;

    switch (context_api) {
        case WAFFLE_CONTEXT_OPENGL: dl = WAFFLE_DL_OPENGL; break;
        case WAFFLE_CONTEXT_OPENGL_ES1: dl = WAFFLE_DL_OPENGL_ES1; break;
        case WAFFLE_CONTEXT_OPENGL_ES2: dl = WAFFLE_DL_OPENGL_ES2; break;
        case WAFFLE_CONTEXT_OPENGL_ES3: dl = WAFFLE_DL_OPENGL_ES3; break;
        default: assert_true(0); break;
    }

    if (waffle_dl_can_open(dl)) {
        sym = waffle_dl_sym(dl, name);
    }

    if (!sym) {
        sym = waffle_get_proc_address(name);
    }

    return sym;
}

static int
gl_basic_init(void **state, int32_t waffle_platform)
{
    struct test_state_gl_basic *ts;
    int ret;

    ret = setup((void **)&ts);
    if (ret)
        return -1;

    const int32_t init_attrib_list[] = {
        WAFFLE_PLATFORM, waffle_platform,
        0,
    };

    ts->initialized = waffle_init(init_attrib_list);
    if (!ts->initialized) {
        // XXX: does cmocka call teardown if setup fails ?
        teardown(state);
        return -1;
    }

    *state = ts;
    return 0;
}

static int
gl_basic_fini(void **state)
{
    struct test_state_gl_basic *ts = *state;
    bool ret = false;

    // XXX: return immediately on error or attempt to finish the teardown ?
    if (ts->dpy) // XXX: keep track if we've had current ctx ?
        ret = waffle_make_current(ts->dpy, NULL, NULL);
    if (ts->window)
        ret = waffle_window_destroy(ts->window);
    if (ts->ctx)
        ret = waffle_context_destroy(ts->ctx);
    if (ts->config)
        ret = waffle_config_destroy(ts->config);
    if (ts->dpy)
        ret = waffle_display_disconnect(ts->dpy);

    if (ts->initialized)
        ret = waffle_teardown();

    teardown(state);
    return ret ? 0 : -1;
}

#define gl_basic_draw(state, ...) \
    \
    gl_basic_draw__(state, (struct gl_basic_draw_args__) { \
        .api = 0, \
        .version = WAFFLE_DONT_CARE, \
        .profile = WAFFLE_DONT_CARE, \
        .forward_compatible = false, \
        .debug = false, \
        .robust = false, \
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
    bool robust;
    bool alpha;
};

static void
error_waffle(void)
{
    const struct waffle_error_info *info = waffle_error_get_info();
    const char *code = waffle_error_to_string(info->code);

    if (info->message_length > 0)
        fprintf(stderr, "Waffle error: 0x%x %s: %s\n", info->code, code, info->message);
    else
        fprintf(stderr, "Waffle error: 0x%x %s\n", info->code, code);
}

#define assert_true_with_wfl_error(_arg) \
    if (!(_arg)) { \
        error_waffle(); \
        assert_true(0); \
    }

static void
gl_basic_draw__(void **state, struct gl_basic_draw_args__ args)
{
    struct test_state_gl_basic *ts = *state;
    int32_t waffle_context_api = args.api;
    int32_t context_version = args.version;
    int32_t context_profile = args.profile;
    int32_t expect_error = args.expect_error;
    bool context_forward_compatible = args.forward_compatible;
    bool context_debug = args.debug;
    bool context_robust = args.robust;
    bool alpha = args.alpha;
    bool ret;

    int32_t config_attrib_list[64];
    int i;

    const intptr_t window_attrib_list[] = {
        WAFFLE_WINDOW_WIDTH,    WINDOW_WIDTH,
        WAFFLE_WINDOW_HEIGHT,   WINDOW_HEIGHT,
        0,
    };

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
    if (context_robust) {
        config_attrib_list[i++] = WAFFLE_CONTEXT_ROBUST_ACCESS;
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
    ts->dpy = waffle_display_connect(NULL);
    assert_true_with_wfl_error(ts->dpy);

    ts->config = waffle_config_choose(ts->dpy, config_attrib_list);
    if (expect_error) {
        assert_true(ts->config == NULL);
        assert_true(waffle_error_get_code() == expect_error);
        return;
    } else if (ts->config == NULL) {
        switch (waffle_error_get_code()) {
        case WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM:
            // fall-through
        case WAFFLE_ERROR_UNKNOWN:
            // Assume that the native platform rejected the requested
            // config flavor.
            skip();
        default:
            assert_true_with_wfl_error(ts->config);
        }
    }

    ts->window = waffle_window_create2(ts->config, window_attrib_list);
    assert_true_with_wfl_error(ts->window);

    ret = waffle_window_show(ts->window);
    assert_true_with_wfl_error(ret);

    ts->ctx = waffle_context_create(ts->config, NULL);
    if (ts->ctx == NULL) {
        switch (waffle_error_get_code()) {
        case WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM:
            // fall-through
        case WAFFLE_ERROR_UNKNOWN:
            // Assume that the native platform rejected the requested
            // context flavor.
            skip();
        default:
            assert_true_with_wfl_error(ts->ctx);
        }
    }

    // Get OpenGL functions.
    assert_true(glClear         = get_gl_symbol(waffle_context_api, "glClear"));
    assert_true(glClearColor    = get_gl_symbol(waffle_context_api, "glClearColor"));
    assert_true(glGetError      = get_gl_symbol(waffle_context_api, "glGetError"));
    assert_true(glGetIntegerv   = get_gl_symbol(waffle_context_api, "glGetIntegerv"));
    assert_true(glReadPixels    = get_gl_symbol(waffle_context_api, "glReadPixels"));
    assert_true(glGetString     = get_gl_symbol(waffle_context_api, "glGetString"));

    ret = waffle_make_current(ts->dpy, ts->window, ts->ctx);
    assert_true_with_wfl_error(ret);

    assert_true(waffle_get_current_display() == ts->dpy);
    assert_true(waffle_get_current_window() == ts->window);
    assert_true(waffle_get_current_context() == ts->ctx);

    const char *version_str, *expected_version_str;
    int major, minor, count;

    ASSERT_GL(version_str = (const char *) glGetString(GL_VERSION));
    assert_true(version_str != NULL);

    switch (waffle_context_api) {
    case WAFFLE_CONTEXT_OPENGL:
	expected_version_str = "";
        break;
    case WAFFLE_CONTEXT_OPENGL_ES1:
	expected_version_str = "OpenGL ES-CM ";
        break;
    case WAFFLE_CONTEXT_OPENGL_ES2:
    case WAFFLE_CONTEXT_OPENGL_ES3:
	expected_version_str = "OpenGL ES ";
        break;
    default:
        assert_true(0);
        break;
    }

    const size_t version_str_len = strlen(expected_version_str);
    assert_true(strncmp(version_str, expected_version_str, version_str_len) == 0);
    version_str += version_str_len;

    count = sscanf(version_str, "%d.%d", &major, &minor);
    assert_true(count == 2);
    assert_true(major >= 0);
    assert_true(minor >= 0 && minor < 10);

    if (context_version != WAFFLE_DONT_CARE) {
        int expected_major = context_version / 10;
        int expected_minor = context_version % 10;

        assert_true(major >= expected_major);
        assert_true(minor >= expected_minor || major > expected_major);
    }

    const char *profile_suffix = "";

    if (waffle_context_api == WAFFLE_CONTEXT_OPENGL) {
        switch (context_profile) {
        case WAFFLE_CONTEXT_CORE_PROFILE:
            profile_suffix = " (Core Profile)";
            break;
        case WAFFLE_CONTEXT_COMPATIBILITY_PROFILE:
            profile_suffix = " (Compatibility Profile)";
            break;
        case WAFFLE_DONT_CARE:
            break;
        default:
            assert_true(0);
            break;
        }
    }

    char profile_str[30]; // 30 should be enough ;-)
    sprintf(profile_str, "%d.%d%s", major, minor, profile_suffix);

    const size_t profile_str_len = strlen(profile_str);
    assert_true(strncmp(version_str, profile_str, profile_str_len) == 0);

    int version_10x = 10 * major + minor;

    if ((waffle_context_api == WAFFLE_CONTEXT_OPENGL && version_10x >= 30) ||
        (waffle_context_api != WAFFLE_CONTEXT_OPENGL && version_10x >= 32)) {
        GLint context_flags = 0;
        if (context_forward_compatible || context_debug) {
            glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
        }

        if (context_forward_compatible) {
            assert_true(context_flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT);
        }

        if (context_debug) {
            assert_true(context_flags & GL_CONTEXT_FLAG_DEBUG_BIT);
        }
    }

    // GL_ROBUST_ACCESS comes with the following extensions
    // GL_ARB_robustness
    // GL_EXT_robustness
    // GL_KHR_robustness
    //
    // To keep it simple, assume the correct extension is there if
    // glGetError is happy ;-)
    if (context_robust) {
        GLint robust_flag = 0;
        glGetIntegerv(GL_CONTEXT_ROBUST_ACCESS, &robust_flag);

        if (glGetError() == GL_NO_ERROR)
            assert_true(robust_flag);
    }

    // Draw.
    ASSERT_GL(glClearColor(RED_F, GREEN_F, BLUE_F, ALPHA_F));
    ASSERT_GL(glClear(GL_COLOR_BUFFER_BIT));
    ASSERT_GL(glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                           GL_RGBA, GL_UNSIGNED_BYTE,
                           ts->actual_pixels));
    ret = waffle_window_swap_buffers(ts->window);
    assert_true_with_wfl_error(ret);

    assert_memory_equal(&ts->actual_pixels, &ts->expect_pixels,
                        sizeof(ts->expect_pixels));
}

//
// List of tests common to all platforms.
//

#define test_XX_rgb(context_api, waffle_api, error)                     \
static void test_gl_basic_##context_api##_rgb(void **state)             \
{                                                                       \
    gl_basic_draw(state,                                                \
                  .api=WAFFLE_CONTEXT_##waffle_api,                     \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_XX_rgba(context_api, waffle_api, error)                    \
static void test_gl_basic_##context_api##_rgba(void **state)            \
{                                                                       \
    gl_basic_draw(state,                                                \
                  .api=WAFFLE_CONTEXT_##waffle_api,                     \
                  .alpha=true,                                          \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_XX_fwdcompat(context_api, waffle_api, error)               \
static void test_gl_basic_##context_api##_fwdcompat(void **state)       \
{                                                                       \
    gl_basic_draw(state,                                                \
                  .api=WAFFLE_CONTEXT_##waffle_api,                     \
                  .forward_compatible=true,                             \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_XX_debug(context_api, waffle_api, error)                   \
static void test_gl_basic_##context_api##_debug(void **state)           \
{                                                                       \
    gl_basic_draw(state,                                                \
                  .api=WAFFLE_CONTEXT_##waffle_api,                     \
                  .debug=true,                                          \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_XX_robust(context_api, waffle_api, error)                  \
static void test_gl_basic_##context_api##_robust(void **state)          \
{                                                                       \
    gl_basic_draw(state,                                                \
                  .api=WAFFLE_CONTEXT_##waffle_api,                     \
                  .robust=true,                                         \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_glXX(waffle_version, error)                                \
static void test_gl_basic_gl##waffle_version(void **state)              \
{                                                                       \
    gl_basic_draw(state,                                                \
                  .api=WAFFLE_CONTEXT_OPENGL,                           \
                  .version=waffle_version,                              \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_glXX_fwdcompat(waffle_version, error)                      \
static void test_gl_basic_gl##waffle_version##_fwdcompat(void **state)  \
{                                                                       \
    gl_basic_draw(state,                                                \
                  .api=WAFFLE_CONTEXT_OPENGL,                           \
                  .version=waffle_version,                              \
                  .forward_compatible=true,                             \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_glXX_core(waffle_version, error)                           \
static void test_gl_basic_gl##waffle_version##_core(void **state)       \
{                                                                       \
    gl_basic_draw(state,                                                \
                  .api=WAFFLE_CONTEXT_OPENGL,                           \
                  .version=waffle_version,                              \
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE,                 \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_glXX_core_fwdcompat(waffle_version, error)                 \
static void test_gl_basic_gl##waffle_version##_core_fwdcompat(void **state) \
{                                                                       \
    gl_basic_draw(state,                                                \
                  .api=WAFFLE_CONTEXT_OPENGL,                           \
                  .version=waffle_version,                              \
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE,                 \
                  .forward_compatible=true,                             \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_glXX_core_debug(waffle_version, error)                     \
static void test_gl_basic_gl##waffle_version##_core_debug(void **state) \
{                                                                       \
    gl_basic_draw(state,                                                \
                  .api=WAFFLE_CONTEXT_OPENGL,                           \
                  .version=waffle_version,                              \
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE,                 \
                  .debug=true,                                          \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_glXX_core_robust(waffle_version, error)                    \
static void test_gl_basic_gl##waffle_version##_core_robust(void **state) \
{                                                                       \
    gl_basic_draw(state,                                                \
                  .api=WAFFLE_CONTEXT_OPENGL,                           \
                  .version=waffle_version,                              \
                  .profile=WAFFLE_CONTEXT_CORE_PROFILE,                 \
                  .robust=true,                                         \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_glXX_compat(waffle_version, error)                         \
static void test_gl_basic_gl##waffle_version##_compat(void **state)     \
{                                                                       \
    gl_basic_draw(state,                                                \
                  .api=WAFFLE_CONTEXT_OPENGL,                           \
                  .version=waffle_version,                              \
                  .profile=WAFFLE_CONTEXT_COMPATIBILITY_PROFILE,        \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_glXX_compat_debug(waffle_version, error)                   \
static void test_gl_basic_gl##waffle_version##_compat_debug(void **state) \
{                                                                       \
    gl_basic_draw(state,                                                \
                  .api=WAFFLE_CONTEXT_OPENGL,                           \
                  .version=waffle_version,                              \
                  .profile=WAFFLE_CONTEXT_COMPATIBILITY_PROFILE,        \
                  .debug=true,                                          \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_glXX_compat_robust(waffle_version, error)                  \
static void test_gl_basic_gl##waffle_version##_compat_robust(void **state) \
{                                                                       \
    gl_basic_draw(state,                                                \
                  .api=WAFFLE_CONTEXT_OPENGL,                           \
                  .version=waffle_version,                              \
                  .profile=WAFFLE_CONTEXT_COMPATIBILITY_PROFILE,        \
                  .robust=true,                                         \
                  .expect_error=WAFFLE_##error);                        \
}

#define test_glesXX(api_version, waffle_version, error)                 \
static void test_gl_basic_gles##waffle_version(void **state)            \
{                                                                       \
    gl_basic_draw(state,                                                \
                  .api=WAFFLE_CONTEXT_OPENGL_ES##api_version,           \
                  .version=waffle_version,                              \
                  .expect_error=WAFFLE_##error);                        \
}


#define CREATE_TESTSUITE(waffle_platform, platform)                     \
                                                                        \
static int                                                              \
setup_##platform(void **state)                                          \
{                                                                       \
    return gl_basic_init(state, waffle_platform);                       \
}                                                                       \
                                                                        \
static int                                                              \
testsuite_##platform(void)                                              \
{                                                                       \
    const struct CMUnitTest tests[] = {                                 \
                                                                        \
        unit_test_make(test_gl_basic_gl_rgb),                           \
        unit_test_make(test_gl_basic_gl_rgba),                          \
        unit_test_make(test_gl_basic_gl_fwdcompat),                     \
        unit_test_make(test_gl_basic_gl_debug),                         \
        unit_test_make(test_gl_basic_gl_robust),                        \
                                                                        \
        unit_test_make(test_gl_basic_gl10),                             \
        unit_test_make(test_gl_basic_gl11),                             \
        unit_test_make(test_gl_basic_gl12),                             \
        unit_test_make(test_gl_basic_gl13),                             \
        unit_test_make(test_gl_basic_gl14),                             \
        unit_test_make(test_gl_basic_gl15),                             \
        unit_test_make(test_gl_basic_gl20),                             \
        unit_test_make(test_gl_basic_gl21),                             \
        unit_test_make(test_gl_basic_gl21_fwdcompat),                   \
                                                                        \
        unit_test_make(test_gl_basic_gl30),                             \
        unit_test_make(test_gl_basic_gl30_fwdcompat),                   \
        unit_test_make(test_gl_basic_gl31),                             \
        unit_test_make(test_gl_basic_gl31_fwdcompat),                   \
                                                                        \
        unit_test_make(test_gl_basic_gl32_core),                        \
        unit_test_make(test_gl_basic_gl32_core_fwdcompat),              \
        unit_test_make(test_gl_basic_gl32_core_debug),                  \
        unit_test_make(test_gl_basic_gl32_core_robust),                 \
        unit_test_make(test_gl_basic_gl33_core),                        \
        unit_test_make(test_gl_basic_gl40_core),                        \
        unit_test_make(test_gl_basic_gl41_core),                        \
        unit_test_make(test_gl_basic_gl42_core),                        \
        unit_test_make(test_gl_basic_gl43_core),                        \
                                                                        \
        unit_test_make(test_gl_basic_gl32_compat),                      \
        unit_test_make(test_gl_basic_gl32_compat_debug),                \
        unit_test_make(test_gl_basic_gl32_compat_robust),               \
        unit_test_make(test_gl_basic_gl33_compat),                      \
        unit_test_make(test_gl_basic_gl40_compat),                      \
        unit_test_make(test_gl_basic_gl41_compat),                      \
        unit_test_make(test_gl_basic_gl42_compat),                      \
        unit_test_make(test_gl_basic_gl43_compat),                      \
                                                                        \
        unit_test_make(test_gl_basic_gles1_rgb),                        \
        unit_test_make(test_gl_basic_gles1_rgba),                       \
        unit_test_make(test_gl_basic_gles1_fwdcompat),                  \
        unit_test_make(test_gl_basic_gles1_robust),                     \
        unit_test_make(test_gl_basic_gles1_debug),                      \
        unit_test_make(test_gl_basic_gles10),                           \
        unit_test_make(test_gl_basic_gles11),                           \
                                                                        \
        unit_test_make(test_gl_basic_gles2_rgb),                        \
        unit_test_make(test_gl_basic_gles2_rgba),                       \
        unit_test_make(test_gl_basic_gles2_fwdcompat),                  \
        unit_test_make(test_gl_basic_gles2_debug),                      \
        unit_test_make(test_gl_basic_gles2_robust),                     \
        unit_test_make(test_gl_basic_gles20),                           \
                                                                        \
        unit_test_make(test_gl_basic_gles3_rgb),                        \
        unit_test_make(test_gl_basic_gles3_rgba),                       \
        unit_test_make(test_gl_basic_gles3_fwdcompat),                  \
        unit_test_make(test_gl_basic_gles3_debug),                      \
        unit_test_make(test_gl_basic_gles3_robust),                     \
        unit_test_make(test_gl_basic_gles30),                           \
                                                                        \
    };                                                                  \
                                                                        \
    return cmocka_run_group_tests_name(#platform, tests, NULL, NULL);   \
}

//
// Most of the following tests will return ERROR_UNSUPPORTED_ON_PLATFORM
// on Apple/CGL, where NO_ERROR is expected.
// This is safe, as the test is skipped when the said error occurs.
//
// As BAD_ATTRIBUTE (core validation) takes greater precedence over
// UNSUPPORTED_ON_PLATFORM (platform specific one).
// Thus we're safe to use the former here, eventhough CGL has support
// for neither GLES* nor fwdcompa "CGL and everyone else".
//
test_XX_rgb(gl, OPENGL, NO_ERROR)
test_XX_rgba(gl, OPENGL, NO_ERROR)
test_XX_fwdcompat(gl, OPENGL, ERROR_BAD_ATTRIBUTE)
test_XX_debug(gl, OPENGL, NO_ERROR)
test_XX_robust(gl, OPENGL, NO_ERROR)

test_glXX(10, NO_ERROR)
test_glXX(11, NO_ERROR)
test_glXX(12, NO_ERROR)
test_glXX(13, NO_ERROR)
test_glXX(14, NO_ERROR)
test_glXX(15, NO_ERROR)
test_glXX(20, NO_ERROR)
test_glXX(21, NO_ERROR)
test_glXX_fwdcompat(21, ERROR_BAD_ATTRIBUTE)
test_glXX(30, NO_ERROR)
test_glXX_fwdcompat(30, NO_ERROR)
test_glXX(31, NO_ERROR)
test_glXX_fwdcompat(31, NO_ERROR)

test_glXX_core(32, NO_ERROR)
test_glXX_core_fwdcompat(32, NO_ERROR)
test_glXX_core_debug(32, NO_ERROR)
test_glXX_core_robust(32, NO_ERROR)
test_glXX_core(33, NO_ERROR)
test_glXX_core(40, NO_ERROR)
test_glXX_core(41, NO_ERROR)
test_glXX_core(42, NO_ERROR)
test_glXX_core(43, NO_ERROR)

test_glXX_compat(32, NO_ERROR)
test_glXX_compat_debug(32, NO_ERROR)
test_glXX_compat_robust(32, NO_ERROR)
test_glXX_compat(33, NO_ERROR)
test_glXX_compat(40, NO_ERROR)
test_glXX_compat(41, NO_ERROR)
test_glXX_compat(42, NO_ERROR)
test_glXX_compat(43, NO_ERROR)

test_XX_rgb(gles1, OPENGL_ES1, NO_ERROR)
test_XX_rgba(gles1, OPENGL_ES1, NO_ERROR)
test_XX_fwdcompat(gles1, OPENGL_ES1, ERROR_BAD_ATTRIBUTE)
test_XX_debug(gles1, OPENGL_ES1, NO_ERROR)
test_XX_robust(gles1, OPENGL_ES1, NO_ERROR)
test_glesXX(1, 10, NO_ERROR)
test_glesXX(1, 11, NO_ERROR)

test_XX_rgb(gles2, OPENGL_ES2, NO_ERROR)
test_XX_rgba(gles2, OPENGL_ES2, NO_ERROR)
test_XX_fwdcompat(gles2, OPENGL_ES2, ERROR_BAD_ATTRIBUTE)
test_XX_debug(gles2, OPENGL_ES2, NO_ERROR)
test_XX_robust(gles2, OPENGL_ES2, NO_ERROR)
test_glesXX(2, 20, NO_ERROR)

test_XX_rgb(gles3, OPENGL_ES3, NO_ERROR)
test_XX_rgba(gles3, OPENGL_ES3, NO_ERROR)
test_XX_fwdcompat(gles3, OPENGL_ES3, ERROR_BAD_ATTRIBUTE)
test_XX_debug(gles3, OPENGL_ES3, NO_ERROR)
test_XX_robust(gles3, OPENGL_ES3, NO_ERROR)
test_glesXX(3, 30, NO_ERROR)


#ifdef WAFFLE_HAS_CGL

#define unit_test_make(name)                                            \
    cmocka_unit_test_setup_teardown(name, setup_cgl, gl_basic_fini)

CREATE_TESTSUITE(WAFFLE_PLATFORM_CGL, cgl)

#undef unit_test_make

#endif // WAFFLE_HAS_CGL

#ifdef WAFFLE_HAS_GBM

#define unit_test_make(name)                                            \
    cmocka_unit_test_setup_teardown(name, setup_gbm, gl_basic_fini)

CREATE_TESTSUITE(WAFFLE_PLATFORM_GBM, gbm)

#undef unit_test_make

#endif // WAFFLE_HAS_GBM

#ifdef WAFFLE_HAS_GLX

#define unit_test_make(name)                                            \
    cmocka_unit_test_setup_teardown(name, setup_glx, gl_basic_fini)

CREATE_TESTSUITE(WAFFLE_PLATFORM_GLX, glx)

#undef unit_test_make

#endif // WAFFLE_HAS_GLX

#ifdef WAFFLE_HAS_SURFACELESS_EGL

#define unit_test_make(name)                                            \
    cmocka_unit_test_setup_teardown(name, setup_surfaceless_egl, gl_basic_fini)

CREATE_TESTSUITE(WAFFLE_PLATFORM_SURFACELESS_EGL, surfaceless_egl)

#undef unit_test_make

#endif // WAFFLE_HAS_SURFACELESS_EGL

#ifdef WAFFLE_HAS_WAYLAND

#define unit_test_make(name)                                            \
    cmocka_unit_test_setup_teardown(name, setup_wayland, gl_basic_fini)

CREATE_TESTSUITE(WAFFLE_PLATFORM_WAYLAND, wayland)

#undef unit_test_make

#endif // WAFFLE_HAS_WAYLAND

#ifdef WAFFLE_HAS_X11_EGL

#define unit_test_make(name)                                            \
    cmocka_unit_test_setup_teardown(name, setup_x11_egl, gl_basic_fini)

CREATE_TESTSUITE(WAFFLE_PLATFORM_X11_EGL, x11_egl)

#undef unit_test_make

#endif // WAFFLE_HAS_X11_EGL

#ifdef WAFFLE_HAS_WGL

#define unit_test_make(name)                                            \
    cmocka_unit_test_setup_teardown(name, setup_wgl, gl_basic_fini)

CREATE_TESTSUITE(WAFFLE_PLATFORM_WGL, wgl)

#undef unit_test_make

#endif // WAFFLE_HAS_WGL

#undef test_glesXX

#undef test_glXX_compat_robust
#undef test_glXX_compat_debug
#undef test_glXX_compat
#undef test_glXX_core_robust
#undef test_glXX_core_debug
#undef test_glXX_core_fwdcompat
#undef test_glXX_core
#undef test_glXX_fwdcompat
#undef test_glXX

#undef test_XX_robust
#undef test_XX_debug
#undef test_XX_fwdcompat
#undef test_XX_rgba
#undef test_XX_rgb

#ifdef __APPLE__

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

static const char *usage_message =
    "Usage:\n"
    "    gl_basic_test <Required Parameter> [Options]\n"
    "\n"
    "Description:\n"
    "    Run the basic functionality tests.\n"
    "\n"
    "Required Parameter:\n"
    "    -p, --platform\n"
    "        One of: cgl, gbm, glx, wayland, wgl or x11_egl\n"
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
    {WAFFLE_PLATFORM_GBM,       "gbm"           },
    {WAFFLE_PLATFORM_GLX,       "glx"           },
    {WAFFLE_PLATFORM_WAYLAND,   "wayland"       },
    {WAFFLE_PLATFORM_WGL,       "wgl"           },
    {WAFFLE_PLATFORM_X11_EGL,   "x11_egl"       },
    {WAFFLE_PLATFORM_SURFACELESS_EGL,   "surfaceless_egl"   },
    {WAFFLE_PLATFORM_SURFACELESS_EGL,   "sl"                },
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
parse_args(int argc, char *argv[], int *out_platform)
{
    bool ok;
    bool loop_get_opt = true;
    int platform = 0;

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
                ok = enum_map_translate_str(platform_map, optarg, &platform);
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

    if (!platform) {
        usage_error_printf("--platform is required");
    }

    *out_platform = platform;
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
        return testsuite_cgl();
#endif
#ifdef WAFFLE_HAS_GBM
    case WAFFLE_PLATFORM_GBM:
        return testsuite_gbm();
#endif
#ifdef WAFFLE_HAS_GLX
    case WAFFLE_PLATFORM_GLX:
        return testsuite_glx();
#endif
#ifdef WAFFLE_HAS_SURFACELESS_EGL
    case WAFFLE_PLATFORM_SURFACELESS_EGL:
        return testsuite_surfaceless_egl();
#endif
#ifdef WAFFLE_HAS_WAYLAND
    case WAFFLE_PLATFORM_WAYLAND:
        return testsuite_wayland();
#endif
#ifdef WAFFLE_HAS_WGL
    case WAFFLE_PLATFORM_WGL:
        return testsuite_wgl();
#endif
#ifdef WAFFLE_HAS_X11_EGL
    case WAFFLE_PLATFORM_X11_EGL:
        return testsuite_x11_egl();
#endif
    default:
        abort();
        break;
    }

    return 0;
}
