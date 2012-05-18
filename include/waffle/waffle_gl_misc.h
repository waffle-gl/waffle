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

#include "waffle_portable.h"

#ifdef __cplusplus
extern "C" {
#endif

struct waffle_context;
struct waffle_display;
struct waffle_window;

/// @brief Check if an extension is contained in a GL-style extension string.
///
/// The @a extension_string must be in the format returned by
/// `glGetString(GL_EXTENSIONS)`. Note that this function is not restricted to
/// the GL extension string; it can also be used on the GLX and EGL extension
/// strings.
///
/// This function may be called before waffle_init().
WAFFLE_API bool
waffle_is_extension_in_string(
        const char *restrict extension_string,
        const char *restrict extension_name);

WAFFLE_API bool
waffle_make_current(
        struct waffle_display *dpy,
        struct waffle_window *window,
        struct waffle_context *ctx);

WAFFLE_API void*
waffle_get_proc_address(const char *name);

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
