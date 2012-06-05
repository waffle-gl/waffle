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

#include <stdlib.h>
#include <string.h>

#include <waffle/waffle_enum.h>
#include <waffle_test/waffle_test.h>

#include "wcore_config_attrs.h"
#include "wcore_error.h"

static struct wcore_config_attrs actual_attrs;
static struct wcore_config_attrs expect_attrs;

static const struct wcore_config_attrs default_attrs = {
    // There is no default context api, so arbitrarily choose OpenGL. The
    // remaining context attrs are the defaults for OpenGL.
    .context_api            = WAFFLE_CONTEXT_OPENGL,
    .context_major_version  = 1,
    .context_minor_version  = 0,
    .context_profile        = WAFFLE_CONTEXT_CORE_PROFILE,

    .rgb_size               = 0,
    .rgba_size              = 0,

    .red_size               = WAFFLE_DONT_CARE,
    .green_size             = WAFFLE_DONT_CARE,
    .blue_size              = WAFFLE_DONT_CARE,
    .alpha_size             = WAFFLE_DONT_CARE,

    .depth_size             = WAFFLE_DONT_CARE,
    .stencil_size           = WAFFLE_DONT_CARE,

    .sample_buffers         = 0,
    .samples                = 0,

    .double_buffered        = true,
};

static void
testgroup_wcore_config_attrs_setup(void)
{
    wcore_error_reset();

    // Fill actual_attrs with canaries.
    memset(&actual_attrs, 0x99, sizeof(actual_attrs));

    // Set expect_attrs to defaults.
    memcpy(&expect_attrs, &default_attrs, sizeof(expect_attrs));
}

static void
testgroup_wcore_config_attrs_teardown(void)
{
    // empty
}

TEST(wcore_config_attrs, null_attrib_list)
{
    ASSERT_TRUE(!wcore_config_attrs_parse(NULL, &actual_attrs));
    EXPECT_TRUE(wcore_error_get_code() == WAFFLE_BAD_ATTRIBUTE);
}

TEST(wcore_config_attrs, empty_attrib_list)
{
    const int32_t attrib_list[] = {0};

    ASSERT_TRUE(!wcore_config_attrs_parse(attrib_list, &actual_attrs));
    EXPECT_TRUE(wcore_error_get_code() == WAFFLE_BAD_ATTRIBUTE);
}

TEST(wcore_config_attrs, bad_attr)
{
    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,         WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_PROFILE,     WAFFLE_CONTEXT_OPENGL_ES2,
        0,
    };

    ASSERT_TRUE(!wcore_config_attrs_parse(attrib_list, &actual_attrs));
    EXPECT_TRUE(wcore_error_get_code() == WAFFLE_BAD_ATTRIBUTE);
    EXPECT_TRUE(strstr(wcore_error_get_info()->message, "WAFFLE_CONTEXT_OPENGL_ES2")
                || strstr(wcore_error_get_info()->message, "0x20d"));
}

TEST(wcore_config_attrs, color_buffer_size)
{
    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API, WAFFLE_CONTEXT_OPENGL,

        WAFFLE_SAMPLES,     4, // noise

        WAFFLE_ALPHA_SIZE,  8,
        WAFFLE_RED_SIZE,    5,
        WAFFLE_GREEN_SIZE,  6,
        0,
    };

    expect_attrs.samples = 4;
    expect_attrs.alpha_size = 8;
    expect_attrs.red_size = 5;
    expect_attrs.green_size = 6;

    expect_attrs.rgb_size = 5 + 6;
    expect_attrs.rgba_size = 8 + 5 + 6;

    ASSERT_TRUE(wcore_config_attrs_parse(attrib_list, &actual_attrs));
    ASSERT_TRUE(!memcmp(&actual_attrs, &expect_attrs, sizeof(expect_attrs)));
}

TEST(wcore_config_attrs, double_buffered_is_true)
{
    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,      WAFFLE_CONTEXT_OPENGL,
        WAFFLE_DOUBLE_BUFFERED, 1,
        0,
    };
    expect_attrs.double_buffered = true;

    ASSERT_TRUE(wcore_config_attrs_parse(attrib_list, &actual_attrs));
    ASSERT_TRUE(!memcmp(&actual_attrs, &expect_attrs, sizeof(expect_attrs)));
}

TEST(wcore_config_attrs, double_buffered_is_false)
{
    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_DOUBLE_BUFFERED, 0,
        0,
    };
    expect_attrs.double_buffered = false;

    ASSERT_TRUE(wcore_config_attrs_parse(attrib_list, &actual_attrs));
    ASSERT_TRUE(!memcmp(&actual_attrs, &expect_attrs, sizeof(expect_attrs)));
}

TEST(wcore_config_attrs, double_buffered_is_bad)
{
    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,     WAFFLE_CONTEXT_OPENGL,
        WAFFLE_DOUBLE_BUFFERED, 0x31415926,
        0,
    };

    ASSERT_TRUE(!wcore_config_attrs_parse(attrib_list, &actual_attrs));
    EXPECT_TRUE(wcore_error_get_code() == WAFFLE_BAD_ATTRIBUTE);
    EXPECT_TRUE(strstr(wcore_error_get_info()->message, "WAFFLE_DOUBLE_BUFFERED"));
    EXPECT_TRUE(strstr(wcore_error_get_info()->message, "0x31415926"));
}

TEST(wcore_config_attrs, core_profile_and_accum_buffer)
{
    const int32_t attrib_list[] = {
        WAFFLE_CONTEXT_API,                 WAFFLE_CONTEXT_OPENGL,
        WAFFLE_CONTEXT_MAJOR_VERSION,       3,
        WAFFLE_CONTEXT_MINOR_VERSION,       2,
        WAFFLE_CONTEXT_PROFILE,             WAFFLE_CONTEXT_CORE_PROFILE,

        WAFFLE_ACCUM_BUFFER,                true,

        0,
    };

    ASSERT_TRUE(!wcore_config_attrs_parse(attrib_list, &actual_attrs));
    EXPECT_TRUE(wcore_error_get_code() == WAFFLE_BAD_ATTRIBUTE);
}

void
testsuite_wcore_config_attrs(void)
{
    TEST_RUN(wcore_config_attrs, null_attrib_list);
    TEST_RUN(wcore_config_attrs, empty_attrib_list);
    TEST_RUN(wcore_config_attrs, bad_attr);
    TEST_RUN(wcore_config_attrs, color_buffer_size);
    TEST_RUN(wcore_config_attrs, double_buffered_is_true);
    TEST_RUN(wcore_config_attrs, double_buffered_is_false);
    TEST_RUN(wcore_config_attrs, double_buffered_is_bad);
    TEST_RUN(wcore_config_attrs, core_profile_and_accum_buffer);
}
