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

#define __GBM__ 1

#include <stdlib.h>
#include <string.h>

#include <gbm.h>

#include <waffle/core/wcore_error.h>
#include <waffle_gbm.h>

#include "gbm_config.h"
#include "gbm_display.h"
#include "gbm_priv_egl.h"
#include "gbm_window.h"

static const struct wcore_window_vtbl wgbm_window_wcore_vtbl;

static bool
wgbm_window_destroy(struct wcore_window *wc_self)
{
    struct wgbm_window *self = wgbm_window(wc_self);
    struct wgbm_display *dpy;
    bool ok = true;

    if (!self)
        return ok;

    dpy = wgbm_display(wc_self->display);

    if (self->egl)
        ok &= egl_destroy_surface(dpy->egl, self->egl);

    ok &= wcore_window_teardown(wc_self);
    free(self);
    return ok;
}

struct wcore_window*
wgbm_window_create(struct wcore_platform *wc_plat,
                   struct wcore_config *wc_config,
                   int width,
                   int height)
{
    struct wgbm_window *self;
    struct wgbm_config *config = wgbm_config(wc_config);
    struct wgbm_display *dpy = wgbm_display(wc_config->display);
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_window_init(&self->wcore, wc_config);
    if (!ok)
        goto error;

    self->gbm_surface = gbm_surface_create(dpy->gbm_device, width, height,
                                           config->gbm_format,
                                           GBM_BO_USE_RENDERING);
    if (!self->gbm_surface) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "gbm_surface_create failed");
        goto error;
    }

    self->egl = wgbm_egl_create_window_surface(dpy->egl,
                                               config->egl,
                                               self->gbm_surface,
                                               config->egl_render_buffer);
    if (!self->egl)
        goto error;

    self->wcore.vtbl = &wgbm_window_wcore_vtbl;
    return &self->wcore;

error:
    wgbm_window_destroy(&self->wcore);
    return NULL;
}


static bool
wgbm_window_show(struct wcore_window *wc_self)
{
    return true;
}

static bool
wgbm_window_swap_buffers(struct wcore_window *wc_self)
{
    struct wgbm_window *self = wgbm_window(wc_self);
    struct wgbm_display *dpy = wgbm_display(wc_self->display);

    return egl_swap_buffers(dpy->egl, self->egl);
}

static union waffle_native_window*
wgbm_window_get_native(struct wcore_window *wc_self)
{
    struct wgbm_window *self = wgbm_window(wc_self);
    struct wgbm_display *dpy = wgbm_display(wc_self->display);
    union waffle_native_window *n_window;

    WCORE_CREATE_NATIVE_UNION(n_window, gbm);
    if (n_window == NULL)
        return NULL;

    wgbm_display_fill_native(dpy, &n_window->gbm->display);
    n_window->gbm->egl_surface = self->egl;
    n_window->gbm->gbm_surface = self->gbm_surface;

    return n_window;
}

static const struct wcore_window_vtbl wgbm_window_wcore_vtbl = {
    .destroy = wgbm_window_destroy,
    .get_native = wgbm_window_get_native,
    .show = wgbm_window_show,
    .swap_buffers = wgbm_window_swap_buffers,
};
