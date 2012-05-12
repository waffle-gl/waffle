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

/// @defgroup waffle_config waffle_config
/// @ingroup waffle_api
/// @{

/// @file

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "waffle_visibility.h"

#ifdef __cplusplus
extern "C" {
#endif

struct waffle_config;
struct waffle_display;

/// @brief Choose a config satisfying some attributes.
///
///
/// ### Attributes ###
///
/// Argument @a attrib_list specifies a list of attributes, described in the
/// table below, that the returned config must satisfy. The list consists of
/// a zero-terminated sequence of name/value pairs. If an attribute is absent
/// from the list, then the attribute assumes its default value. If @a
/// attrib_list is null or empty, then all attributes assume their default
/// values.
///
/// | Name                                    | Type | Default          |
/// |:----------------------------------------|-----:|-----------------:|
/// | WAFFLE_RED_SIZE                         |  int | WAFFLE_DONT_CARE |
/// | WAFFLE_GREEN_SIZE                       |  int | WAFFLE_DONT_CARE |
/// | WAFFLE_BLUE_SIZE                        |  int | WAFFLE_DONT_CARE |
/// | WAFFLE_ALPHA_SIZE                       |  int | WAFFLE_DONT_CARE |
/// | .                                       |    . |                . |
/// | WAFFLE_DEPTH_SIZE                       |  int | WAFFLE_DONT_CARE |
/// | WAFFLE_STENCIL_SIZE                     |  int | WAFFLE_DONT_CARE |
/// | .                                       |    . |                . |
/// | WAFFLE_SAMPLE_BUFFERS                   | bool |            false |
/// | WAFFLE_SAMPLES                          |  int |                0 |
/// | .                                       |    . |                . |
/// | WAFFLE_DOUBLE_BUFFERED                  | bool |             true |
///
///
/// ### Example Attribute Lists ###
///
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/// const int32_t empty_list[] = {0};
///
/// const int32_t rgb565[] = {
///     WAFFLE_RED_SIZE,    5,
///     WAFFLE_GREEN_SIZE,  6,
///     WAFFLE_BLUE_SIZE,   5,
///     0,
/// };
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///
///
/// @return A config that satisfies @a attrib_list. If no such config is
///     found, return null.
/// @see waffle_enum
///
WAFFLE_API struct waffle_config*
waffle_config_choose(struct waffle_display *dpy, const int32_t attrib_list[]);

WAFFLE_API bool
waffle_config_destroy(struct waffle_config *self);

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
