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

/// @defgroup waffle_error waffle_error
/// @ingroup waffle_api
///
/// @{

/// @file

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "waffle_portability.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Thread-local error info.
///
/// Waffle functions usually return either a bool, in which false indicates
/// failure, or a pointer, in which null indicates failure. In addition to
/// returning a flag indicating failure, most functions also clear and then
/// set some thread-local error state. The only functions that do not alter
/// the error state are those in this module, @ref waffle_error.
///
/// This struct contains the user-visible portion of the thread-local
/// error state.
struct waffle_error_info {
    /// @brief An error code from `enum waffle_error`.
    ///
    /// This may be `WAFFLE_NO_ERROR`.
    int32_t code;

    /// Never null.
    const char *message;

    /// @brief The length of @ref message according to strlen().
    size_t message_length;
};

/// @brief Get the current thread's error code.
///
/// This may be called when waffle is uninitialized (that is, before
/// waffle_init() and after waffle_finish()).
///
/// Calling this function is equivalent to obtaining the error code with
/// `waffle_error_get_info()->code`.
///
WAFFLE_API int32_t
waffle_error_get_code(void);

/// @brief Get the current thread's error info.
///
/// This function never returns null. The returned pointer becomes invalid
/// when the thread-local error state changes.
///
/// This may be called when waffle is uninitialized (that is, before
/// waffle_init() and after waffle_finish()).
WAFFLE_API const struct waffle_error_info*
waffle_error_get_info(void);

/// @brief Convert error code to string.
///
/// For example, convert @c WAFFLE_BAD_ATTRIBUTE to @c "WAFFLE_BAD_ATTRIBUTE" .
///
/// @return null if enum is invalid.
WAFFLE_API const char*
waffle_error_to_string(int32_t e);

/// The documentation for each waffle function lists errors the function can
/// produce. In addition to each list, all functions can produce the
/// following errors:
///     - WAFFLE_FATAL_ERROR
///     - WAFFLE_UNKOWN_ERROR
///     - WAFFLE_OUT_OF_MEMORY
///     - WAFFLE_INTERNAL_ERROR
///
/// @see waffle_error_listener
///
enum waffle_error {
    WAFFLE_NO_ERROR                     = 0x00,

    /// A terrible thing has happened. The behavior of all future waffle
    /// calls is undefined. You should clean up and exit as soon as possible.
    WAFFLE_FATAL_ERROR                  = 0x01,

    /// @brief Something went wrong, but waffle lacks an error code for it.
    ///
    /// This is usually emitted when an underlying native call, such as
    /// XOpenDisplay(), fails for an unknown reason.
    WAFFLE_UNKNOWN_ERROR                = 0x02,

    /// @brief You've found a bug in waffle.
    WAFFLE_INTERNAL_ERROR               = 0x03,

    WAFFLE_OUT_OF_MEMORY                = 0x04,

    WAFFLE_NOT_INITIALIZED              = 0x05,
    WAFFLE_ALREADY_INITIALIZED          = 0x06,
    WAFFLE_BAD_ATTRIBUTE                = 0x08,
    WAFFLE_INCOMPATIBLE_ATTRIBUTES      = 0x09,
    WAFFLE_BAD_PARAMETER                = 0x10,

    /// @brief You passed objects to waffle that belong to different displays.
    WAFFLE_BAD_DISPLAY_MATCH            = 0x11,

    /// @brief Requested action is unsupported on the current platform.
    ///
    /// For example, on GLX it is not possible to create an OpenGL ES1 context.
    WAFFLE_UNSUPPORTED_ON_PLATFORM      = 0x12,

    /// @brief You found a feature whose implementation is incomplete.
    WAFFLE_NOT_IMPLEMENTED              = 0x13,
};

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
