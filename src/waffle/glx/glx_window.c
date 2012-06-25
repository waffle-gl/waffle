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

#include <waffle/core/wcore_error.h>

#include "glx_config.h"
#include "glx_display.h"
#include "glx_window.h"

static const struct wcore_window_vtbl glx_window_wcore_vtbl;

static bool
glx_window_destroy(struct wcore_window *wc_self)
{
    struct glx_window *self = glx_window(wc_self);
    bool ok = true;

    if (!wc_self)
        return ok;

    ok &= x11_window_teardown(&self->x11);
    ok &= wcore_window_teardown(wc_self);
    free(self);
    return ok;
}

struct wcore_window*
glx_window_create(struct wcore_platform *wc_plat,
                  struct wcore_config *wc_config,
                  int width,
                  int height)
{
    struct glx_window *self;
    struct glx_display *dpy = glx_display(wc_config->display);
    struct glx_config *config = glx_config(wc_config);
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

    self->wcore.vtbl = &glx_window_wcore_vtbl;
    return &self->wcore;

error:
    glx_window_destroy(&self->wcore);
    return NULL;
}

static bool
glx_window_show(struct wcore_window *wc_self)
{
    return x11_window_show(&glx_window(wc_self)->x11);
}

static bool
glx_window_swap_buffers(struct wcore_window *wc_self)
{
    struct glx_window *self = glx_window(wc_self);
    struct glx_display *dpy = glx_display(wc_self->display);

    glXSwapBuffers(dpy->x11.xlib, self->x11.xcb);

    return true;
}

static union waffle_native_window*
glx_window_get_native(struct wcore_window *wc_self)
{
    struct glx_window *self = glx_window(wc_self);
    struct glx_display *dpy = glx_display(wc_self->display);
    struct waffle_glx_window *n_window;

    n_window = wcore_malloc(sizeof(*n_window));
    if (n_window == NULL)
        return NULL;

    n_window->xlib_display = dpy->x11.xlib;
    n_window->xlib_window = self->x11.xcb;

    return (union waffle_native_window*) n_window;
}

static const struct wcore_window_vtbl glx_window_wcore_vtbl = {
    .destroy = glx_window_destroy,
    .get_native = glx_window_get_native,
    .show  = glx_window_show,
    .swap_buffers = glx_window_swap_buffers,
};
