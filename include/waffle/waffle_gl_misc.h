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

/// @defgroup waffle_gl_misc waffle_gl_misc
/// @ingroup waffle_api
/// @{

/// @file

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "waffle_visibility.h"

#ifdef __cplusplus
extern "C" {
#endif

struct waffle_context;
struct waffle_display;
struct waffle_window;

WAFFLE_API bool
waffle_make_current(
        struct waffle_display *dpy,
        struct waffle_window *window,
        struct waffle_context *ctx);

WAFFLE_API void*
waffle_get_proc_address(const char *name);

/// @brief Get symbol from GL library.
///
/// The library is selected with @a dl, which must be one of
///     - WAFFLE_DL_OPENGL
///     - WAFFLE_DL_OPENGL_ES1
///     - WAFFLE_DL_OPENGL_ES2
WAFFLE_API void*
waffle_dlsym_gl(int32_t dl, const char *name);

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
