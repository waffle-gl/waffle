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

#include "waffle_gbm.h"

#include "wcore_error.h"

#include "wegl_config.h"

#include "wgbm_config.h"
#include "wgbm_display.h"
#include "wgbm_window.h"

bool
wgbm_window_destroy(struct wcore_window *wc_self)
{
    struct wgbm_window *self = wgbm_window(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    ok &= wegl_window_teardown(&self->wegl);
    gbm_surface_destroy(self->gbm_surface);
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
    struct wgbm_display *dpy = wgbm_display(wc_config->display);
    uint32_t gbm_format;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    gbm_format = wgbm_config_get_gbm_format(&wc_config->attrs);
    assert(gbm_format != 0);
    self->gbm_surface = gbm_surface_create(dpy->gbm_device, width, height,
                                           gbm_format,
                                           GBM_BO_USE_RENDERING);
    if (!self->gbm_surface) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "gbm_surface_create failed");
        goto error;
    }

    ok = wegl_window_init(&self->wegl, wc_config,
                          (intptr_t) self->gbm_surface);
    if (!ok)
        goto error;

    return &self->wegl.wcore;

error:
    wgbm_window_destroy(&self->wegl.wcore);
    return NULL;
}


bool
wgbm_window_show(struct wcore_window *wc_self)
{
    return true;
}


bool
wgbm_window_swap_buffers(struct wcore_window *wc_self)
{
    if (!wegl_window_swap_buffers(wc_self))
        return false;

    struct wgbm_window *self = wgbm_window(wc_self);
    struct gbm_bo *bo = gbm_surface_lock_front_buffer(self->gbm_surface);
    if (!bo)
        return false;

    gbm_surface_release_buffer(self->gbm_surface, bo);
    return true;
}


union waffle_native_window*
wgbm_window_get_native(struct wcore_window *wc_self)
{
    struct wgbm_window *self = wgbm_window(wc_self);
    struct wgbm_display *dpy = wgbm_display(wc_self->display);
    union waffle_native_window *n_window;

    WCORE_CREATE_NATIVE_UNION(n_window, gbm);
    if (n_window == NULL)
        return NULL;

    wgbm_display_fill_native(dpy, &n_window->gbm->display);
    n_window->gbm->egl_surface = self->wegl.egl;
    n_window->gbm->gbm_surface = self->gbm_surface;

    return n_window;
}
