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

/// @addtogroup wayland_window
/// @{

/// @file

#include "wayland_window.h"

#include <stdlib.h>
#include <string.h>

#include <waffle/native.h>
#include <waffle/core/wcore_error.h>
#include <waffle/x11/x11.h>

#include "wayland_priv_egl.h"
#include "wayland_priv_types.h"

union native_window*
wayland_window_create(
        union native_config *config,
        int width,
        int height)
{
    union native_display *display = config->wl->display;

    union native_window *self;
    NATIVE_ALLOC(self, wl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->wl->display = display;

    if (!display->wl->wl_compositor) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR, "wayland compositor not found");
        goto error;
    }
    if (!display->wl->wl_shell) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR, "wayland shell not found");
        goto error;
    }

    self->wl->wl_surface = wl_compositor_create_surface(display->wl->wl_compositor);
    if (!self->wl->wl_surface) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "wl_compositor_create_surface failed");
        goto error;
    }

    self->wl->wl_shell_surface = wl_shell_get_shell_surface(
                                        display->wl->wl_shell,
                                        self->wl->wl_surface);
    if (!self->wl->wl_shell_surface) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "wl_shell_get_shell_surface failed");
        goto error;
    }

    self->wl->wl_window = wl_egl_window_create(self->wl->wl_surface,
                                               width,
                                               height);
    if (!self->wl->wl_window) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR, "wl_egl_window_create failed");
        goto error;
    }

    self->wl->egl_surface = wayland_egl_create_window_surface(
                                    display->wl->egl_display,
                                    config->wl->egl_config,
                                    self->wl->wl_window,
                                    config->wl->egl_render_buffer);
    if (!self->wl->egl_surface)
     goto error;

    return self;

error:
    wayland_window_destroy(self);
    return NULL;
}

bool
wayland_window_destroy(union native_window *self)
{
    if (!self)
        return true;

    bool ok = true;
    union native_display *dpy = self->wl->display;

    if (self->wl->egl_surface)
        ok &= egl_destroy_surface(dpy->wl->egl_display,
                                  self->wl->egl_surface);

    if (self->wl->wl_window)
        wl_egl_window_destroy(self->wl->wl_window);
    if (self->wl->wl_shell_surface)
        wl_shell_surface_destroy(self->wl->wl_shell_surface);
    if (self->wl->wl_surface)
        wl_surface_destroy(self->wl->wl_surface);

    free(self);
    return ok;
}

bool
wayland_window_show(union native_window *native_self)
{
    struct wayland_window *self = native_self->wl;

    wl_shell_surface_set_toplevel(self->wl_shell_surface);

    // FIXME: How to detect errors in Wayland?
    return true;
}

bool
wayland_window_swap_buffers(union native_window *self)
{
    union native_display *dpy = self->wl->display;
    return egl_swap_buffers(dpy->wl->egl_display,
                            self->wl->egl_surface);
}

/// @}
