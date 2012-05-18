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

/// @defgroup waffle_window waffle_window
/// @ingroup waffle_api
/// @{

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "waffle_portable.h"

#ifdef __cplusplus
extern "C" {
#endif

struct waffle_config;
struct waffle_window;

/// If the platform allows, the window is not displayed onto the screen
/// after creation. To display the window, call waffle_window_show().
WAFFLE_API struct waffle_window*
waffle_window_create(
        struct waffle_config *config,
        int32_t width,
        int32_t height);

WAFFLE_API bool
waffle_window_destroy(struct waffle_window *self);

/// @brief Show the window on the screen.
///
/// If the window is already shown, this does nothing.
WAFFLE_API bool
waffle_window_show(struct waffle_window *self);

WAFFLE_API bool
waffle_window_swap_buffers(struct waffle_window *self);

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
