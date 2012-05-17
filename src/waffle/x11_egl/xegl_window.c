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
xegl_window_swap_buffers(union native_window *self)
{
    return egl_swap_buffers(self->xegl->display->xegl->egl_display,
                            self->xegl->egl_surface);
}

/// @}
