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
