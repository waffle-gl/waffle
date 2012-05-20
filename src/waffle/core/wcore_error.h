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

/// @defgroup wcore_error wcore_error
/// @ingroup wcore
///
/// @brief Error handling and reporting
///
/// All functions operate on thread-local storage.
///
/// @{

/// @file

#pragma once

#include <stdbool.h>

#include <waffle/waffle_error.h>


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
wcore_error(int error);

/// @brief Set error code and message for client.
///
/// @param error is an `enum waffle_error`.
/// @param format may be null.
void
wcore_errorf(int error, const char *format, ...);

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
int
wcore_error_get_code(void);

/// @brief Get the user-visible portion of the error state.
const struct waffle_error_info*
wcore_error_get_info(void);

void
_wcore_error_internal(const char *file, int line, const char *format, ...);

void _wcore_error_enable(void);
void _wcore_error_disable(void);

/// @}
