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

#define _XOPEN_SOURCE 500 // for strdup()

#include "wt_runner.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../waffle_test.h"
#include "wt_test.h"

struct wt_testname {
    const char *group;
    const char *name;
};

struct wt_runner {
    bool is_init;

    int num_pass;
    int num_fail;
    int num_skip;

    struct wt_testname *fail_names;
    int fail_names_length;
};

/// @brief Singleton test runner.
static struct wt_runner self;

static void
wt_runner_check_preconditions(void)
{
    if (!self.is_init) {
        printf("wt: error: wt_run_test*() was called outside of wt_main()\n");
        abort();
    }
}

static void
wt_runner_accum_result(int result)
{
    switch (result) {
        case WT_RESULT_PASS:   ++self.num_pass;    break;
        case WT_RESULT_FAIL:   ++self.num_fail;    break;
        case WT_RESULT_SKIP:   ++self.num_skip;    break;

        default:
            printf("wt: internal error: %s:%d: result has bad value 0x%x\n",
                    __FILE__, __LINE__, result);
            abort();
    }
}

static void
wt_runner_double_failed_test_names(void)
{
    int size = 2 * self.fail_names_length * sizeof(*self.fail_names);
    self.fail_names_length *= 2;
    self.fail_names = realloc(self.fail_names, size);

    if (!self.fail_names) {
        printf("wt: error: out of memory\n");
        abort();
    }
}

static void
wt_runner_save_failed_test_name(const char *group, const char *name)
{
    if (self.num_fail == self.fail_names_length)
        wt_runner_double_failed_test_names();

    self.fail_names[self.num_fail].group = strdup(group);
    self.fail_names[self.num_fail].name = strdup(name);
}

static void
wt_runner_finish_test(const char *group, const char *name, int result)
{
    if (result == WT_RESULT_FAIL)
        wt_runner_save_failed_test_name(group, name);

    wt_runner_accum_result(result);
}

void
wt_runner_init(void)
{
    memset(&self, 0, sizeof(self));
    self.is_init = true;
    self.fail_names_length = 64;
    self.fail_names = malloc(self.fail_names_length * sizeof(*self.fail_names));
}

void
wt_runner_finish(void)
{
    free(self.fail_names);
    self.is_init = false;
}

void
wt_runner_run_test(
        const char *group,
        const char *name,
        void (*test)(void),
        void (*setup)(void),
        void (*teardown)(void))
{
    int result;

    wt_runner_check_preconditions();
    wt_test_run(group, name, test, setup, teardown, &result);
    wt_runner_finish_test(group, name, result);
}

void
wt_runner_get_totals(int *pass, int *fail, int *ignore)
{
    if (pass)
        *pass = self.num_pass;
    if (fail)
        *fail = self.num_fail;
    if (ignore)
        *ignore = self.num_skip;
}

void
wt_runner_print_summary(void)
{
    int i;

    if (self.num_fail > 0) {
        printf("test: failed tests:\n");
        for (i = 0; i < self.num_fail; ++i) {
            struct wt_testname *name = &self.fail_names[i];
            printf("  %s.%s\n", name->group, name->name);
        }
    }

    printf("test: summary: failed %d, skipped %d, passed %d\n",
           self.num_fail, self.num_skip, self.num_pass);
}
