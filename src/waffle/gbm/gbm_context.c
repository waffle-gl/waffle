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

#include <waffle/core/wcore_config.h>
#include <waffle/core/wcore_error.h>

#include "gbm_config.h"
#include "gbm_context.h"
#include "gbm_display.h"
#include "gbm_priv_egl.h"

static const struct wcore_context_vtbl gbm_context_wcore_vtbl;

static bool
gbm_context_destroy(struct wcore_context *wc_self)
{
    struct gbm_context *self = gbm_context(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    if (self->egl)
        ok &= egl_destroy_context(gbm_display(wc_self->display)->egl,
                                  self->egl);

    ok &= wcore_context_teardown(wc_self);
    free(self);
    return ok;
}

struct wcore_context*
gbm_context_create(struct wcore_platform *wc_plat,
                   struct wcore_config *wc_config,
                   struct wcore_context *wc_share_ctx)
{
    struct gbm_context *self;
    struct gbm_config *config = gbm_config(wc_config);
    struct gbm_context *share_ctx = gbm_context(wc_share_ctx);
    struct gbm_display *dpy = gbm_display(wc_config->display);
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_context_init(&self->wcore, wc_config);
    if (!ok)
        goto error;

    self->egl = egl_create_context(dpy->egl,
                                   config->egl,
                                   share_ctx
                                      ? share_ctx->egl
                                      : NULL,
                                   config->waffle_context_api);
    if (!self->egl)
        goto error;

    self->wcore.vtbl = &gbm_context_wcore_vtbl;
    return &self->wcore;

error:
    gbm_context_destroy(&self->wcore);
    return NULL;
}

static union waffle_native_context*
gbm_context_get_native(struct wcore_context *wc_self)
{
    struct gbm_context *self = gbm_context(wc_self);
    struct gbm_display *dpy = gbm_display(wc_self->display);
    union waffle_native_context *n_ctx;

    WCORE_CREATE_NATIVE_UNION(n_ctx, gbm);
    if (n_ctx == NULL)
        return NULL;

    gbm_display_fill_native(dpy, &n_ctx->gbm->display);
    n_ctx->gbm->egl_context = self->egl;

    return n_ctx;
}

static const struct wcore_context_vtbl gbm_context_wcore_vtbl = {
    .destroy = gbm_context_destroy,
    .get_native = gbm_context_get_native,
};
