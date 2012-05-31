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

/// @addtogroup cgl_display
/// @{

/// @file

#include "cgl_display.h"

#include <stdlib.h>

#include <waffle/native.h>
#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_error.h>

union native_display*
cgl_display_connect(
        union native_platform *native_platform,
        const char *name)
{
    union native_display *native_self;

    NATIVE_ALLOC(native_self, cgl);
    if (!native_self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    return native_self;
}

bool
cgl_display_disconnect(union native_display *native_self)
{
    free(native_self);
    return true;
}

bool
cgl_display_supports_context_api(
        union native_display *native_self,
        int32_t context_api)
{
    switch (context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            return true;
        case WAFFLE_CONTEXT_OPENGL_ES1:
            return false;
        case WAFFLE_CONTEXT_OPENGL_ES2:
            return false;
        default:
            wcore_error_internal("waffle_context_api has bad value %#x",
                                 context_api);
            return false;
    }
}

/// @}
