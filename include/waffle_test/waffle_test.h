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

#pragma once

#include "priv/wt_test.h"
#include "priv/wt_runner.h"

#define TEST(group, name) \
    static void test_##group##_##name(void)

#define TESTGROUP(group, setup, teardown) \
    static void testgroup_##group##_setup(void)    { setup();    } \
    static void testgroup_##group##_teardown(void) { teardown(); }

#define TESTGROUP_SIMPLE(group) \
    static void testgroup_##group##_setup(void) {} \
    static void testgroup_##group##_teardown(void) {}

/// This must be called only from test suite s passed to wt_main().
#define TEST_RUN(group, name) \
    wt_runner_run_test( \
           #group, #name, \
           test_##group##_##name, \
           testgroup_##group##_setup, \
           testgroup_##group##_teardown)

/// This must be called only from test suite s passed to wt_main().
#define TEST_RUN2(group, displayname, testname) \
    wt_runner_run_test( \
           #group, #displayname, \
           test_##group##_##testname, \
           testgroup_##group##_setup, \
           testgroup_##group##_teardown)

/// @param test_runners is a list of functions that call TEST_RUN(). The list
///     is a null-terminated.
/// @return number of failed tests.
#ifdef _WIN32
int wt_main(int *argc, char **argv, void (__stdcall *test_suites[])(void));
#else
int wt_main(int *argc, char **argv, void (*test_suites[])(void));
#endif // _WIN32

#define TEST_PASS()                           wt_test_pass()
#define TEST_SKIP()                           wt_test_skip()
#define TEST_FAIL()                           wt_test_fail( __FILE__, __LINE__)
#define TEST_FAIL_PRINTF(format, ...)         wt_test_fail_printf( __FILE__, __LINE__, format, __VA_ARGS__)

#define TEST_ERROR()                          wt_test_error(__FILE__, __LINE__)
#define TEST_ERROR_PRINTF(format, ...)        wt_test_error_printf(__FILE__, __LINE__, format, __VA_ARGS__)

#define EXPECT_TRUE(cond)                     wt_test_expect( __FILE__, __LINE__, (cond), #cond)
#define ASSERT_TRUE(cond)                     wt_test_assert( __FILE__, __LINE__, (cond), #cond)
