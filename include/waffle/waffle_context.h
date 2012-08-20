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

/// @defgroup waffle_context waffle_context
/// @ingroup waffle_api
/// @{

/// @file

#pragma once

#include <stdbool.h>

#include "waffle_portability.h"

#ifdef __cplusplus
extern "C" {
#endif

struct waffle_config;
struct waffle_context;

struct waffle_glx_context;
struct waffle_x11_egl_context;
struct waffle_wayland_context;

union waffle_native_context {
    struct waffle_glx_context *glx;
    struct waffle_x11_egl_context *x11_egl;
    struct waffle_wayland_context *wayland;
};

WAFFLE_API struct waffle_context*
waffle_context_create(
        struct waffle_config *config,
        struct waffle_context *shared_ctx);

WAFFLE_API bool
waffle_context_destroy(struct waffle_context *self);

/// @brief Get underlying native objects.
///
/// Use free() to deallocate the returned pointer.
WAFFLE_API union waffle_native_context*
waffle_context_get_native(struct waffle_context *self);

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
