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

/// @brief Managed by waffle_init() and waffle_finish().
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