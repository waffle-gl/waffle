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

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/// @brief Convert int32_t attribute list to an intptr_t attribute list.
intptr_t*
wcore_attrib_list_from_int32(const int32_t attrib_list[]);

size_t
wcore_attrib_list_length(const intptr_t attrib_list[]);

intptr_t*
wcore_attrib_list_copy(const intptr_t attrib_list[]);

bool
wcore_attrib_list_get(
        const intptr_t *attrib_list,
        intptr_t key,
        intptr_t *value);

bool
wcore_attrib_list_get_with_default(
        const intptr_t attrib_list[],
        intptr_t key,
        intptr_t *value,
        intptr_t default_value);

bool
wcore_attrib_list_update(
        intptr_t *attrib_list,
        intptr_t key,
        intptr_t value);

size_t
wcore_attrib_list32_length(const int32_t attrib_list[]);

bool
wcore_attrib_list32_get(
        const int32_t *attrib_list,
        int32_t key,
        int32_t *value);

bool
wcore_attrib_list32_get_with_default(
        const int32_t attrib_list[],
        int32_t key,
        int32_t *value,
        int32_t default_value);

bool
wcore_attrib_list32_update(
        int32_t *attrib_list,
        int32_t key,
        int32_t value);
