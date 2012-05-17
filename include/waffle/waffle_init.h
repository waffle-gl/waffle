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

#include "waffle_visibility.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Initialize the per-process global state of the waffle library.
///
/// This function must be called before calling any other waffle functions
/// (except waffle_finish()). If waffle is already initialized, then calling
/// waffle_init() produces the error @c WAFFLE_ALREADY_INITIALIZED. To reset
/// waffle's global state, call waffle_finish(). After calling
/// waffle_finish(), it is safe to reinitialize the library with a new set of
/// attributes.
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
/// | .                                      |     .    |    . |       . | WAFFLE_PLATFORM_GLX          |
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
///     to waffle_init(), but has not been reset with waffle_finish().
///
/// - WAFFLE_BAD_ATTRIBUTE @n
///     An item in @a attrib_list is unrecognized, missing, or has an
///     invalid value.
///
/// @see waffle_enum
///
WAFFLE_API bool
waffle_init(const int32_t *attrib_list);

/// @brief Reset the per-process global state of the waffle library.
///
/// This restores waffle to its uninitialized state. If the library is
/// uninitialized, then calling waffle_finish() has no effect and the function
/// trivially succeeds. After calling waffle_finish(), it is safe to
/// reinitialize waffle with waffle_init().
///
/// This invalidates all waffle objects. Passing an invalid object to a
/// waffle function produces a WAFFLE_OLD_OBJECT error.
///
/// If waffle_finish() returns false, then things have gone terribly wrong;
/// all future waffle calls result in undefined behavior.
///
WAFFLE_API bool
waffle_finish(void);

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
