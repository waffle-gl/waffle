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

/// @addtogroup waffle_attrib_list
/// @{

/// @file
/// @ingroup api

#include <waffle/waffle_attrib_list.h>

int32_t
waffle_attrib_list_length(const int32_t attrib_list[])
{
    const int32_t *i = attrib_list;

    while (*i != 0)
        i += 2;

    return (i - attrib_list) / 2;
}

bool
waffle_attrib_list_get(
        const int32_t *attrib_list,
        int32_t key,
        int32_t *value)
{
    int i;

    for (i = 0; attrib_list[i] != 0; i += 2) {
        if (attrib_list[i] != key)
            continue;

        *value = attrib_list[i + 1];
        return true;
    }

    return false;
}

bool
waffle_attrib_list_get_with_default(
        const int32_t attrib_list[],
        int32_t key,
        int32_t *value,
        int32_t default_value)
{
    if (waffle_attrib_list_get(attrib_list, key, value)) {
        return true;
    }
    else {
        *value = default_value;
        return false;
    }
}

bool
waffle_attrib_list_update(
        int32_t *attrib_list,
        int32_t key,
        int32_t value)
{
    int32_t *i = attrib_list;

    while (*i != 0 && *i != key)
        i += 2;

    if (*i == key) {
        i[1] = value;
        return true;
    }
    else {
        return false;
    }
}

/// @}
