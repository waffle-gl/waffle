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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <waffle/core/wcore_error.h>
#include <waffle_test/waffle_test.h>

TESTGROUP_SIMPLE(wcore_error)

TEST(wcore_error, code_unknown_error)
{
    wcore_error(WAFFLE_UNKNOWN_ERROR);
    EXPECT_TRUE(wcore_error_get_code() == WAFFLE_UNKNOWN_ERROR);
    EXPECT_TRUE(!strcmp(wcore_error_get_message(), ""));

}

TEST(wcore_error, code_bad_attribute)
{
    wcore_error(WAFFLE_BAD_ATTRIBUTE);
    EXPECT_TRUE(wcore_error_get_code() == WAFFLE_BAD_ATTRIBUTE);
    EXPECT_TRUE(!strcmp(wcore_error_get_message(), ""));

}

TEST(wcore_error, with_message)
{
    wcore_errorf(WAFFLE_BAD_PARAMETER, "bad %s (0x%x)", "gl_api", 0x17);
    EXPECT_TRUE(wcore_error_get_code() == WAFFLE_BAD_PARAMETER);
    EXPECT_TRUE(!strcmp(wcore_error_get_message(), "bad gl_api (0x17)"));
}

TEST(wcore_error, internal_error)
{
    char error_location[1024];
    snprintf(error_location, 1024, "%s:%d:", __FILE__, __LINE__ + 2);

    wcore_error_internal("%s zoroaster %d", "hello", 5);
    EXPECT_TRUE(wcore_error_get_code() == WAFFLE_INTERNAL_ERROR);
    EXPECT_TRUE(strstr(wcore_error_get_message(), "hello zoroaster 5"));
    EXPECT_TRUE(strstr(wcore_error_get_message(), error_location));
}

TEST(wcore_error, last_call_without_message_wins)
{
    wcore_errorf(WAFFLE_UNKNOWN_ERROR, "cookies");
    wcore_error(WAFFLE_BAD_ATTRIBUTE);
    EXPECT_TRUE(wcore_error_get_code() == WAFFLE_BAD_ATTRIBUTE);
    EXPECT_TRUE(!strcmp(wcore_error_get_message(), ""));
}

TEST(wcore_error, last_call_with_message_wins)
{
    wcore_errorf(WAFFLE_UNKNOWN_ERROR, "cookies");
    wcore_errorf(WAFFLE_NO_ERROR, "all is well");
    EXPECT_TRUE(wcore_error_get_code() == WAFFLE_NO_ERROR);
    EXPECT_TRUE(!strcmp(wcore_error_get_message(), "all is well"));
}

TEST(wcore_error, disable_then_error)
{
    wcore_error(WAFFLE_NOT_INITIALIZED);
    WCORE_ERROR_DISABLED(
        wcore_error(WAFFLE_BAD_ATTRIBUTE);
    );
    EXPECT_TRUE(wcore_error_get_code() == WAFFLE_NOT_INITIALIZED);
}

TEST(wcore_error, disable_then_errorf)
{
    wcore_error(WAFFLE_NOT_INITIALIZED);
    WCORE_ERROR_DISABLED(
        wcore_errorf(WAFFLE_BAD_ATTRIBUTE, "i'm not here");
    );
    EXPECT_TRUE(wcore_error_get_code() == WAFFLE_NOT_INITIALIZED);
}

TEST(wcore_error, disable_then_error_internal)
{
    wcore_error(WAFFLE_NOT_INITIALIZED);
    WCORE_ERROR_DISABLED({
        // Compilation fails with gcc when wcore_error_internal() appears
        // here. So directly the macro's implementing function.
        _wcore_error_internal(__FILE__, __LINE__, "this isn't happening");
    });
    EXPECT_TRUE(wcore_error_get_code() == WAFFLE_NOT_INITIALIZED);
}

void
testsuite_wcore_error(void)
{
    TEST_RUN(wcore_error, code_unknown_error);
    TEST_RUN(wcore_error, code_bad_attribute);
    TEST_RUN(wcore_error, code_unknown_error);
    TEST_RUN(wcore_error, with_message);
    TEST_RUN(wcore_error, internal_error);
    TEST_RUN(wcore_error, last_call_without_message_wins);
    TEST_RUN(wcore_error, last_call_with_message_wins);
    TEST_RUN(wcore_error, disable_then_error);
    TEST_RUN(wcore_error, disable_then_errorf);
    TEST_RUN(wcore_error, disable_then_error_internal);
}