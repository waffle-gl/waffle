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

#include "droid_window.h"

#include <stdlib.h>
#include <string.h>

#include <waffle/core/wcore_error.h>

#include "droid_priv_egl.h"
#include "droid_config.h"
#include "droid_display.h"
#include "droid_surfaceflingerlink.h"

static bool
droid_window_destroy(struct wcore_window *wc_self);

static const struct wcore_window_vtbl droid_window_wcore_vtbl;

struct wcore_window*
droid_window_create(struct wcore_platform *wc_plat,
                    struct wcore_config *wc_config,
                    int width,
                    int height)
{
    struct droid_window *self;
    struct droid_config *config = droid_config(wc_config);
    struct droid_display *dpy = droid_display(wc_config->display);
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_window_init(&self->wcore, wc_config);
    if (!ok)
        goto error;

    self->egl = droid_create_surface(
        width,
        height,
        config->egl,
        dpy->egl,
        dpy->pSFContainer,
        &self->pANWContainer);

    if (!self->egl)
        goto error;

    self->wcore.vtbl = &droid_window_wcore_vtbl;
    return &self->wcore;

error:
    droid_window_destroy(&self->wcore);
    return NULL;
}

static bool
droid_window_destroy(struct wcore_window *wc_self)
{
    struct droid_window *self = droid_window(wc_self);
    struct droid_display *dpy;
    bool ok = true;

    if (!self)
        return ok;

    dpy = droid_display(wc_self->display);

    if (self->egl)
        ok &= egl_destroy_surface(dpy->egl, self->egl);

    droid_destroy_surface(dpy->pSFContainer, self->pANWContainer);

    ok &= wcore_window_teardown(&self->wcore);
    free(self);
    return ok;
}

static bool
droid_window_show(struct wcore_window *wc_self)
{
    struct droid_window *self = droid_window(wc_self);
    struct droid_display *dpy;

    if (!self)
        return false;

    dpy = droid_display(wc_self->display);

    return droid_show_surface(dpy->pSFContainer, self->pANWContainer);
}

static union waffle_native_window*
droid_window_get_native(struct wcore_window *wc_self)
{
    wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                 "droid_window_get_native not supported");
    return (union waffle_native_window*) NULL;
}


static bool
droid_window_swap_buffers(struct wcore_window *wc_self)
{
    struct droid_window *self = droid_window(wc_self);
    struct droid_display *dpy = droid_display(wc_self->display);

    return egl_swap_buffers(dpy->egl, self->egl);
}

static const struct wcore_window_vtbl droid_window_wcore_vtbl = {
    .destroy = droid_window_destroy,
    .get_native = droid_window_get_native,
    .show = droid_window_show,
    .swap_buffers = droid_window_swap_buffers,
};
