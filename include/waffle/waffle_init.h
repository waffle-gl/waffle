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

/// @file
/// @brief Initializing and terminating global library state.

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
/// waffle_init() produces an error. To reset waffle's global state, call
/// waffle_finish(). After calling waffle_finish(), it is safe to reinitialize
/// the library with a new set of attributes.
///
/// @par Initialization Attributes
///
///     Argument @a attrib_list is a null-terminated list of attributes with
///     which to initialize the waffle library. Each attribute consists of a
///     key/value pair. If an attribute is omitted, a default value is chosen.
///     Supplying a null pointer for @a attrib_list produces the same behavior
///     as supplying the empty list.  The attributes and their valid values
///     are listed in `enum waffle_enum`.  Some attribute combinations are
///     inherently invalid due to platform or API incompatibilities; for
///     example, desktop OpenGL is not available on Android.
///
/// @par Required Attributes
///
///     - WAFFLE_PLATFORM
///     - WAFFLE_GL_API
///
/// @par Example: Invalid Attribute List
///
///     The following is an invalid attribute list because desktop OpenGL is not
///     availabe on Android.  Passing it to waffle_init() will produce the @c
///     WAFFLE_INCOMPATIBLE_ATTRIBUTES error.
///         @code
///             {
///               WAFFLE_PLATFORM, WAFFLE_PLATFORM_ANDROID,
///               WAFFLE_GL_API,   WAFFLE_GL,
///               0,
///             }
///         @endcode
///
/// @par Errors
///
///     - WAFFLE_ALREADY_INITIALIZED @n
///         The library's global state has already been initialized with a call
///         to waffle_init(), but has not been reset with waffle_finish().
///
///     - WAFFLE_BAD_ATTRIBUTE @n
///         An item in @a attrib_list is unrecognized, missing, or has an
///         invalid value.
///
///     - WAFFLE_INCOMPATIBLE_ATTRIBUTES @n
///         The attribute list contains attributes that conflict with each
///         other.
///
/// @param attrib_list is a null-terminated list of key/value pairs that may be
///     null.
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
