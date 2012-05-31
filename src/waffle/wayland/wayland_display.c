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

/// @addtogroup wayland_display
/// @{

/// @file

#include "wayland_display.h"

#include <stdlib.h>
#include <string.h>

#include <waffle/native.h>
#include <waffle/core/wcore_error.h>

#include "wayland_priv_egl.h"
#include "wayland_priv_types.h"

static void
wayland_display_listener(
        struct wl_display *display,
        uint32_t id,
        const char *interface,
        uint32_t version,
        void *data)
{
    union native_display *self = data;

    if (!strncmp(interface, "wl_compositor", 14)) {
        self->wl->wl_compositor = wl_display_bind(display, id,
                                                  &wl_compositor_interface);
    }
    else if (!strncmp(interface, "wl_shell", 9)) {
        self->wl->wl_shell = wl_display_bind(display, id,
                                             &wl_shell_interface);
    }
}

union native_display*
wayland_display_connect(
        union native_platform *platform,
        const char *name)
{
    union native_display *self;
    NATIVE_ALLOC(self, wl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->wl->platform = platform;

    self->wl->wl_display = wl_display_connect(name);
    if (!self->wl->wl_display) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR, "wl_display_connect failed");
        goto error;
    }

    wl_display_add_global_listener(self->wl->wl_display,
                                   wayland_display_listener,
                                   self);

    self->wl->egl_display = wayland_egl_initialize(self->wl->wl_display);
    if (!self->wl->egl_display)
        goto error;

    return self;

error:
    wayland_display_disconnect(self);
    return NULL;
}

bool
wayland_display_disconnect(union native_display *self)
{
    bool ok = true;

    if (!self)
        return true;

    if (self->wl->egl_display)
        ok &= egl_terminate(self->wl->egl_display);

    if (self->wl->wl_display)
        wl_display_disconnect(self->wl->wl_display);

    free(self);
    return ok;
}

bool
wayland_display_supports_context_api(
        union native_display *self,
        int32_t context_api)
{
    union native_platform *platform = self->wl->platform;
    return egl_supports_context_api(platform->wl->linux_, context_api);
}


/// @}
