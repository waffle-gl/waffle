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

#include "wt_test.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct wt_test {
    const char *group;
    const char *name;

    /// This is initialized to TEST_PASS at the start of each test.
    int result;

    /// @brief Allows wt_fail() and friends to terminate the test.
    jmp_buf jump;

    /// @brief A test is executing.
    bool in_test;
};

/// @brief The test singleton.
struct wt_test self;

static void
wt_test_print_result(void)
{
    const char *tag;

    switch (self.result) {
        default:
        case WT_RESULT_FAIL:       tag = "fail:";      break;
        case WT_RESULT_PASS:       tag = "pass:";      break;
        case WT_RESULT_SKIP:       tag = "skip:";    break;
    }

    printf("test: %-7s %s.%s\n", tag, self.group, self.name);
}

static void
wt_test_run_check_preconditions(void)
{
    if (self.in_test) {
        printf("test: error: attempted to run a new test while one was "
               "already running\n");
        abort();
    }
}

void
wt_test_run(
        const char *group, const char *name,
        void (*test)(void),
        void (*fixture_setup)(void),
        void (*fixture_teardown)(void),
        int *result)
{
    wt_test_run_check_preconditions();

    self.group = group;
    self.name = name;
    self.result = WT_RESULT_PASS;
    self.in_test = true;

    if (setjmp(self.jump) == 0) {
        fixture_setup();
        test();
        fixture_teardown();
    }

    wt_test_print_result();
    *result = self.result;
    self.in_test = false;
}

static void
wt_test_check_command_preconditions(void)
{
    if (!self.in_test) {
        printf("test: error: a test command was called (such as ASSERT_TRUE), "
               "but no test is running\n");
        abort();
    }
}

static void
wt_test_terminate_early(int result)
{
    self.result = result;
    longjmp(self.jump, 1);
}

static void
wt_test_error_vprintf(
        const char *file, int line,
        const char *format, va_list ap)
{
    self.result = WT_RESULT_FAIL;

    // Use the same error format as GCC.
    printf("%s:%d: error in test %s.%s", file, line, self.group, self.name);
    if (format) {
        printf(": ");
        vprintf(format, ap);
    }
    printf("\n");
}

void
wt_test_pass(void)
{
    wt_test_check_command_preconditions();
    wt_test_terminate_early(WT_RESULT_PASS);
}

void
wt_test_skip(void)
{
    wt_test_check_command_preconditions();
    wt_test_terminate_early(WT_RESULT_SKIP);
}

void
wt_test_fail(const char *file, int line)
{
    wt_test_check_command_preconditions();
    wt_test_error_printf(file, line, "forced failure");
    wt_test_terminate_early(WT_RESULT_FAIL);
}

void
wt_test_fail_printf(const char *file, int line, const char *format, ...)
{
    va_list ap;

    wt_test_check_command_preconditions();

    va_start(ap, format);
    wt_test_error_vprintf(file, line, format, ap);
    va_end(ap);

    wt_test_terminate_early(WT_RESULT_FAIL);
}

void
wt_test_error(const char *file, int line)
{
    wt_test_error_printf(file, line, NULL);
}

void
wt_test_error_printf(const char *file, int line, const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    wt_test_error_vprintf(file, line, format, ap);
    va_end(ap);
}

void
wt_test_expect(
        const char *file, int line,
        bool cond, const char *cond_str)
{
    if (cond)
        return;

    wt_test_error_printf(file, line, "expect(%s)", cond_str);
}

void
wt_test_assert(
        const char *file, int line,
        bool cond, const char *cond_str)
{
    if (cond)
        return;

    wt_test_error_printf(file, line, "assert(%s)", cond_str);
    wt_test_terminate_early(WT_RESULT_FAIL);
}
