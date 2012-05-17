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

#include <stdlib.h>
#include <string.h>

#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_config_attrs.h>
#include <waffle/core/wcore_error.h>
#include <waffle_test/waffle_test.h>

static struct wcore_config_attrs actual_attrs;
static struct wcore_config_attrs expect_attrs;

static const struct wcore_config_attrs default_attrs = {
    // There is no default context api, so arbitrarily choose OpenGL. The
    // remaining context attrs are the defaults for OpenGL.
    .context_api            = WAFFLE_CONTEXT_OPENGL,
    .context_major_version  = 1,
    .context_minor_version  = 0,
    .context_profile        = WAFFLE_CONTEXT_CORE_PROFILE,

    .color_buffer_size      = 0,
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
        WAFFLE_CONTEXT_API, WAFFLE_CONTEXT_OPENGL,
        WAFFLE_OPENGL_API,  WAFFLE_OPENGL_ES2,
        0,
    };

    ASSERT_TRUE(!wcore_config_attrs_parse(attrib_list, &actual_attrs));
    EXPECT_TRUE(wcore_error_get_code() == WAFFLE_BAD_ATTRIBUTE);
    EXPECT_TRUE(strstr(wcore_error_get_message(), "WAFFLE_OPENGL_API")
                || strstr(wcore_error_get_message(), "0x10"));
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
    expect_attrs.color_buffer_size = 8 + 5 + 6;

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
    EXPECT_TRUE(strstr(wcore_error_get_message(), "WAFFLE_DOUBLE_BUFFERED"));
    EXPECT_TRUE(strstr(wcore_error_get_message(), "0x31415926"));
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
}
