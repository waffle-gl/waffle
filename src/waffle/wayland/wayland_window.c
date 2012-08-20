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

#include <wayland-egl.h>
#undef container_of

#include <waffle/core/wcore_error.h>
#include <waffle/waffle_wayland.h>

#include "wayland_config.h"
#include "wayland_display.h"
#include "wayland_priv_egl.h"
#include "wayland_window.h"

static const struct wcore_window_vtbl wayland_window_wcore_vtbl;

static bool
wayland_window_destroy(struct wcore_window *wc_self)
{
    struct wayland_window *self = wayland_window(wc_self);
    struct wayland_display *dpy;
    bool ok = true;

    if (!self)
        return ok;

    dpy = wayland_display(wc_self->display);

    if (self->egl)
        ok &= egl_destroy_surface(dpy->egl, self->egl);

    if (self->wl_window)
        wl_egl_window_destroy(self->wl_window);

    if (self->wl_shell_surface)
        wl_shell_surface_destroy(self->wl_shell_surface);

    if (self->wl_surface)
        wl_surface_destroy(self->wl_surface);

    ok &= wcore_window_teardown(wc_self);
    free(self);
    return ok;
}

struct wcore_window*
wayland_window_create(struct wcore_platform *wc_plat,
                      struct wcore_config *wc_config,
                      int width,
                      int height)
{
    struct wayland_window *self;
    struct wayland_config *config = wayland_config(wc_config);
    struct wayland_display *dpy = wayland_display(wc_config->display);
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_window_init(&self->wcore, wc_config);
    if (!ok)
        goto error;

    if (!dpy->wl_compositor) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "wayland compositor not found");
        goto error;
    }
    if (!dpy->wl_shell) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "wayland shell not found");
        goto error;
    }

    self->wl_surface = wl_compositor_create_surface(dpy->wl_compositor);
    if (!self->wl_surface) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "wl_compositor_create_surface failed");
        goto error;
    }

    self->wl_shell_surface = wl_shell_get_shell_surface(dpy->wl_shell,
                                                        self->wl_surface);
    if (!self->wl_shell_surface) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "wl_shell_get_shell_surface failed");
        goto error;
    }

    self->wl_window = wl_egl_window_create(self->wl_surface, width, height);
    if (!self->wl_window) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "wl_egl_window_create failed");
        goto error;
    }

    self->egl = wayland_egl_create_window_surface(dpy->egl,
                                                  config->egl,
                                                  self->wl_window,
                                                  config->egl_render_buffer);
    if (!self->egl)
        goto error;

    self->wcore.vtbl = &wayland_window_wcore_vtbl;
    return &self->wcore;

error:
    wayland_window_destroy(&self->wcore);
    return NULL;
}


static bool
wayland_window_show(struct wcore_window *wc_self)
{
    struct wayland_window *self = wayland_window(wc_self);

    wl_shell_surface_set_toplevel(self->wl_shell_surface);

    // FIXME: How to detect errors in Wayland?
    return true;
}

static bool
wayland_window_swap_buffers(struct wcore_window *wc_self)
{
    struct wayland_window *self = wayland_window(wc_self);
    struct wayland_display *dpy = wayland_display(wc_self->display);

    return egl_swap_buffers(dpy->egl, self->egl);
}

static union waffle_native_window*
wayland_window_get_native(struct wcore_window *wc_self)
{
    struct wayland_window *self = wayland_window(wc_self);
    struct wayland_display *dpy = wayland_display(wc_self->display);
    struct waffle_wayland_window *n_window;

    n_window = wcore_malloc(sizeof(*n_window));
    if (n_window == NULL)
        return NULL;

    wayland_display_fill_native(dpy, &n_window->display);
    n_window->wl_surface = self->wl_surface;
    n_window->wl_shell_surface = self->wl_shell_surface;
    n_window->wl_window = self->wl_window;
    n_window->egl_surface = self->egl;

    return (union waffle_native_window*) n_window;
}

static const struct wcore_window_vtbl wayland_window_wcore_vtbl = {
    .destroy = wayland_window_destroy,
    .get_native = wayland_window_get_native,
    .show = wayland_window_show,
    .swap_buffers = wayland_window_swap_buffers,
};
