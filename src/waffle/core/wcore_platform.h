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

/// @defgroup wcore_platform wcore_platform
/// @ingroup wcore
///
/// @brief Abstract native platform.
/// @{

/// @file

#pragma once

#include <stdbool.h>
#include <stdint.h>

struct native_dispatch;
union native_platform;

struct wcore_platform {
    /// @brief Each instance has a unique id.
    uint64_t id;

    /// @brief One of WAFFLE_PLATFORM_*. Denotes type of `native`.
    int native_tag;

    union native_platform *native;
    const struct native_dispatch *dispatch;
};

struct wcore_platform*
wcore_platform_create(int platform);

bool
wcore_platform_destroy(struct wcore_platform *self);

/// @}
