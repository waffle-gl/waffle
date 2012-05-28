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

/// @defgroup waffle_init waffle_init
/// @ingroup waffle_api
///
/// @brief Initializing and terminating global library state.
/// @{

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "waffle_portable.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Initialize the per-process global state of the waffle library.
///
/// This function must be called before calling any other waffle functions
/// If waffle is already initialized, then calling waffle_init() produces the
/// error @c WAFFLE_ALREADY_INITIALIZED.
///
///
/// ### Attributes ###
///
/// Argument @a attrib_list specifies a list of attributes, described in the
/// table below, with which to initialize the waffle library. The list consists
/// of a zero-terminated sequence of name/value pairs. If an attribute is
/// absent from the list, then the attribute assumes its default value. If @a
/// attrib_list is null or empty, then all attributes assume their default
/// values.
///
/// |Name                                    | Required | Type | Default | Choices                      |
/// |:---------------------------------------|:--------:|-----:|--------:|:-----------------------------|
/// | WAFFLE_PLATFORM                        |   yes    | enum |    none | WAFFLE_PLATFORM_ANDROID      |
/// | .                                      |     .    |    . |       . | WAFFLE_PLATFORM_COCOA        |
/// | .                                      |     .    |    . |       . | WAFFLE_PLATFORM_CGL          |
/// | .                                      |     .    |    . |       . | WAFFLE_PLATFORM_WAYLAND      |
/// | .                                      |     .    |    . |       . | WAFFLE_PLATFORM_X11_EGL      |
///
///
/// ### Example Attribute Lists ###
///
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/// const int32_t attrib_list[] = {
///   WAFFLE_PLATFORM,      WAFFLE_PLATFORM_ANDROID,
///   0,
/// }
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///
///
/// ### Errors ###
///
/// - WAFFLE_ALREADY_INITIALIZED @n
///     The library's global state has already been initialized with a call
///     to waffle_init().
///
/// - WAFFLE_BAD_ATTRIBUTE @n
///     An item in @a attrib_list is unrecognized, missing, or has an
///     invalid value.
///
/// @see waffle_enum
///
WAFFLE_API bool
waffle_init(const int32_t *attrib_list);

/// @brief Get the platform given to waffle_init().
///
/// This may be called before waffle_init(), in which `WAFFLE_NONE`
/// is returned. Otherwise, one of `WAFFLE_PLATFORM_*` is returned.
WAFFLE_API int32_t
waffle_get_platform(void);

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
