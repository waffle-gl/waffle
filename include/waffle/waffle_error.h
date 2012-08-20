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
/// This may be called when waffle is uninitialized [that is, before
/// waffle_init()].
WAFFLE_API const struct waffle_error_info*
waffle_error_get_info(void);

/// @brief Convert error code to string.
///
/// For example, convert @c WAFFLE_BAD_ATTRIBUTE to @c "WAFFLE_BAD_ATTRIBUTE" .
///
/// @return null if enum is invalid.
WAFFLE_API const char*
waffle_error_to_string(int32_t e);

enum waffle_error {
    /// The function succeeded.
    WAFFLE_NO_ERROR                     = 0x00,

    /// Waffle encountered a fatal error.  All future Waffle calls result in
    /// undefined behavior.
    WAFFLE_ERROR_FATAL                  = 0x01,

    /// Waffle encountered an error for which it lacks an error code.  This is
    /// usually produced when an underlying native call, such as
    /// XOpenDisplay(), fails for an unknown reason.
    WAFFLE_ERROR_UNKNOWN                = 0x02,

    /// You found a bug in Waffle.  Please report it.  The error message,
    /// obtained by waffle_error_get_info(), should contain a description of
    /// the bug.
    WAFFLE_ERROR_INTERNAL               = 0x03,

    /// Waffle failed to allocate memory.
    WAFFLE_ERROR_BAD_ALLOC              = 0x04,

    /// The failed function requires Waffle to be initialized with
    /// waffle_init().
    WAFFLE_ERROR_NOT_INITIALIZED        = 0x05,

    /// If Waffle has already been initialized by a successful call to
    /// waffle_init(), then subsequent calls to waffle_init() produce this
    /// error.
    WAFFLE_ERROR_ALREADY_INITIALIZED    = 0x06,

    /// An unrecognized attribute name or attribute value was passed in an
    /// attribute list.
    WAFFLE_ERROR_BAD_ATTRIBUTE          = 0x08,

    /// The failed function was passed an invalid argument.
    WAFFLE_ERROR_BAD_PARAMETER          = 0x10,

    /// The Waffle objects passed to the failed function belong to different
    /// displays.
    WAFFLE_ERROR_BAD_DISPLAY_MATCH      = 0x11,

    /// The requested action is unsupported on the current system or platform,
    /// but is otherwise valid.  For example, the following produce this
    /// error:
    ///     - Attemtping to choose a waffle_config whose API is OpenGL ES1
    ///       when the current platform is GLX.
    ///     - Calling `waffle_dl_sym(WAFFLE_DL_OPENGL_ES2, ...)` when the
    ///       OpenGL ES2 library is not present on the system.
    WAFFLE_ERROR_UNSUPPORTED            = 0x12,
};

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
