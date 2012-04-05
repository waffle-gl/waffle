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

#include "api_priv.h"

#include <waffle/core/wcore_error.h>
#include <waffle/core/wcore_platform.h>

struct wcore_platform *api_current_platform = 0;

bool
api_check_entry(const struct api_object *obj_list[], int length)
{
    int i;

    if (!api_current_platform) {
        wcore_error(WAFFLE_NOT_INITIALIZED);
        return false;
    }

    for (i = 0; i < length; ++i) {
        if (obj_list[i] == NULL) {
            wcore_errorf(WAFFLE_BAD_PARAMETER, "null pointer");
            return false;
        }

        if (obj_list[i]->platform_id != api_current_platform->id) {
            wcore_error(WAFFLE_OLD_OBJECT);
            return false;
        }

        if (obj_list[i]->display_id != obj_list[0]->display_id) {
            wcore_error(WAFFLE_BAD_DISPLAY_MATCH);
        }
    }

    return true;
}

size_t
api_new_object_id(void)
{
    static size_t counter = 1;

    if (counter == 0) {
        fprintf(stderr, "waffle: error: internal counter wrapped to 0\n");
        abort();
    }

    return counter++;
}