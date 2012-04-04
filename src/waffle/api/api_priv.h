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
#include <stddef.h>

struct api_object;
struct wcore_platform;

/// @brief Managed by waffle_init() and waffle_finish().
extern struct wcore_platform *api_current_platform;

/// @defgroup API Types
/// @{

struct api_object {
    size_t platform_id;
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
