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
#include <stdint.h>

#include "waffle_visibility.h"

#ifdef __cplusplus
extern "C" {
#endif

WAFFLE_API int32_t
waffle_attrib_list_length(const int32_t attrib_list[]);

/// If the @a key is not found, then @a value is not modified.
///
/// @return true if key was found.
WAFFLE_API bool
waffle_attrib_list_get(
        const int32_t attrib_list[],
        int32_t key,
        int32_t *value);

/// If the @a key is not found, the @a value is set to default.
///
/// @return true if @a key was found.
WAFFLE_API bool
waffle_attrib_list_get_with_default(
        const int32_t attrib_list[],
        int32_t key,
        int32_t *value,
        int32_t default_value);

/// @return false if @a key is not found.
WAFFLE_API bool
waffle_attrib_list_update(
        int32_t *attrib_list,
        int32_t key,
        int32_t value);

#ifdef __cplusplus
} // end extern "C"
#endif
