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

/// @addtogroup xegl_display
/// @{

/// @file

#include "xegl_display.h"

#include <stdlib.h>

#include <xcb/xcb.h>
#include <X11/Xlib-xcb.h>

#include <waffle/native.h>
#include <waffle/core/wcore_error.h>
#include <waffle/x11/x11.h>

#include "xegl_priv_egl.h"
#include "xegl_priv_types.h"

union native_display*
xegl_display_connect(
        union native_platform *platform,
        const char *name)
{
    bool ok = true;

    union native_display *self;
    NATIVE_ALLOC(self, xegl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->xegl->platform = platform;

    ok &= x11_display_connect(name, &self->xegl->xlib_display, &self->xegl->xcb_connection);
    if (!ok)
        goto error;

    self->xegl->egl_display = xegl_egl_initialize(self->xegl->xlib_display);
    if (!self->xegl->egl_display)
        goto error;

    return self;

error:
    WCORE_ERROR_DISABLED({
        xegl_display_disconnect(self);
    });
    return NULL;
}

bool
xegl_display_disconnect(union native_display *self)
{
    bool ok = true;

    if (!self)
        return true;

    if (self->xegl->egl_display)
        ok &= egl_terminate(self->xegl->egl_display);

    if (self->xegl->xlib_display)
        ok &= x11_display_disconnect(self->xegl->xlib_display);

    free(self);
    return ok;
}

/// @}
