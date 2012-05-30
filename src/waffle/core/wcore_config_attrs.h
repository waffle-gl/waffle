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

/// @defgroup wcore_config_attrs wcore_config_attrs
/// @ingroup wcore
///
/// @brief Process the attribute list given to waffle_config_choose().
/// @{

/// @file

#pragma once

#include <stdbool.h>
#include <stdint.h>

/// @brief Encodes the attribute list received by waffle_config_choose().
struct wcore_config_attrs {
    int32_t context_api;
    int32_t context_major_version;
    int32_t context_minor_version;
    int32_t context_profile;

    int32_t rgb_size;
    int32_t rgba_size;

    int32_t red_size;
    int32_t green_size;
    int32_t blue_size;
    int32_t alpha_size;

    int32_t depth_size;
    int32_t stencil_size;

    int32_t sample_buffers;
    int32_t samples;

    bool double_buffered;

    bool accum_buffer;
};

bool
wcore_config_attrs_parse(
      const int32_t waffle_attrib_list[],
      struct wcore_config_attrs *attrs);

/// @}
