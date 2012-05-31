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
    xegl_display_disconnect(self);
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

bool
xegl_display_supports_context_api(
        union native_display *self,
        int32_t context_api)
{
    union native_platform *platform = self->xegl->platform;
    return egl_supports_context_api(platform->xegl->linux_, context_api);
}

/// @}
