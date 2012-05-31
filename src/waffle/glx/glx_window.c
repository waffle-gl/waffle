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

/// @addtogroup glx_window
/// @{

/// @file

#include "glx_window.h"

#include <stdlib.h>
#include <string.h>

#include <waffle/native.h>
#include <waffle/core/wcore_error.h>
#include <waffle/x11/x11.h>

#include "glx_priv_types.h"

union native_window*
glx_window_create(
        union native_config *config,
        int width,
        int height)
{
    union native_display *display = config->glx->display;

    union native_window *self;
    NATIVE_ALLOC(self, glx);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->glx->display = display;
    self->glx->xcb_window = x11_window_create(
                                display->glx->xcb_connection,
                                config->glx->xcb_visual_id,
                                width,
                                height);
    if (!self->glx->xcb_window)
        goto error;

    return self;

error:
    glx_window_destroy(self);
    return NULL;
}

bool
glx_window_destroy(union native_window *self)
{
    if (!self)
        return true;

    bool ok = true;
    union native_display *dpy = self->glx->display;

    if (self->glx->xcb_window)
        ok &= x11_window_destroy(dpy->glx->xcb_connection,
                                 self->glx->xcb_window);

    free(self);
    return ok;
}

bool
glx_window_show(union native_window *native_self)
{
    struct glx_window *self = native_self->glx;
    struct glx_display *display = self->display->glx;

    return x11_window_show(display->xcb_connection, self->xcb_window);
}

bool
glx_window_swap_buffers(union native_window *self)
{
    union native_display *dpy = self->glx->display;
    glXSwapBuffers(dpy->glx->xlib_display, self->glx->xcb_window);
    return true;
}

/// @}
