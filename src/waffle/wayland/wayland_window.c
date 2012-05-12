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


    wl_shell_surface_set_toplevel(self->wl->wl_shell_surface);

    return self;

error:
    WCORE_ERROR_DISABLED({
        wayland_window_destroy(self);
    });
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
wayland_window_swap_buffers(union native_window *self)
{
    union native_display *dpy = self->wl->display;
    return egl_swap_buffers(dpy->wl->egl_display,
                            self->wl->egl_surface);
}

/// @}
