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

#define WL_EGL_PLATFORM 1

#include <stdlib.h>
#include <string.h>

#include <wayland-client.h>
#undef container_of

#include <waffle/core/wcore_error.h>
#include <waffle/core/wcore_display.h>

#include "wayland_display.h"
#include "wayland_platform.h"
#include "wayland_priv_egl.h"

static const struct wcore_display_vtbl wayland_display_wcore_vtbl;

static bool
wayland_display_destroy(struct wcore_display *wc_self)
{
    struct wayland_display *self = wayland_display(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    if (self->egl)
        ok &= egl_terminate(self->egl);

    if (self->wl_display)
        wl_display_disconnect(self->wl_display);

    ok &= wcore_display_teardown(&self->wcore);
    free(self);
    return ok;
}

static void
wayland_display_listener(struct wl_display *display,
                         uint32_t name,
                         const char *interface,
                         uint32_t version,
                         void *data)
{
    struct wayland_display *self = data;

    if (!strncmp(interface, "wl_compositor", 14)) {
        self->wl_compositor = wl_display_bind(display, name, &wl_compositor_interface);
    }
    else if (!strncmp(interface, "wl_shell", 9)) {
        self->wl_shell = wl_display_bind(display, name, &wl_shell_interface);
    }
}

struct wcore_display*
wayland_display_connect(struct wcore_platform *wc_plat,
                        const char *name)
{
    struct wayland_display *self;
    bool ok = true;

    self = calloc(1, sizeof(*self));
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    ok = wcore_display_init(&self->wcore, wc_plat);
    if (!ok)
        goto error;

    self->wl_display = wl_display_connect(name);
    if (!self->wl_display) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR, "wl_display_connect failed");
        goto error;
    }

    wl_display_add_global_listener(self->wl_display,
                                   wayland_display_listener,
                                   self);

    self->egl = wayland_egl_initialize(self->wl_display);
    if (!self->egl)
        goto error;

    self->wcore.vtbl = &wayland_display_wcore_vtbl;
    return &self->wcore;

error:
    wayland_display_destroy(&self->wcore);
    return NULL;
}


static bool
wayland_display_supports_context_api(struct wcore_display *wc_self,
                                     int32_t waffle_context_api)
{
    return egl_supports_context_api(wc_self->platform, waffle_context_api);
}

static const struct wcore_display_vtbl wayland_display_wcore_vtbl = {
    .destroy = wayland_display_destroy,
    .supports_context_api = wayland_display_supports_context_api,
};
