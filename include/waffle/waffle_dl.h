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

/// @defgroup waffle_dl waffle_dl
/// @ingroup waffle_api
///
/// @brief Dynamic libraries.
///
/// Use these functions to query dynamic libraries. For each function, @a dl
/// must be one of:
/// - WAFFLE_DL_OPENGL
/// - WAFFLE_DL_OPENGL_ES1
/// - WAFFLE_DL_OPENGL_ES2
///
/// @{

/// @file

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "waffle_visibility.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Test if a dynamic library can be opened.
WAFFLE_API bool
waffle_dl_can_open(int32_t dl);

/// @brief Get symbol from dynamic library.
WAFFLE_API void*
waffle_dl_sym(int32_t dl, const char *name);

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
