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

#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include <cmocka.h>

#include "wcore_config_attrs.h"
#include "wcore_error.h"

struct test_state_wcore_config_attrs {

    struct wcore_config_attrs actual_attrs;
    struct wcore_config_attrs expect_attrs;
};

static void
setup(void **state) {
    static const struct wcore_config_attrs default_attrs = {
        // There is no default context api, so arbitrarily choose OpenGL. The
        // remaining context attrs are the defaults for OpenGL.
        .context_api            = WAFFLE_CONTEXT_OPENGL,
        .context_major_version  = 1,
        .context_minor_version  = 0,
        .context_profile        = WAFFLE_NONE,
        .context_debug          = false,
        .context_forward_compatible = false,

        .rgb_size               = 0,
        .rgba_size              = 0,

        .red_size               = 0,
        .green_size             = 0,
        .blue_size              = 0,
        .alpha_size             = 0,

        .depth_size             = 0,
        .stencil_size           = 0,

        .sample_buffers         = 0,
        .samples                = 0,

        .double_buffered        = true,
    };

    struct test_state_wcore_config_attrs *ts;

    wcore_error_reset();

    *state = ts = calloc(1, sizeof(*ts));

    // Fill actual_attrs with canaries.
    memset(&ts->actual_attrs, 0x99, sizeof(ts->actual_attrs));

    // Set expect_attrs to defaults.
    memcpy(&ts->expect_attrs, &default_attrs, sizeof(ts->expect_attrs));
}

static void
teardown(void **state) {
    free(*state);
}

static void
test_wcore_config_attrs_null_attrib_list(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    assert_false(wcore_config_attrs_parse(NULL, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_empty_attrib_list(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;
    const int32_t attrib_list[] = {0};

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_gl_defaults(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;
    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API, WAFFLE_CONTEXT_OPENGL,
        0,
    };

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));

    assert_int_equal(ts->actual_attrs.context_major_version, 1);
    assert_int_equal(ts->actual_attrs.context_minor_version, 0);
    assert_int_equal(ts->actual_attrs.context_profile, WAFFLE_NONE);
    assert_int_equal(ts->actual_attrs.red_size, 0);
    assert_int_equal(ts->actual_attrs.green_size, 0);
    assert_int_equal(ts->actual_attrs.blue_size, 0);
    assert_int_equal(ts->actual_attrs.alpha_size, 0);
    assert_int_equal(ts->actual_attrs.depth_size, 0);
    assert_int_equal(ts->actual_attrs.stencil_size, 0);
    assert_int_equal(ts->actual_attrs.sample_buffers, false);
    assert_int_equal(ts->actual_attrs.samples, 0);
    assert_int_equal(ts->actual_attrs.double_buffered, true);
    assert_int_equal(ts->actual_attrs.accum_buffer, false);
    assert_int_equal(ts->actual_attrs.context_forward_compatible, false);
}

static void
test_wcore_config_attrs_gles1_defaults(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API, WAFFLE_CONTEXT_OPENGL_ES1,
        0,
    };

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));

    assert_int_equal(ts->actual_attrs.context_major_version, 1);
    assert_int_equal(ts->actual_attrs.context_minor_version, 0);
    assert_int_equal(ts->actual_attrs.context_profile, WAFFLE_NONE);

    assert_int_equal(ts->actual_attrs.red_size, 0);
    assert_int_equal(ts->actual_attrs.green_size, 0);
    assert_int_equal(ts->actual_attrs.blue_size, 0);
    assert_int_equal(ts->actual_attrs.alpha_size, 0);
    assert_int_equal(ts->actual_attrs.depth_size, 0);
    assert_int_equal(ts->actual_attrs.stencil_size, 0);
    assert_int_equal(ts->actual_attrs.sample_buffers, false);
    assert_int_equal(ts->actual_attrs.samples, 0);
    assert_int_equal(ts->actual_attrs.double_buffered, true);
    assert_int_equal(ts->actual_attrs.accum_buffer, false);
}

static void
test_wcore_config_attrs_gles2_defaults(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API, WAFFLE_CONTEXT_OPENGL_ES2,
        0,
    };

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));

    assert_int_equal(ts->actual_attrs.context_major_version, 2);
    assert_int_equal(ts->actual_attrs.context_minor_version, 0);
    assert_int_equal(ts->actual_attrs.context_profile, WAFFLE_NONE);

    assert_int_equal(ts->actual_attrs.red_size, 0);
    assert_int_equal(ts->actual_attrs.green_size, 0);
    assert_int_equal(ts->actual_attrs.blue_size, 0);
    assert_int_equal(ts->actual_attrs.alpha_size, 0);
    assert_int_equal(ts->actual_attrs.depth_size, 0);
    assert_int_equal(ts->actual_attrs.stencil_size, 0);
    assert_int_equal(ts->actual_attrs.sample_buffers, false);
    assert_int_equal(ts->actual_attrs.samples, 0);
    assert_int_equal(ts->actual_attrs.double_buffered, true);
    assert_int_equal(ts->actual_attrs.accum_buffer, false);
}

static void
test_wcore_config_attrs_gles3_defaults(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API, WAFFLE_CONTEXT_OPENGL_ES3,
        0,
    };

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));

    assert_int_equal(ts->actual_attrs.context_major_version, 3);
    assert_int_equal(ts->actual_attrs.context_minor_version, 0);
    assert_int_equal(ts->actual_attrs.context_profile, WAFFLE_NONE);

    assert_int_equal(ts->actual_attrs.red_size, 0);
    assert_int_equal(ts->actual_attrs.green_size, 0);
    assert_int_equal(ts->actual_attrs.blue_size, 0);
    assert_int_equal(ts->actual_attrs.alpha_size, 0);
    assert_int_equal(ts->actual_attrs.depth_size, 0);
    assert_int_equal(ts->actual_attrs.stencil_size, 0);
    assert_int_equal(ts->actual_attrs.sample_buffers, false);
    assert_int_equal(ts->actual_attrs.samples, 0);
    assert_int_equal(ts->actual_attrs.double_buffered, true);
    assert_int_equal(ts->actual_attrs.accum_buffer, false);
}

static void
test_wcore_config_attrs_bad_attr(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,              WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_CORE_PROFILE,     WAFFLE_CONTEXT_OPENGL_ES2,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_gl31_none_profile(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,   3,
        WAFFLE_CONTEXT_MINOR_VERSION,   1,
        WAFFLE_CONTEXT_PROFILE,         WAFFLE_NONE,
        0,
    };

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(ts->actual_attrs.context_profile, WAFFLE_NONE);
}

static void
test_wcore_config_attrs_gl31_trash_profile(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,   3,
        WAFFLE_CONTEXT_MINOR_VERSION,   1,
        WAFFLE_CONTEXT_PROFILE,         0x314159,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_gl31_core(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,   3,
        WAFFLE_CONTEXT_MINOR_VERSION,   1,
        WAFFLE_CONTEXT_PROFILE,         WAFFLE_CONTEXT_CORE_PROFILE,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_gl31_compat(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,   3,
        WAFFLE_CONTEXT_MINOR_VERSION,   1,
        WAFFLE_CONTEXT_PROFILE,         WAFFLE_CONTEXT_COMPATIBILITY_PROFILE,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_gl32_none_profile(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,   3,
        WAFFLE_CONTEXT_MINOR_VERSION,   2,
        WAFFLE_CONTEXT_PROFILE,         WAFFLE_NONE,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_gl32_trash_profile(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,   3,
        WAFFLE_CONTEXT_MINOR_VERSION,   2,
        WAFFLE_CONTEXT_PROFILE,         0x314159,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_gl32_core(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,   3,
        WAFFLE_CONTEXT_MINOR_VERSION,   2,
        WAFFLE_CONTEXT_PROFILE,         WAFFLE_CONTEXT_CORE_PROFILE,
        0,
    };

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(ts->actual_attrs.context_profile, WAFFLE_CONTEXT_CORE_PROFILE);
}

static void
test_wcore_config_attrs_gl32_compat(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,   3,
        WAFFLE_CONTEXT_MINOR_VERSION,   2,
        WAFFLE_CONTEXT_PROFILE,         WAFFLE_CONTEXT_COMPATIBILITY_PROFILE,
        0,
    };

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(ts->actual_attrs.context_profile, WAFFLE_CONTEXT_COMPATIBILITY_PROFILE);
}

static void
test_wcore_config_attrs_gles1_profile_is_checked(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL_ES1,
        WAFFLE_CONTEXT_PROFILE,         WAFFLE_CONTEXT_CORE_PROFILE,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_true(strstr(wcore_error_get_info()->message, "WAFFLE_CONTEXT_PROFILE"));
}

static void
test_wcore_config_attrs_gles2_profile_is_checked(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL_ES2,
        WAFFLE_CONTEXT_PROFILE,         WAFFLE_CONTEXT_CORE_PROFILE,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_true(strstr(wcore_error_get_info()->message, "WAFFLE_CONTEXT_PROFILE"));
}

static void
test_wcore_config_attrs_gles3_profile_is_checked(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL_ES3,
        WAFFLE_CONTEXT_PROFILE,         WAFFLE_CONTEXT_CORE_PROFILE,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_true(strstr(wcore_error_get_info()->message, "WAFFLE_CONTEXT_PROFILE"));
}

static void
test_wcore_config_attrs_negative_minor_version(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MINOR_VERSION,   -1,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_gles10(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL_ES1,
        WAFFLE_CONTEXT_MAJOR_VERSION,   1,
        WAFFLE_CONTEXT_MINOR_VERSION,   0,
        0,
    };

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(ts->actual_attrs.context_major_version, 1);
    assert_int_equal(ts->actual_attrs.context_minor_version, 0);
}

static void
test_wcore_config_attrs_gles11(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL_ES1,
        WAFFLE_CONTEXT_MAJOR_VERSION,   1,
        WAFFLE_CONTEXT_MINOR_VERSION,   1,
        0,
    };

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(ts->actual_attrs.context_major_version, 1);
    assert_int_equal(ts->actual_attrs.context_minor_version, 1);
}

static void
test_wcore_config_attrs_gles12_is_bad(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL_ES1,
        WAFFLE_CONTEXT_MAJOR_VERSION,   1,
        WAFFLE_CONTEXT_MINOR_VERSION,   2,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_gles20(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL_ES2,
        WAFFLE_CONTEXT_MAJOR_VERSION,   2,
        WAFFLE_CONTEXT_MINOR_VERSION,   0,
        0,
    };

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(ts->actual_attrs.context_major_version, 2);
    assert_int_equal(ts->actual_attrs.context_minor_version, 0);
}

static void
test_wcore_config_attrs_gles21(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL_ES2,
        WAFFLE_CONTEXT_MAJOR_VERSION,   2,
        WAFFLE_CONTEXT_MINOR_VERSION,   1,
        0,
    };

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(ts->actual_attrs.context_major_version, 2);
    assert_int_equal(ts->actual_attrs.context_minor_version, 1);
}

static void
test_wcore_config_attrs_gles2_with_version_30(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL_ES2,
        WAFFLE_CONTEXT_MAJOR_VERSION,   3,
        WAFFLE_CONTEXT_MINOR_VERSION,   0,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_gles30(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL_ES3,
        WAFFLE_CONTEXT_MAJOR_VERSION,   3,
        WAFFLE_CONTEXT_MINOR_VERSION,   0,
        0,
    };

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(ts->actual_attrs.context_major_version, 3);
    assert_int_equal(ts->actual_attrs.context_minor_version, 0);
}

static void
test_wcore_config_attrs_gles31(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL_ES3,
        WAFFLE_CONTEXT_MAJOR_VERSION,   3,
        WAFFLE_CONTEXT_MINOR_VERSION,   1,
        0,
    };

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(ts->actual_attrs.context_major_version, 3);
    assert_int_equal(ts->actual_attrs.context_minor_version, 1);
}

static void
test_wcore_config_attrs_gles3_with_version_20(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL_ES3,
        WAFFLE_CONTEXT_MAJOR_VERSION,   2,
        WAFFLE_CONTEXT_MINOR_VERSION,   0,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_gles3_with_version_40(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL_ES3,
        WAFFLE_CONTEXT_MAJOR_VERSION,   4,
        WAFFLE_CONTEXT_MINOR_VERSION,   0,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_color_buffer_size(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API, WAFFLE_CONTEXT_OPENGL,

        WAFFLE_SAMPLES,     4, // noise

        WAFFLE_ALPHA_SIZE,  8,
        WAFFLE_RED_SIZE,    5,
        WAFFLE_GREEN_SIZE,  6,
        0,
    };

    ts->expect_attrs.samples = 4;
    ts->expect_attrs.alpha_size = 8;
    ts->expect_attrs.red_size = 5;
    ts->expect_attrs.green_size = 6;

    ts->expect_attrs.rgb_size = 5 + 6;
    ts->expect_attrs.rgba_size = 8 + 5 + 6;

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_memory_equal(&ts->actual_attrs, &ts->expect_attrs, sizeof(ts->expect_attrs));
}

static void
test_wcore_config_attrs_double_buffered_is_true(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,      WAFFLE_CONTEXT_OPENGL,
        WAFFLE_DOUBLE_BUFFERED, 1,
        0,
    };

    ts->expect_attrs.double_buffered = true;

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_memory_equal(&ts->actual_attrs, &ts->expect_attrs, sizeof(ts->expect_attrs));
}

static void
test_wcore_config_attrs_double_buffered_is_false(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_DOUBLE_BUFFERED, 0,
        0,
    };

    ts->expect_attrs.double_buffered = false;

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_memory_equal(&ts->actual_attrs, &ts->expect_attrs, sizeof(ts->expect_attrs));
}

static void
test_wcore_config_attrs_double_buffered_is_bad(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_DOUBLE_BUFFERED, 0x31415926,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_true(strstr(wcore_error_get_info()->message, "WAFFLE_DOUBLE_BUFFERED"));
    assert_true(strstr(wcore_error_get_info()->message, "0x31415926"));
}

static void
test_wcore_config_attrs_sample_buffers_is_bad(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_SAMPLE_BUFFERS,  0x31415926,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_true(strstr(wcore_error_get_info()->message, "WAFFLE_SAMPLE_BUFFERS"));
    assert_true(strstr(wcore_error_get_info()->message, "0x31415926"));
}

static void
test_wcore_config_attrs_accum_buffer_is_bad(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_ACCUM_BUFFER,    0x31415926,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_true(strstr(wcore_error_get_info()->message, "WAFFLE_ACCUM_BUFFER"));
    assert_true(strstr(wcore_error_get_info()->message, "0x31415926"));
}

static void
test_wcore_config_attrs_core_profile_and_accum_buffer(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                 WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,       3,
        WAFFLE_CONTEXT_MINOR_VERSION,       2,
        WAFFLE_CONTEXT_PROFILE,             WAFFLE_CONTEXT_CORE_PROFILE,

        WAFFLE_ACCUM_BUFFER,                true,

        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_negative_red(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,       WAFFLE_CONTEXT_OPENGL,
        WAFFLE_RED_SIZE,          -2,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_true(strstr(wcore_error_get_info()->message, "WAFFLE_RED_SIZE"));
    assert_true(strstr(wcore_error_get_info()->message, "-2"));
}

static void
test_wcore_config_attrs_negative_green(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,       WAFFLE_CONTEXT_OPENGL,
        WAFFLE_GREEN_SIZE,        -2,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_true(strstr(wcore_error_get_info()->message, "WAFFLE_GREEN_SIZE"));
    assert_true(strstr(wcore_error_get_info()->message, "-2"));
}

static void
test_wcore_config_attrs_negative_blue(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,       WAFFLE_CONTEXT_OPENGL,
        WAFFLE_BLUE_SIZE,         -2,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_true(strstr(wcore_error_get_info()->message, "WAFFLE_BLUE_SIZE"));
    assert_true(strstr(wcore_error_get_info()->message, "-2"));
}

static void
test_wcore_config_attrs_negative_alpha(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,       WAFFLE_CONTEXT_OPENGL,
        WAFFLE_ALPHA_SIZE,        -2,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_true(strstr(wcore_error_get_info()->message, "WAFFLE_ALPHA_SIZE"));
    assert_true(strstr(wcore_error_get_info()->message, "-2"));
}

static void
test_wcore_config_attrs_negative_depth(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,       WAFFLE_CONTEXT_OPENGL,
        WAFFLE_DEPTH_SIZE,        -2,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_true(strstr(wcore_error_get_info()->message, "WAFFLE_DEPTH_SIZE"));
    assert_true(strstr(wcore_error_get_info()->message, "-2"));
}

static void
test_wcore_config_attrs_negative_stencil(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,       WAFFLE_CONTEXT_OPENGL,
        WAFFLE_STENCIL_SIZE,      -2,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_true(strstr(wcore_error_get_info()->message, "WAFFLE_STENCIL_SIZE"));
    assert_true(strstr(wcore_error_get_info()->message, "-2"));
}

static void
test_wcore_config_attrs_negative_samples(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,       WAFFLE_CONTEXT_OPENGL,
        WAFFLE_SAMPLES,           -2,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_true(strstr(wcore_error_get_info()->message, "WAFFLE_SAMPLES"));
    assert_true(strstr(wcore_error_get_info()->message, "-2"));
}


static void
test_wcore_config_attrs_forward_compat_gles1_emits_bad_attribute(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL_ES1,
        WAFFLE_CONTEXT_FORWARD_COMPATIBLE,      true,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_forward_compat_gles2_emits_bad_attribute(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL_ES2,
        WAFFLE_CONTEXT_FORWARD_COMPATIBLE,      true,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_forward_compat_gles3_emits_bad_attribute(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL_ES3,
        WAFFLE_CONTEXT_FORWARD_COMPATIBLE,      true,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_forward_compat_gl_emits_bad_attribute(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_FORWARD_COMPATIBLE,      true,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_forward_compat_gl10_emits_bad_attribute(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,           1,
        WAFFLE_CONTEXT_MAJOR_VERSION,           0,
        WAFFLE_CONTEXT_FORWARD_COMPATIBLE,      true,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_forward_compat_gl21_emits_bad_attribute(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,           2,
        WAFFLE_CONTEXT_MAJOR_VERSION,           1,
        WAFFLE_CONTEXT_FORWARD_COMPATIBLE,      true,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
}

static void
test_wcore_config_attrs_forward_compat_gl30(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,           3,
        WAFFLE_CONTEXT_MAJOR_VERSION,           0,
        WAFFLE_CONTEXT_FORWARD_COMPATIBLE,      true,
        0,
    };

    ts->expect_attrs.context_major_version = 3;
    ts->expect_attrs.context_minor_version = 0;
    ts->expect_attrs.context_forward_compatible = true;

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_NO_ERROR);
    assert_memory_equal(&ts->actual_attrs, &ts->expect_attrs, sizeof(ts->expect_attrs));
}

static void
test_wcore_config_attrs_forward_compat_gl32(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,           3,
        WAFFLE_CONTEXT_MINOR_VERSION,           2,
        WAFFLE_CONTEXT_FORWARD_COMPATIBLE,      true,
        0,
    };

    ts->expect_attrs.context_major_version = 3;
    ts->expect_attrs.context_minor_version = 2;
    ts->expect_attrs.context_profile = WAFFLE_CONTEXT_CORE_PROFILE;
    ts->expect_attrs.context_forward_compatible = true;

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_NO_ERROR);
    assert_memory_equal(&ts->actual_attrs, &ts->expect_attrs, sizeof(ts->expect_attrs));
}

static void
test_wcore_config_attrs_forward_compat_gl41(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,           4,
        WAFFLE_CONTEXT_MINOR_VERSION,           1,
        WAFFLE_CONTEXT_FORWARD_COMPATIBLE,      true,
        0,
    };

    ts->expect_attrs.context_major_version = 4;
    ts->expect_attrs.context_minor_version = 1;
    ts->expect_attrs.context_profile = WAFFLE_CONTEXT_CORE_PROFILE;
    ts->expect_attrs.context_forward_compatible = true;

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_NO_ERROR);
    assert_memory_equal(&ts->actual_attrs, &ts->expect_attrs, sizeof(ts->expect_attrs));
}

static void
test_wcore_config_attrs_forward_compat_gl30_dont_care(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,           3,
        WAFFLE_CONTEXT_MAJOR_VERSION,           0,
        WAFFLE_CONTEXT_FORWARD_COMPATIBLE,      WAFFLE_DONT_CARE,
        0,
    };

    ts->expect_attrs.context_major_version = 3;
    ts->expect_attrs.context_minor_version = 0;
    ts->expect_attrs.context_forward_compatible = false;

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_NO_ERROR);
    assert_memory_equal(&ts->actual_attrs, &ts->expect_attrs, sizeof(ts->expect_attrs));
}

static void
test_wcore_config_attrs_forward_compat_gl30_bad_value(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,           3,
        WAFFLE_CONTEXT_MAJOR_VERSION,           0,
        WAFFLE_CONTEXT_FORWARD_COMPATIBLE,      2,
        0,
    };

    assert_false(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_true(strstr(wcore_error_get_info()->message, "2"));
}

static void
test_wcore_config_attrs_debug_gl(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_DEBUG,                   true,
        0,
    };

    ts->expect_attrs.context_api = WAFFLE_CONTEXT_OPENGL;
    ts->expect_attrs.context_debug = true;

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_NO_ERROR);
    assert_memory_equal(&ts->actual_attrs, &ts->expect_attrs, sizeof(ts->expect_attrs));
}

static void
test_wcore_config_attrs_debug_gl21(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,           2,
        WAFFLE_CONTEXT_MINOR_VERSION,           1,
        WAFFLE_CONTEXT_DEBUG,                   true,
        0,
    };

    ts->expect_attrs.context_api = WAFFLE_CONTEXT_OPENGL;
    ts->expect_attrs.context_major_version = 2;
    ts->expect_attrs.context_minor_version = 1;
    ts->expect_attrs.context_debug = true;

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_NO_ERROR);
    assert_memory_equal(&ts->actual_attrs, &ts->expect_attrs, sizeof(ts->expect_attrs));
}

static void
test_wcore_config_attrs_debug_gl32_core(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_PROFILE,                 WAFFLE_CONTEXT_CORE_PROFILE,
        WAFFLE_CONTEXT_MAJOR_VERSION,           3,
        WAFFLE_CONTEXT_MINOR_VERSION,           2,
        WAFFLE_CONTEXT_DEBUG,                   true,
        0,
    };

    ts->expect_attrs.context_api = WAFFLE_CONTEXT_OPENGL;
    ts->expect_attrs.context_profile = WAFFLE_CONTEXT_CORE_PROFILE;
    ts->expect_attrs.context_major_version = 3;
    ts->expect_attrs.context_minor_version = 2;
    ts->expect_attrs.context_debug = true;

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_NO_ERROR);
    assert_memory_equal(&ts->actual_attrs, &ts->expect_attrs, sizeof(ts->expect_attrs));
}

static void
test_wcore_config_attrs_debug_gl32_compat(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_PROFILE,                 WAFFLE_CONTEXT_COMPATIBILITY_PROFILE,
        WAFFLE_CONTEXT_MAJOR_VERSION,           3,
        WAFFLE_CONTEXT_MINOR_VERSION,           2,
        WAFFLE_CONTEXT_DEBUG,                   true,
        0,
    };

    ts->expect_attrs.context_api = WAFFLE_CONTEXT_OPENGL;
    ts->expect_attrs.context_profile = WAFFLE_CONTEXT_COMPATIBILITY_PROFILE;
    ts->expect_attrs.context_major_version = 3;
    ts->expect_attrs.context_minor_version = 2;
    ts->expect_attrs.context_debug = true;

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_NO_ERROR);
    assert_memory_equal(&ts->actual_attrs, &ts->expect_attrs, sizeof(ts->expect_attrs));
}

static void
test_wcore_config_attrs_debug_gles1(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL_ES1,
        WAFFLE_CONTEXT_DEBUG,                   true,
        0,
    };

    ts->expect_attrs.context_api = WAFFLE_CONTEXT_OPENGL_ES1;
    ts->expect_attrs.context_major_version = 1;
    ts->expect_attrs.context_debug = true;

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_NO_ERROR);
    assert_memory_equal(&ts->actual_attrs, &ts->expect_attrs, sizeof(ts->expect_attrs));
}

static void
test_wcore_config_attrs_debug_gles2(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL_ES2,
        WAFFLE_CONTEXT_DEBUG,                   true,
        0,
    };

    ts->expect_attrs.context_api = WAFFLE_CONTEXT_OPENGL_ES2;
    ts->expect_attrs.context_major_version = 2;
    ts->expect_attrs.context_debug = true;

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_NO_ERROR);
    assert_memory_equal(&ts->actual_attrs, &ts->expect_attrs, sizeof(ts->expect_attrs));
}

static void
test_wcore_config_attrs_debug_gles3(void **state) {
    struct test_state_wcore_config_attrs *ts = *state;

    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                     WAFFLE_CONTEXT_OPENGL_ES3,
        WAFFLE_CONTEXT_DEBUG,                   true,
        0,
    };

    ts->expect_attrs.context_api = WAFFLE_CONTEXT_OPENGL_ES3;
    ts->expect_attrs.context_major_version = 3;
    ts->expect_attrs.context_debug = true;

    assert_true(wcore_config_attrs_parse(attrib_list, &ts->actual_attrs));
    assert_int_equal(wcore_error_get_code(), WAFFLE_NO_ERROR);
    assert_memory_equal(&ts->actual_attrs, &ts->expect_attrs, sizeof(ts->expect_attrs));
}

int
main(void) {
    const UnitTest tests[] = {
        #define unit_test_make(name) unit_test_setup_teardown(name, setup, teardown)

        unit_test_make(test_wcore_config_attrs_null_attrib_list),
        unit_test_make(test_wcore_config_attrs_empty_attrib_list),
        unit_test_make(test_wcore_config_attrs_gl_defaults),
        unit_test_make(test_wcore_config_attrs_gles1_defaults),
        unit_test_make(test_wcore_config_attrs_gles2_defaults),
        unit_test_make(test_wcore_config_attrs_gles3_defaults),
        unit_test_make(test_wcore_config_attrs_bad_attr),
        unit_test_make(test_wcore_config_attrs_gl31_none_profile),
        unit_test_make(test_wcore_config_attrs_gl31_trash_profile),
        unit_test_make(test_wcore_config_attrs_gl31_core),
        unit_test_make(test_wcore_config_attrs_gl31_compat),
        unit_test_make(test_wcore_config_attrs_gl32_none_profile),
        unit_test_make(test_wcore_config_attrs_gl32_trash_profile),
        unit_test_make(test_wcore_config_attrs_gl32_core),
        unit_test_make(test_wcore_config_attrs_gl32_compat),
        unit_test_make(test_wcore_config_attrs_gles1_profile_is_checked),
        unit_test_make(test_wcore_config_attrs_gles2_profile_is_checked),
        unit_test_make(test_wcore_config_attrs_gles3_profile_is_checked),
        unit_test_make(test_wcore_config_attrs_negative_minor_version),
        unit_test_make(test_wcore_config_attrs_gles10),
        unit_test_make(test_wcore_config_attrs_gles11),
        unit_test_make(test_wcore_config_attrs_gles12_is_bad),
        unit_test_make(test_wcore_config_attrs_gles20),
        unit_test_make(test_wcore_config_attrs_gles21),
        unit_test_make(test_wcore_config_attrs_gles2_with_version_30),
        unit_test_make(test_wcore_config_attrs_gles30),
        unit_test_make(test_wcore_config_attrs_gles31),
        unit_test_make(test_wcore_config_attrs_gles3_with_version_20),
        unit_test_make(test_wcore_config_attrs_gles3_with_version_40),
        unit_test_make(test_wcore_config_attrs_color_buffer_size),
        unit_test_make(test_wcore_config_attrs_double_buffered_is_true),
        unit_test_make(test_wcore_config_attrs_double_buffered_is_false),
        unit_test_make(test_wcore_config_attrs_double_buffered_is_bad),
        unit_test_make(test_wcore_config_attrs_sample_buffers_is_bad),
        unit_test_make(test_wcore_config_attrs_accum_buffer_is_bad),
        unit_test_make(test_wcore_config_attrs_core_profile_and_accum_buffer),
        unit_test_make(test_wcore_config_attrs_negative_red),
        unit_test_make(test_wcore_config_attrs_negative_green),
        unit_test_make(test_wcore_config_attrs_negative_blue),
        unit_test_make(test_wcore_config_attrs_negative_alpha),
        unit_test_make(test_wcore_config_attrs_negative_depth),
        unit_test_make(test_wcore_config_attrs_negative_stencil),
        unit_test_make(test_wcore_config_attrs_negative_samples),
        unit_test_make(test_wcore_config_attrs_forward_compat_gles1_emits_bad_attribute),
        unit_test_make(test_wcore_config_attrs_forward_compat_gles2_emits_bad_attribute),
        unit_test_make(test_wcore_config_attrs_forward_compat_gles3_emits_bad_attribute),
        unit_test_make(test_wcore_config_attrs_forward_compat_gl_emits_bad_attribute),
        unit_test_make(test_wcore_config_attrs_forward_compat_gl10_emits_bad_attribute),
        unit_test_make(test_wcore_config_attrs_forward_compat_gl21_emits_bad_attribute),
        unit_test_make(test_wcore_config_attrs_forward_compat_gl30),
        unit_test_make(test_wcore_config_attrs_forward_compat_gl32),
        unit_test_make(test_wcore_config_attrs_forward_compat_gl41),
        unit_test_make(test_wcore_config_attrs_forward_compat_gl30_dont_care),
        unit_test_make(test_wcore_config_attrs_forward_compat_gl30_bad_value),
        unit_test_make(test_wcore_config_attrs_debug_gl),
        unit_test_make(test_wcore_config_attrs_debug_gl21),
        unit_test_make(test_wcore_config_attrs_debug_gl32_core),
        unit_test_make(test_wcore_config_attrs_debug_gl32_compat),
        unit_test_make(test_wcore_config_attrs_debug_gles1),
        unit_test_make(test_wcore_config_attrs_debug_gles2),
        unit_test_make(test_wcore_config_attrs_debug_gles3),

        #undef unit_test_make
    };

    return run_tests(tests);
}
