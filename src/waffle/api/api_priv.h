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

#define API_TYPE(name) \
    union native_##name;                                                    \
                                                                            \
    struct waffle_##name {                                                  \
        struct api_object api;                                              \
        union native_##name *native;                                        \
    };                                                                      \
                                                                            \
    static inline struct api_object*                                        \
    waffle_##name##_cast_to_api_object(struct waffle_##name *x)             \
    {                                                                       \
        if (x)                                                              \
            return &x->api;                                                 \
        else                                                                \
            return NULL;                                                    \
    }

API_TYPE(display)
API_TYPE(config)
API_TYPE(context)
API_TYPE(window)

#undef API_TYPE

/// @}
