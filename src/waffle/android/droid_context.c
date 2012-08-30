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

#include "droid_context.h"

#include <stdlib.h>

#include "waffle/core/wcore_error.h"

#include "droid_display.h"
#include "droid_config.h"
#include "droid_priv_egl.h"
#include "droid_surfaceflingerlink.h"

static bool
droid_context_destroy(struct wcore_context *wc_self);

static const struct wcore_context_vtbl droid_context_wcore_vtbl;

struct wcore_context*
droid_context_create(struct wcore_platform *wc_plat,
                     struct wcore_config *wc_config,
                     struct wcore_context *wc_share_ctx)
{
    struct droid_context *self;
    struct droid_config *config = droid_config(wc_config);
    struct droid_context *share_ctx = droid_context(wc_share_ctx);
    struct droid_display *dpy = droid_display(wc_config->display);
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

    self->wcore.vtbl = &droid_context_wcore_vtbl;
    return &self->wcore;

error:
    droid_context_destroy(&self->wcore);
    return NULL;
}

static bool
droid_context_destroy(struct wcore_context *wc_self)
{
    struct droid_context *self = droid_context(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    if (self->egl)
        ok &= egl_destroy_context(
            droid_display(wc_self->display)->egl, self->egl);

    ok &= wcore_context_teardown(wc_self);
    free(self);
    return ok;
}

static union waffle_native_context*
droid_context_get_native(struct wcore_context *wc_self)
{
    wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                 "droid_context_get_native not supported");
    return (union waffle_native_context*) NULL;
}

static const struct wcore_context_vtbl droid_context_wcore_vtbl = {
    .destroy = droid_context_destroy,
    .get_native = droid_context_get_native,
};
