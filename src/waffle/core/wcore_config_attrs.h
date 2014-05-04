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

    int32_t samples;

    bool context_forward_compatible;
    bool context_debug;
    bool double_buffered;
    bool sample_buffers;
    bool accum_buffer;
};

bool
wcore_config_attrs_parse(
      const int32_t waffle_attrib_list[],
      struct wcore_config_attrs *attrs);

bool
wcore_config_attrs_version_eq(
      const struct wcore_config_attrs *attrs,
      int merged_version);

bool
wcore_config_attrs_version_gt(
      const struct wcore_config_attrs *attrs,
      int merged_version);

bool
wcore_config_attrs_version_ge(
      const struct wcore_config_attrs *attrs,
      int merged_version);

bool
wcore_config_attrs_version_lt(
      const struct wcore_config_attrs *attrs,
      int merged_version);

bool
wcore_config_attrs_version_le(
      const struct wcore_config_attrs *attrs,
      int merged_version);
