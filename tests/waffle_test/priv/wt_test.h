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

#include <stdbool.h>

enum wt_result {
    WT_RESULT_PASS,
    WT_RESULT_FAIL,
    WT_RESULT_SKIP,
};

/// This must be called only from wt_main().
void
wt_test_run(
        const char *group, const char *name,
        void (*test)(void),
        void (*fixture_setup)(void),
        void (*fixture_teardown)(void),
        int *result);

void wt_test_pass(void);
void wt_test_skip(void);
void wt_test_fail(const char *file, int line);
void wt_test_fail_printf(const char *file, int line, const char *format, ...);
void wt_test_error(const char *filename, int line);
void wt_test_error_printf(const char *file, int line, const char *format, ...);
void wt_test_expect(const char *file, int line, bool cond, const char *cond_str);
void wt_test_assert(const char *file, int line, bool cond, const char *cond_str);
