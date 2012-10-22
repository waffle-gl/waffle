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

#include "waffle/core/wcore_config_attrs.h"
#include "waffle/core/wcore_error.h"

#include "wgbm_config.h"
#include "wgbm_display.h"
#include "wgbm_platform.h"
#include "wgbm_priv_egl.h"

bool
wgbm_config_destroy(struct wcore_config *wc_self)
{
    struct wgbm_config *self = wgbm_config(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    ok &= wcore_config_teardown(wc_self);
    free(self);
    return ok;
}

static uint32_t
get_gbm_format(const struct wcore_config_attrs *attrs)
{
    if (attrs->red_size > 8 || attrs->blue_size > 8 ||
        attrs->green_size > 8 || attrs->alpha_size > 8) {
        return 0;
    }

    if (attrs->alpha_size <= 0)
        return GBM_FORMAT_XRGB8888;
    else if (attrs->alpha_size <= 8)
        return GBM_FORMAT_ABGR8888;

    return 0;
}

struct wcore_config*
wgbm_config_choose(struct wcore_platform *wc_plat,
                   struct wcore_display *wc_dpy,
                   const struct wcore_config_attrs *attrs)
{
    struct wgbm_config *self;
    struct wgbm_display *dpy = wgbm_display(wc_dpy);
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_config_init(&self->wcore, wc_dpy);
    if (!ok)
        goto error;

    self->gbm_format = get_gbm_format(attrs);
    if (self->gbm_format == 0) {
        wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                     "requested config is unsupported on GBM");
        goto error;
    }

    ok = egl_get_render_buffer_attrib(attrs, &self->egl_render_buffer);
    if (!ok)
        goto error;

    self->egl = egl_choose_config(wc_plat, dpy->egl, attrs);
    if (!self->egl)
        goto error;

    self->waffle_context_api = attrs->context_api;
    return &self->wcore;

error:
    wgbm_config_destroy(&self->wcore);
    return NULL;
}

union waffle_native_config*
wgbm_config_get_native(struct wcore_config *wc_self)
{
    struct wgbm_config *self = wgbm_config(wc_self);
    struct wgbm_display *dpy = wgbm_display(wc_self->display);
    union waffle_native_config *n_config;

    WCORE_CREATE_NATIVE_UNION(n_config, gbm);
    if (n_config == NULL)
        return NULL;

    wgbm_display_fill_native(dpy, &n_config->gbm->display);
    n_config->gbm->egl_config = self->egl;

    return n_config;
}
