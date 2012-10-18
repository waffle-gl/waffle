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

#include <stdlib.h>
#include <string.h>

#include "waffle/core/wcore_error.h"

#include "xegl_config.h"
#include "xegl_display.h"
#include "xegl_priv_egl.h"
#include "xegl_window.h"

static const struct wcore_window_vtbl xegl_window_wcore_vtbl;

static bool
xegl_window_destroy(struct wcore_window *wc_self)
{
    struct xegl_window *self = xegl_window(wc_self);
    struct xegl_display *dpy;
    bool ok = true;

    if (!self)
        return ok;

    dpy = xegl_display(wc_self->display);

    if (self->egl)
        ok &= egl_destroy_surface(dpy->egl, self->egl);

    ok &= x11_window_teardown(&self->x11);
    ok &= wcore_window_teardown(&self->wcore);
    free(self);
    return ok;
}

struct wcore_window*
xegl_window_create(struct wcore_platform *wc_plat,
                   struct wcore_config *wc_config,
                   int width,
                   int height)
{
    struct xegl_window *self;
    struct xegl_config *config = xegl_config(wc_config);
    struct xegl_display *dpy = xegl_display(wc_config->display);
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_window_init(&self->wcore, wc_config);
    if (!ok)
        goto error;

    ok = x11_window_init(&self->x11,
                         &dpy->x11,
                         config->xcb_visual_id,
                         width,
                         height);
    if (!ok)
        goto error;

    self->egl = xegl_egl_create_window_surface(dpy->egl,
                                               config->egl,
                                               self->x11.xcb,
                                               config->egl_render_buffer);
    if (!self->egl)
        goto error;

    self->wcore.vtbl = &xegl_window_wcore_vtbl;
    return &self->wcore;

error:
    xegl_window_destroy(&self->wcore);
    return NULL;
}

static bool
xegl_window_show(struct wcore_window *wc_self)
{
    return x11_window_show(&xegl_window(wc_self)->x11);
}

static bool
xegl_window_swap_buffers(struct wcore_window *wc_self)
{
    struct xegl_window *self = xegl_window(wc_self);
    struct xegl_display *dpy = xegl_display(wc_self->display);

    return egl_swap_buffers(dpy->egl, self->egl);
}

static union waffle_native_window*
xegl_window_get_native(struct wcore_window *wc_self)
{
    struct xegl_window *self = xegl_window(wc_self);
    struct xegl_display *dpy = xegl_display(wc_self->display);
    union waffle_native_window *n_window;

    WCORE_CREATE_NATIVE_UNION(n_window, x11_egl);
    if (!n_window)
        return NULL;

    xegl_display_fill_native(dpy, &n_window->x11_egl->display);
    n_window->x11_egl->xlib_window = self->x11.xcb;
    n_window->x11_egl->egl_surface = self->egl;

    return n_window;
}

static const struct wcore_window_vtbl xegl_window_wcore_vtbl = {
    .destroy = xegl_window_destroy,
    .get_native = xegl_window_get_native,
    .show = xegl_window_show,
    .swap_buffers = xegl_window_swap_buffers,
};
