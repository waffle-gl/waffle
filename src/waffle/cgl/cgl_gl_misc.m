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

/// @addtogroup cgl_gl_misc
/// @{

/// @file

#include "cgl_gl_misc.h"

#include <waffle/native.h>

#include "cgl_context.h"
#include "cgl_window.h"

@class NSView;

bool
cgl_make_current(
        union native_display *dpy,
        union native_window *window,
        union native_context *ctx)
{
    NSOpenGLContext *cocoa_ctx = ctx ? ctx->cgl->ns_context : NULL;
    WaffleGLView* cocoa_view = window ? window->cgl->gl_view : NULL;

    if (cocoa_view)
        cocoa_view.glContext = cocoa_ctx;

    if (cocoa_ctx) {
        [cocoa_ctx makeCurrentContext];
        [cocoa_ctx setView:cocoa_view];
    }

    return true;
}

void*
cgl_get_proc_address(
        union native_platform *native,
        const char *name)
{
    // There is no CGLGetProcAddress.
    return NULL;
}

/// @}
