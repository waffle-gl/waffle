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

/// @addtogroup xegl_window
/// @{

/// @file

#include "xegl_window.h"

#include <stdlib.h>
#include <string.h>

#include <waffle/native.h>
#include <waffle/core/wcore_error.h>
#include <waffle/x11/x11.h>

#include "xegl_priv_egl.h"
#include "xegl_priv_types.h"

union native_window*
xegl_window_create(
        union native_config *config,
        int width,
        int height)
{
    union native_display *display = config->xegl->display;

    union native_window *self;
    NATIVE_ALLOC(self, xegl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->xegl->display = display;
    self->xegl->xcb_window = x11_window_create(
                                display->xegl->xcb_connection,
                                config->xegl->xcb_visual_id,
                                width,
                                height);
    if (!self->xegl->xcb_window)
        goto error;

    self->xegl->egl_surface = xegl_egl_create_window_surface(
                                    display->xegl->egl_display,
                                    config->xegl->egl_config,
                                    self->xegl->xcb_window,
                                    config->xegl->egl_render_buffer);
    if (!self->xegl->egl_surface)
     goto error;

    return self;

error:
    xegl_window_destroy(self);
    return NULL;
}

bool
xegl_window_destroy(union native_window *self)
{
    if (!self)
        return true;

    bool ok = true;
    union native_display *dpy = self->xegl->display;

    if (self->xegl->egl_surface)
        ok &= egl_destroy_surface(dpy->xegl->egl_display,
                                  self->xegl->egl_surface);
    if (self->xegl->xcb_window)
        ok &= x11_window_destroy(dpy->xegl->xcb_connection,
                                 self->xegl->xcb_window);

    free(self);
    return ok;
}

bool
xegl_window_show(union native_window *native_self)
{
    struct xegl_window *self = native_self->xegl;
    struct xegl_display *display = self->display->xegl;

    return x11_window_show(display->xcb_connection, self->xcb_window);
}


bool
xegl_window_swap_buffers(union native_window *self)
{
    return egl_swap_buffers(self->xegl->display->xegl->egl_display,
                            self->xegl->egl_surface);
}

/// @}
