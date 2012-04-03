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

void
wt_runner_init(void);

void
wt_runner_finish(void);

void
wt_runner_run_test(
        const char *group,
        const char *name,
        void (*test)(void),
        void (*setup)(void),
        void (*teardown)(void));

void
wt_runner_print_summary(void);

void
wt_runner_get_totals(int *pass, int *fail, int *ignore);