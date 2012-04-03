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

#pragma once

#include "priv/wt_test.h"
#include "priv/wt_runner.h"

/// @defgroup Defining Tests
/// @{

#define TEST(group, name) \
    static void test_##group##_##name(void)

#define TESTGROUP(group, setup, teardown) \
    static void testgroup_##group##_setup(void)    { setup();    } \
    static void testgroup_##group##_teardown(void) { teardown(); }

#define TESTGROUP_SIMPLE(group) \
    static void testgroup_##group##_setup(void) {} \
    static void testgroup_##group##_teardown(void) {}

/// @}
/// @defgroup Running Tests
/// @{

/// This must be called only from test suite s passed to wt_main().
#define TEST_RUN(group, name) \
    wt_runner_run_test( \
           #group, #name, \
           test_##group##_##name, \
           testgroup_##group##_setup, \
           testgroup_##group##_teardown)

/// @param test_runners is a list of functions that call TEST_RUN(). The list
///     is a null-terminated.
/// @return number of failed tests.
int wt_main(int *argc, char **argv, void (*test_suites[])(void));

/// @}
/// @defgroup Test Commands
/// @{

#define TEST_PASS()                           wt_test_pass()
#define TEST_IGNORE()                         wt_test_ignore()
#define TEST_FAIL()                           wt_test_fail( __FILE__, __LINE__)
#define TEST_FAIL_PRINTF(format, ...)         wt_test_fail_printf( __FILE__, __LINE__, format, __VA_ARGS__)

#define TEST_ERROR()                          wt_test_error(__FILE__, __LINE__)
#define TEST_ERROR_PRINTF(format, ...)        wt_test_error_printf(__FILE__, __LINE__, format, __VA_ARGS__)

#define EXPECT_TRUE(cond)                     wt_test_expect( __FILE__, __LINE__, (cond), #cond)
#define ASSERT_TRUE(cond)                     wt_test_assert( __FILE__, __LINE__, (cond), #cond)

/// @}
