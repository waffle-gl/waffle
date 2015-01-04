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

#include "waffle.h"

/// @brief Thread-local info for the wcore_error module.
struct wcore_error_tinfo;

struct wcore_error_tinfo*
wcore_error_tinfo_create(void);

bool
wcore_error_tinfo_destroy(struct wcore_error_tinfo *self);

/// @brief Reset the error state to WAFFLE_NO_ERROR.
void
wcore_error_reset(void);

/// @brief Set error code for client.
///
/// @param error is an `enum waffle_error`.
void
wcore_error(enum waffle_error error);

/// @brief Set error code and message for client.
///
/// @param error is an `enum waffle_error`.
/// @param format may be null.
void
wcore_errorf(enum waffle_error error, const char *format, ...);

/// @brief Emit error for errno.
///
/// Set error code to WAFFLE_ERROR_UNKNOWN and place the output of
/// strerror() in the error message. If \a format is not null,
/// then prepend the strerror() message with "${format}: ".
void
wcore_error_errno(const char *format, ...);

/// @brief Emit WAFFLE_ERROR_BAD_ATTRIBUTE with a default error message.
void
wcore_error_bad_attribute(intptr_t attr);

/// @brief Set error to WAFFLE_INTERNAL_ERROR with source location.
#define wcore_error_internal(format, ...) \
    _wcore_error_internal(__FILE__, __LINE__, format, __VA_ARGS__)

/// @brief Execute a statement with errors disabled.
#define WCORE_ERROR_DISABLED(statement) \
    do { \
        _wcore_error_disable(); \
        statement \
        _wcore_error_enable(); \
    } while (0)

/// @brief Get the last set error code.
enum waffle_error
wcore_error_get_code(void);

/// @brief Get the user-visible portion of the error state.
const struct waffle_error_info*
wcore_error_get_info(void);

void
_wcore_error_internal(const char *file, int line, const char *format, ...);

void _wcore_error_enable(void);
void _wcore_error_disable(void);
