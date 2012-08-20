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

/// @defgroup waffle_init waffle_init
/// @ingroup waffle_api
///
/// @brief Initializing and terminating global library state.
/// @{

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "waffle_portability.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Initialize the per-process global state of the waffle library.
///
/// This function must be called before calling any other waffle functions
/// If waffle is already initialized, then calling waffle_init() produces the
/// error @c WAFFLE_ERROR_ALREADY_INITIALIZED.
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
/// | .                                      |     .    |    . |       . | WAFFLE_PLATFORM_CGL          |
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
/// - WAFFLE_ERROR_ALREADY_INITIALIZED @n
///     The library's global state has already been initialized with a call
///     to waffle_init().
///
/// - WAFFLE_ERROR_BAD_ATTRIBUTE @n
///     An item in @a attrib_list is unrecognized, missing, or has an
///     invalid value.
///
/// @see waffle_enum
///
WAFFLE_API bool
waffle_init(const int32_t *attrib_list);

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
