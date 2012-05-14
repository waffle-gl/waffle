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

    // From the GLX 1.4 spec:
    //      Currently no attributes are recognized, so attrib list must be
    //      NULL or empty (first attribute of None).
    self->glx->glx_window = glXCreateWindow(display->glx->xlib_display,
                                            config->glx->glx_fbconfig,
                                            self->glx->xcb_window,
                                            NULL /*attrib_list*/);
    if (!self->glx->glx_window) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR, "glXCreateWindow failed");
        goto error;
    }

    return self;

error:
    WCORE_ERROR_DISABLED({
        glx_window_destroy(self);
    });
    return NULL;
}

bool
glx_window_destroy(union native_window *self)
{
    if (!self)
        return true;

    bool ok = true;
    union native_display *dpy = self->glx->display;

    if (self->glx->glx_window)
        glXDestroyWindow(dpy->glx->xlib_display,
                               self->glx->glx_window);

    if (self->glx->xcb_window)
        ok &= x11_window_destroy(dpy->glx->xcb_connection,
                                 self->glx->xcb_window);

    free(self);
    return ok;
}


bool
glx_window_swap_buffers(union native_window *self)
{
    union native_display *dpy = self->glx->display;
    glXSwapBuffers(dpy->glx->xlib_display, self->glx->glx_window);
    return true;
}

/// @}
