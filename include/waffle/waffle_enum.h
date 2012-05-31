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

/// @defgroup waffle_enum waffle_enum
/// @ingroup waffle_api
/// @{

/// @file

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "waffle_portable.h"

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
    // ------------------------------------------------------------------
    // Generic
    // ------------------------------------------------------------------

    WAFFLE_DONT_CARE                                            = -1,
    WAFFLE_NONE                                                 =  0,

    // ------------------------------------------------------------------
    // For waffle_init()
    // ------------------------------------------------------------------

    WAFFLE_PLATFORM                                             = 0x0010,
        WAFFLE_PLATFORM_ANDROID                                 = 0x0011,
        WAFFLE_PLATFORM_CGL                                     = 0x0012,
        WAFFLE_PLATFORM_GLX                                     = 0x0013,
        WAFFLE_PLATFORM_WAYLAND                                 = 0x0014,
        WAFFLE_PLATFORM_X11_EGL                                 = 0x0015,

    // ------------------------------------------------------------------
    // For waffle_config_choose()
    // ------------------------------------------------------------------

    WAFFLE_CONTEXT_API                                          = 0x020a,
        WAFFLE_CONTEXT_OPENGL                                   = 0x020b,
        WAFFLE_CONTEXT_OPENGL_ES1                               = 0x020c,
        WAFFLE_CONTEXT_OPENGL_ES2                               = 0x020d,

    WAFFLE_CONTEXT_MAJOR_VERSION                                = 0x020e,
    WAFFLE_CONTEXT_MINOR_VERSION                                = 0x020f,

    WAFFLE_CONTEXT_PROFILE                                      = 0x0210,
        WAFFLE_CONTEXT_CORE_PROFILE                             = 0x0211,
        WAFFLE_CONTEXT_COMPATIBILITY_PROFILE                    = 0x0212,

    WAFFLE_RED_SIZE                                             = 0x0201,
    WAFFLE_GREEN_SIZE                                           = 0x0202,
    WAFFLE_BLUE_SIZE                                            = 0x0203,
    WAFFLE_ALPHA_SIZE                                           = 0x0204,

    WAFFLE_DEPTH_SIZE                                           = 0x0205,
    WAFFLE_STENCIL_SIZE                                         = 0x0206,

    WAFFLE_SAMPLE_BUFFERS                                       = 0x0207,
    WAFFLE_SAMPLES                                              = 0x0208,

    WAFFLE_DOUBLE_BUFFERED                                      = 0x0209,

    WAFFLE_ACCUM_BUFFER                                         = 0x0213,

    // ------------------------------------------------------------------
    // For waffle_dl_sym()
    // ------------------------------------------------------------------

    WAFFLE_DL_OPENGL                                            = 0x0301,
    WAFFLE_DL_OPENGL_ES1                                        = 0x0302,
    WAFFLE_DL_OPENGL_ES2                                        = 0x0303,
};

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
