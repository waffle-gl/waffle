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

/// @defgroup api_priv api_priv
/// @ingroup waffle_api
///
/// @brief Private bits for implementing Waffle's API.
/// @{

/// @file

#pragma once

#include <stdbool.h>
#include <stddef.h>

struct api_object;
struct wcore_platform;
union native_config;
union native_context;
union native_display;
union native_window;

/// @brief Managed by waffle_init().
extern struct wcore_platform *api_current_platform;

/// @brief Used to validate most API entry points.
///
/// The objects that the user passed into the API entry point are listed in
/// @a obj_list. If its @a length is 0, then the objects are not validated.
///
/// Emit an error and return false if any of the following:
///     - waffle is not initialized
///     - an object pointer is null
///     - an object has an old platform id
bool
api_check_entry(const struct api_object *obj_list[], int length);

/// @brief Create a unique id.
size_t
api_new_object_id(void);

struct api_object {
    size_t platform_id;
    size_t object_id;

    /// @brief Display to which object belongs.
    ///
    /// This is identical to object_id for waffle_display.
    size_t display_id;
};

struct waffle_config {
    struct api_object api;
    union native_config *native;
};

struct waffle_context {
    struct api_object api;
    union native_context *native;
};

struct waffle_display {
    struct api_object api;
    union native_display *native;
};

struct waffle_window {
    struct api_object api;
    union native_window *native;
};

/// Return null if @a config is null.
static inline struct api_object*
waffle_config_get_api_obj(struct waffle_config *config)
{
    return config ? &config->api : NULL;
}

/// Return null if @a ctx is null.
static inline struct api_object*
waffle_context_get_api_obj(struct waffle_context *ctx)
{
    return ctx ? &ctx->api : NULL;
}

/// Return null if @a display is null.
static inline struct api_object*
waffle_display_get_api_obj(struct waffle_display *display)
{
    return display ? &display->api : NULL;
}

/// Return null if @a window is null.
static inline struct api_object*
waffle_window_get_api_obj(struct waffle_window *window)
{
    return window ? &window->api : NULL;
}

/// @}
