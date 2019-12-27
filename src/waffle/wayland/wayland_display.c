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

// The wrapper must be included before wayland-client.h
#include "wayland_wrapper.h"
#undef container_of

#include "wcore_error.h"
#include "wcore_display.h"

#include "wegl_display.h"

#include "wayland_display.h"
#include "wayland_platform.h"
#include "wl-xdg-shell-proto.h"

bool
wayland_display_destroy(struct wcore_display *wc_self)
{
    struct wayland_display *self = wayland_display(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    ok &= wegl_display_teardown(&self->wegl);

    if (self->wl_display)
        wl_display_disconnect(self->wl_display);

    free(self);
    return ok;
}

static void
xdg_wm_base_ping(void *data, struct xdg_wm_base *shell, uint32_t serial)
{
       xdg_wm_base_pong(shell, serial);
}

static const struct xdg_wm_base_listener wm_base_listener = {
       .ping = xdg_wm_base_ping,
};

static void
registry_listener_global(void *data,
                         struct wl_registry *registry,
                         uint32_t name,
                         const char *interface,
                         uint32_t version)
{
    struct wayland_display *self = data;

    if (!strncmp(interface, "wl_compositor", 14)) {
        self->wl_compositor = wl_registry_bind(self->wl_registry, name,
                                               &wl_compositor_interface, 1);
    }
    else if (!strncmp(interface, "xdg_wm_base", 12)) {
        self->xdg_shell = wl_registry_bind(self->wl_registry, name,
                                           &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(self->xdg_shell, &wm_base_listener, NULL);
    }
    else if (!strncmp(interface, "wl_shell", 9)) {
        self->wl_shell = wl_registry_bind(self->wl_registry, name,
                                          &wl_shell_interface, 1);
    }
}

static void
registry_listener_global_remove(void *data,
                                struct wl_registry *registry,
                                uint32_t name)
{
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_listener_global,
    .global_remove = registry_listener_global_remove
};

struct wcore_display*
wayland_display_connect(struct wcore_platform *wc_plat,
                        const char *name)
{
    struct wayland_display *self;
    bool ok = true;
    int error = 0;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    self->wl_display = wl_display_connect(name);
    if (!self->wl_display) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "wl_display_connect failed");
        goto error;
    }

    self->wl_registry = wl_display_get_registry(self->wl_display);
    if (!self->wl_registry) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "wl_display_get_registry failed");
        goto error;
    }

    error = wl_registry_add_listener(self->wl_registry,
                                     &registry_listener,
                                     self);
    if (error < 0) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "wl_registry_add_listener failed");
        goto error;
    }

    // Block until the Wayland server has processed all pending requests and
    // has sent out pending events on all event queues. This should ensure
    // that the registry listener has received announcement of the shell and
    // compositor.
    ok = wayland_display_sync(self);
    if (!ok)
        goto error;

    if (!self->wl_compositor) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "failed to bind to the wayland "
                     "compositor");
        goto error;
    }

    if (!self->xdg_shell && !self->wl_shell) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "failed to bind to the wayland "
                     "shell");
        goto error;
    }

    ok = wegl_display_init(&self->wegl, wc_plat, self->wl_display);
    if (!ok)
        goto error;

    return &self->wegl.wcore;

error:
    wayland_display_destroy(&self->wegl.wcore);
    return NULL;
}

void
wayland_display_fill_native(struct wayland_display *self,
                            struct waffle_wayland_display *n_dpy)
{
    n_dpy->wl_display = self->wl_display;
    n_dpy->wl_compositor = self->wl_compositor;
    n_dpy->wl_shell = self->wl_shell;
    n_dpy->egl_display = self->wegl.egl;
    n_dpy->xdg_shell = self->xdg_shell;
}

union waffle_native_display*
wayland_display_get_native(struct wcore_display *wc_self)
{
    struct wayland_display *self = wayland_display(wc_self);
    union waffle_native_display *n_dpy;

    WCORE_CREATE_NATIVE_UNION(n_dpy, wayland);
    if (!n_dpy)
        return NULL;

    wayland_display_fill_native(self, n_dpy->wayland);

    return n_dpy;
}

bool
wayland_display_sync(struct wayland_display *dpy)
{
    if (wl_display_roundtrip(dpy->wl_display) == -1) {
        wcore_error_errno("error on wl_display");
        return false;
    }

    return true;
}
