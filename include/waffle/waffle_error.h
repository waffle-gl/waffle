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

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "waffle_visibility.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Get info on most recent failure.
///
/// Waffle functions usually return either a bool, in which false indicates
/// failure, or a pointer, in which null indicates failure. To receive more
/// information about the most recent failed function, call waffle_get_error().
///
/// This may be called when waffle is uninitialized (that is, before
/// waffle_init() and after waffle_finish()).
///
/// @return an error code from `enum waffle_error`.
WAFFLE_API int32_t
waffle_get_error(void);

/// @copybrief waffle_get_error()
///
/// This is a variant of waffle_get_error() that returns an error
/// message.
///
/// The error message is placed into @a message_buffer. If @a buffer_size is
/// 0, the @a message_buffer is ignored. If @a buffer_size is non-zero, than
/// the content of @a message_buffer will be a null-terminated string. No more
/// than `min(buffer_size, *message_length + 1)` bytes will be written to
/// @a message_buffer.
///
/// The message's length, according to strlen(), is returned in @a
/// message_length. If @a message_length is null, then it is ignored. If the
/// message does not fit into @a message_buffer, then @a message_length will be
/// the length of the message that would have been returned if @a message_buffer
/// had sufficient space.
///
/// This may be called when waffle is uninitialized (that is, before
/// waffle_init() and after waffle_finish()).
///
/// @see waffle_get_error()
WAFFLE_API int32_t
waffle_get_error_m(
        char message_buffer[],
        size_t buffer_size,
        size_t *message_length);

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
    WAFFLE_OLD_OBJECT                   = 0x07,
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
