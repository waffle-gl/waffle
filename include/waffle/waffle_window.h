// Copyright 2012 Intel Corporation
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

/// @defgroup waffle_window waffle_window
/// @ingroup waffle_api
/// @{

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "waffle_portability.h"

#ifdef __cplusplus
extern "C" {
#endif

struct waffle_config;
struct waffle_window;

struct waffle_glx_window;
struct waffle_x11_egl_window;
struct waffle_wayland_window;

union waffle_native_window {
    struct waffle_glx_window *glx;
    struct waffle_x11_egl_window *x11_egl;
    struct waffle_wayland_window *wayland;
};

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

/// @brief Get underlying native objects.
///
/// Use free() to deallocate the returned pointer.
WAFFLE_API union waffle_native_window*
waffle_window_get_native(struct waffle_window *self);

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
