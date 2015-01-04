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
#include <string.h>

#include "wcore_error.h"
#include "wcore_util.h"

#define WCORE_ATTRIB_LIST_COMMON_FUNCS(T,                               \
                                       length_func,                     \
                                       get_func,                        \
                                       get_with_default_func,           \
                                       update_func)                     \
                                                                        \
    size_t                                                              \
    length_func(const T attrib_list[])                                  \
    {                                                                   \
        const T *i = attrib_list;                                       \
                                                                        \
        if (!attrib_list) {                                             \
            return 0;                                                   \
        }                                                               \
                                                                        \
        while (*i) {                                                    \
            i += 2;                                                     \
        }                                                               \
                                                                        \
        return (i - attrib_list) / 2;                                   \
    }                                                                   \
                                                                        \
    bool                                                                \
    get_func(const T *attrib_list,                                      \
             T key,                                                     \
             T *value)                                                  \
    {                                                                   \
        if (!attrib_list) {                                             \
            return false;                                               \
        }                                                               \
                                                                        \
        for (size_t i = 0; attrib_list[i] != 0; i += 2) {               \
            if (attrib_list[i] == key) {                                \
                *value = attrib_list[i + 1];                            \
                return true;                                            \
            }                                                           \
        }                                                               \
                                                                        \
        return false;                                                   \
    }                                                                   \
                                                                        \
    bool                                                                \
    get_with_default_func(                                              \
            const T attrib_list[],                                      \
            T key,                                                      \
            T *value,                                                   \
            T default_value)                                            \
    {                                                                   \
        if (get_func(attrib_list, key, value)) {                        \
            return true;                                                \
        } else {                                                        \
            *value = default_value;                                     \
            return false;                                               \
        }                                                               \
    }                                                                   \
                                                                        \
    bool                                                                \
    update_func(T *attrib_list,                                         \
                T key,                                                  \
                T value)                                                \
    {                                                                   \
        T *i = attrib_list;                                             \
                                                                        \
        if (attrib_list == NULL) {                                      \
            return false;                                               \
        }                                                               \
                                                                        \
        while (*i != 0 && *i != key) {                                  \
            i += 2;                                                     \
        }                                                               \
                                                                        \
        if (*i == key) {                                                \
            i[1] = value;                                               \
            return true;                                                \
        } else {                                                        \
            return false;                                               \
        }                                                               \
    }

WCORE_ATTRIB_LIST_COMMON_FUNCS(int32_t,
                               wcore_attrib_list32_length,
                               wcore_attrib_list32_get,
                               wcore_attrib_list32_get_with_default,
                               wcore_attrib_list32_update)

WCORE_ATTRIB_LIST_COMMON_FUNCS(intptr_t,
                               wcore_attrib_list_length,
                               wcore_attrib_list_get,
                               wcore_attrib_list_get_with_default,
                               wcore_attrib_list_update)

/// Given length of attribute list, calculate its size in bytes. Return false
/// on arithemtic overflow.
static bool
wcore_attrib_list_get_size(size_t *size, size_t len) {
    bool ok;

    ok = wcore_mul_size(size, 2, len);
    ok &= wcore_iadd_size(size, 1);
    ok &= wcore_imul_size(size, sizeof(intptr_t));

    return ok;
}

intptr_t*
wcore_attrib_list_from_int32(const int32_t attrib_list32[])
{
    size_t len = 0;
    size_t size = 0;
    intptr_t *attrib_list = NULL;

    len = wcore_attrib_list32_length(attrib_list32);

    if (!wcore_attrib_list_get_size(&size, len)) {
        // Arithmetic overflow occurred, therefore we can't allocate the
        // memory.
        wcore_error(WAFFLE_ERROR_BAD_ALLOC);
        return NULL;
    }

    attrib_list = wcore_malloc(size);
    if (!attrib_list) {
        return NULL;
    }

    // Copy all key/value pairs.
    for (size_t i = 0; i < 2 * len; ++i) {
        attrib_list[i] = attrib_list32[i];
    }

    // Add terminal null.
    attrib_list[2 * len] = 0;

    return attrib_list;
}

intptr_t*
wcore_attrib_list_copy(const intptr_t attrib_list[])
{
    intptr_t *copy = NULL;

    if (attrib_list) {
        size_t len;
        size_t size = 0;

        len = wcore_attrib_list_length(attrib_list);

        if (!wcore_attrib_list_get_size(&size, len)) {
            // Arithmetic overflow occurred, therefore we can't allocate the
            // memory.
            wcore_error(WAFFLE_ERROR_BAD_ALLOC);
            return NULL;
        }

        copy = wcore_malloc(size);
        if (!copy) {
            return NULL;
        }

        memcpy(copy, attrib_list, size);
    } else {
        copy = wcore_malloc(sizeof(intptr_t));
        if (!copy) {
            return NULL;
        }

        copy[0] = 0;
    }

    return copy;
}
