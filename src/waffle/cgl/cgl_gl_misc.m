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
