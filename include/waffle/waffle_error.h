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

#include <stdint.h>
#include <stddef.h>

#include "waffle_portability.h"

#ifdef __cplusplus
extern "C" {
#endif

struct waffle_error_info {
    int32_t code;
    const char *message;
    size_t message_length;
};

WAFFLE_API int32_t
waffle_error_get_code(void);

WAFFLE_API const struct waffle_error_info*
waffle_error_get_info(void);

WAFFLE_API const char*
waffle_error_to_string(int32_t e);

enum waffle_error {
    WAFFLE_NO_ERROR                     = 0x00,
    WAFFLE_ERROR_FATAL                  = 0x01,
    WAFFLE_ERROR_UNKNOWN                = 0x02,
    WAFFLE_ERROR_INTERNAL               = 0x03,
    WAFFLE_ERROR_BAD_ALLOC              = 0x04,
    WAFFLE_ERROR_NOT_INITIALIZED        = 0x05,
    WAFFLE_ERROR_ALREADY_INITIALIZED    = 0x06,
    WAFFLE_ERROR_BAD_ATTRIBUTE          = 0x08,
    WAFFLE_ERROR_BAD_PARAMETER          = 0x10,
    WAFFLE_ERROR_BAD_DISPLAY_MATCH      = 0x11,
    WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM = 0x12,
    WAFFLE_ERROR_BUILT_WITHOUT_SUPPORT   = 0x13,
};

#ifdef __cplusplus
} // end extern "C"
#endif
