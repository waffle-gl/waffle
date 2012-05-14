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

/// @defgroup waffle_enum waffle_enum
/// @ingroup waffle_api
/// @{

/// @file

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "waffle_visibility.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Convert a waffle_enum to a string.
///
/// For example, convert @c WAFFLE_OPENGL to @c "WAFFLE_OPENGL" .
///
/// @return null if enum is invalid.
WAFFLE_API const char*
waffle_enum_to_string(int32_t e);

/// @brief All non-error enums used by waffle.
///
/// All enums are placed in one list so that each has a distinct value. This
/// enables better error detection at API entry points.
enum waffle_enum {
    // -----------------------------------------
    // Generic
    // -----------------------------------------

    WAFFLE_DONT_CARE                    = -1,
    WAFFLE_NONE                         =  0,

    // -----------------------------------------
    // For waffle_init()
    // -----------------------------------------

    WAFFLE_PLATFORM                     = 0x0010,
        WAFFLE_PLATFORM_ANDROID         = 0x0011,
        WAFFLE_PLATFORM_COCOA           = 0x0012,
        WAFFLE_PLATFORM_GLX             = 0x0013,
        WAFFLE_PLATFORM_WAYLAND         = 0x0014,
        WAFFLE_PLATFORM_X11_EGL         = 0x0015,

    WAFFLE_OPENGL_API                   = 0x0100,
        WAFFLE_OPENGL                   = 0x0101,
        WAFFLE_OPENGL_ES1               = 0x0102,
        WAFFLE_OPENGL_ES2               = 0x0103,

    // -----------------------------------------
    // For waffle_config_choose()
    // -----------------------------------------

    WAFFLE_RED_SIZE                     = 0x0201,
    WAFFLE_GREEN_SIZE                   = 0x0202,
    WAFFLE_BLUE_SIZE                    = 0x0203,
    WAFFLE_ALPHA_SIZE                   = 0x0204,

    WAFFLE_DEPTH_SIZE                   = 0x0205,
    WAFFLE_STENCIL_SIZE                 = 0x0206,

    WAFFLE_SAMPLE_BUFFERS               = 0x0207,
    WAFFLE_SAMPLES                      = 0x0208,

    WAFFLE_DOUBLE_BUFFERED              = 0x0209,

    // -----------------------------------------
    // For waffle_dlsym_gl()
    // -----------------------------------------

    WAFFLE_DL_OPENGL                    = 0x0301,
    WAFFLE_DL_OPENGL_ES1                = 0x0302,
    WAFFLE_DL_OPENGL_ES2                = 0x0303,
};

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
