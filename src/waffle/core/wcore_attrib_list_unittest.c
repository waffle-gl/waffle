// Copyright 2013 Intel Corporation
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

#include <string.h>

#include "waffle_test/waffle_test.h"

#include "waffle.h"
#include "wcore_attrib_list.h"

TESTGROUP_SIMPLE(wcore_attrib_list_get)

TEST(wcore_attrib_list_get, null)
{
    int32_t *attrib_list = NULL;
    int32_t key = 0;
    int32_t value;

    EXPECT_TRUE(!wcore_attrib_list_get(attrib_list, key, &value));
}

TEST(wcore_attrib_list_get, empty)
{
    int32_t attrib_list[] = { 0 };
    int32_t key = 0;
    int32_t value;

    EXPECT_TRUE(!wcore_attrib_list_get(attrib_list, key, &value));
}

TEST(wcore_attrib_list_get, missing_value)
{
    int32_t attrib_list[] = {
        1, 11,
        0,
    };
    int32_t key = 2;
    int32_t value;

    ASSERT_TRUE(!wcore_attrib_list_get(attrib_list, key, &value));
}

TEST(wcore_attrib_list_get, trailing_items)
{
    int32_t attrib_list[] = {
        1, 11,
        0,
        2, 22,
    };
    int32_t key = 2;
    int32_t value;

    ASSERT_TRUE(!wcore_attrib_list_get(attrib_list, key, &value));
}

TEST(wcore_attrib_list_get, value_not_modified_if_not_found)
{
    int32_t attrib_list[] = {
        1, 11,
        0,
    };
    int32_t value = 17;

    ASSERT_TRUE(!wcore_attrib_list_get(attrib_list, 2, &value));
    ASSERT_TRUE(value == 17);
}

TEST(wcore_attrib_list_get, key_is_first)
{
    int32_t attrib_list[] = {
        1, 11,
        2, 22,
        3, 33,
        0,
    };
    int32_t key = 1;
    int32_t value;

    ASSERT_TRUE(wcore_attrib_list_get(attrib_list, key, &value));
    ASSERT_TRUE(value == 11);
}

TEST(wcore_attrib_list_get, key_is_last)
{
    int32_t attrib_list[] = {
        1, 11,
        2, 22,
        3, 33,
        0,
    };
    int32_t key = 3;
    int32_t value;

    ASSERT_TRUE(wcore_attrib_list_get(attrib_list, key, &value));
    ASSERT_TRUE(value == 33);
}

TESTGROUP_SIMPLE(wcore_attrib_list_length)

TEST(wcore_attrib_list_length, null)
{
    int32_t *attrib_list = NULL;
    ASSERT_TRUE(wcore_attrib_list_length(attrib_list) == 0);
}

TEST(wcore_attrib_list_length, is_0)
{
    int32_t attrib_list[] = {0};
    ASSERT_TRUE(wcore_attrib_list_length(attrib_list) == 0);
}

TEST(wcore_attrib_list_length, is_1)
{
    int32_t attrib_list[] = {
        1, 1,
        0,
    };
    ASSERT_TRUE(wcore_attrib_list_length(attrib_list) == 1);
}

TEST(wcore_attrib_list_length, is_2)
{
    int32_t attrib_list[] = {
        1, 1,
        2, 2,
        0,
    };
    ASSERT_TRUE(wcore_attrib_list_length(attrib_list) == 2);
}

TEST(wcore_attrib_list_length, is_37)
{
    int32_t attrib_list[75];
    memset(attrib_list, 0xff, 74 * sizeof(int32_t));
    attrib_list[74] = 0;

    ASSERT_TRUE(wcore_attrib_list_length(attrib_list) == 37);
}

TEST(wcore_attrib_list_length, trailing_items)
{
    int32_t attrib_list[] = {
        1, 1,
        2, 2,
        3, 3,
        0,
        4, 4,
        5, 5,
        0,
    };

    ASSERT_TRUE(wcore_attrib_list_length(attrib_list) == 3);
}

TESTGROUP_SIMPLE(wcore_attrib_list_update)

TEST(wcore_attrib_list_update, null)
{
    int32_t *attrib_list = NULL;
    ASSERT_TRUE(!wcore_attrib_list_update(attrib_list, 7, 7));
}

TEST(wcore_attrib_list_update, empty_list)
{
    int32_t attrib_list[] = {0};
    ASSERT_TRUE(!wcore_attrib_list_update(attrib_list, 7, 7));
}

TEST(wcore_attrib_list_update, at_0)
{
    int32_t v;
    int32_t attrib_list[] = {
        10, 10,
        20, 20,
        30, 30,
        0,
    };

    ASSERT_TRUE(wcore_attrib_list_update(attrib_list, 10, 99));
    ASSERT_TRUE(wcore_attrib_list_get(attrib_list, 10, &v));
    ASSERT_TRUE(v == 99);
}

TEST(wcore_attrib_list_update, at_1)
{
    int32_t v;
    int32_t attrib_list[] = {
        10, 10,
        20, 20,
        30, 30,
        0,
    };

    ASSERT_TRUE(wcore_attrib_list_update(attrib_list, 20, 99));
    ASSERT_TRUE(wcore_attrib_list_get(attrib_list, 20, &v));
    ASSERT_TRUE(v == 99);
}

TEST(wcore_attrib_list_update, missing_key)
{
    int32_t attrib_list[] = {
        10, 10,
        20, 20,
        30, 30,
        0,
    };

    ASSERT_TRUE(!wcore_attrib_list_update(attrib_list, 50, 99));
}

static void
testsuite_wcore_attrib_list(void)
{
    TEST_RUN(wcore_attrib_list_get, null);
    TEST_RUN(wcore_attrib_list_get, empty);
    TEST_RUN(wcore_attrib_list_get, missing_value);
    TEST_RUN(wcore_attrib_list_get, trailing_items);
    TEST_RUN(wcore_attrib_list_get, value_not_modified_if_not_found);
    TEST_RUN(wcore_attrib_list_get, key_is_first);
    TEST_RUN(wcore_attrib_list_get, key_is_last);
    TEST_RUN(wcore_attrib_list_length, null);
    TEST_RUN(wcore_attrib_list_length, is_0);
    TEST_RUN(wcore_attrib_list_length, is_1);
    TEST_RUN(wcore_attrib_list_length, is_2);
    TEST_RUN(wcore_attrib_list_length, is_37);
    TEST_RUN(wcore_attrib_list_length, trailing_items);
    TEST_RUN(wcore_attrib_list_update, null);
    TEST_RUN(wcore_attrib_list_update, empty_list);
    TEST_RUN(wcore_attrib_list_update, at_0);
    TEST_RUN(wcore_attrib_list_update, at_1);
    TEST_RUN(wcore_attrib_list_update, missing_key);
}

int
main(int argc, char *argv[])
{
    void (*test_runners[])(void) = {
        testsuite_wcore_attrib_list,
        0,
    };

    return wt_main(&argc, argv, test_runners);
}
