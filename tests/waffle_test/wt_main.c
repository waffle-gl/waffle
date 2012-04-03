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

#include "waffle_test.h"
#include "priv/wt_runner.h"

int
wt_main(int *argc, char **argv, void (*test_suites[])(void))
{
    int i;
    int num_fail;

    wt_runner_init();

    for (i = 0; test_suites[i] != 0; ++i)
        test_suites[i]();

    wt_runner_print_summary();
    wt_runner_get_totals(0, &num_fail, 0);
    wt_runner_finish();

    return num_fail;
}