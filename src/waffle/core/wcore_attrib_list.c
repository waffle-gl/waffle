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

#include "wcore_attrib_list.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

int32_t
wcore_attrib_list32_length(const int32_t attrib_list[])
{
    const int32_t *i = attrib_list;

    if (attrib_list == NULL)
        return 0;

    while (*i != 0)
        i += 2;

    return (int32_t) (i - attrib_list) / 2;
}

bool
wcore_attrib_list32_get(
        const int32_t *attrib_list,
        int32_t key,
        int32_t *value)
{
    if (attrib_list == NULL)
        return false;

    for (int i = 0; attrib_list[i] != 0; i += 2) {
        if (attrib_list[i] != key)
            continue;

        *value = attrib_list[i + 1];
        return true;
    }

    return false;
}

bool
wcore_attrib_list32_get_with_default(
        const int32_t attrib_list[],
        int32_t key,
        int32_t *value,
        int32_t default_value)
{
    if (wcore_attrib_list32_get(attrib_list, key, value)) {
        return true;
    }
    else {
        *value = default_value;
        return false;
    }
}

bool
wcore_attrib_list32_update(
        int32_t *attrib_list,
        int32_t key,
        int32_t value)
{
    int32_t *i = attrib_list;

    if (attrib_list == NULL)
        return false;

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
