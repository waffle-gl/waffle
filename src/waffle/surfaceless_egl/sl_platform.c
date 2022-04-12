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

#include <dlfcn.h>
#include <stdlib.h>

#include "wcore_error.h"

#include "linux_platform.h"

#include "wegl_config.h"
#include "wegl_context.h"
#include "wegl_platform.h"
#include "wegl_util.h"

#include "sl_config.h"
#include "sl_display.h"
#include "sl_platform.h"
#include "sl_window.h"

static const struct wcore_platform_vtbl sl_platform_vtbl;

static bool
sl_platform_destroy(struct wcore_platform *wc_self)
{
    struct sl_platform *self = sl_platform(wegl_platform(wc_self));
    bool ok = true;

    if (!self)
        return true;

    if (self->linux)
        ok &= linux_platform_destroy(self->linux);

    ok &= wegl_platform_teardown(&self->wegl);
    free(self);
    return ok;
}

struct wcore_platform*
sl_platform_create(void)
{
    bool ok = true;

    struct sl_platform *self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wegl_platform_init(&self->wegl, EGL_PLATFORM_SURFACELESS_MESA);
    if (!ok)
        goto fail;

    self->wegl.egl_surface_type_mask = EGL_PBUFFER_BIT;

    self->linux = linux_platform_create();
    if (!self->linux)
        goto fail;

    self->wegl.wcore.vtbl = &sl_platform_vtbl;
    return &self->wegl.wcore;

fail:
    sl_platform_destroy(&self->wegl.wcore);
    return NULL;
}

static bool
sl_dl_can_open(struct wcore_platform *wc_self, int32_t waffle_dl)
{
    struct sl_platform *self = sl_platform(wegl_platform(wc_self));
    return linux_platform_dl_can_open(self->linux, waffle_dl);
}

static void*
sl_dl_sym(struct wcore_platform *wc_self,
          int32_t waffle_dl, const char *name)
{
    struct sl_platform *self = sl_platform(wegl_platform(wc_self));
    return linux_platform_dl_sym(self->linux, waffle_dl, name);
}

static union waffle_native_context *
sl_context_get_native(struct wcore_context *wc_ctx)
{
    struct sl_display *dpy = sl_display(wegl_display(wc_ctx->display));
    struct wegl_context *ctx = wegl_context(wc_ctx);
    union waffle_native_context *n_ctx;

    WCORE_CREATE_NATIVE_UNION(n_ctx, surfaceless_egl);
    if (!n_ctx)
        return NULL;

    sl_display_fill_native(dpy, &n_ctx->surfaceless_egl->display);
    n_ctx->surfaceless_egl->egl_context = ctx->egl;

    return n_ctx;
}

static const struct wcore_platform_vtbl sl_platform_vtbl = {
    .destroy = sl_platform_destroy,

    .make_current = wegl_make_current,
    .get_proc_address = wegl_get_proc_address,

    .dl_can_open = sl_dl_can_open,
    .dl_sym = sl_dl_sym,

    .display = {
        .connect = sl_display_connect,
        .destroy = sl_display_destroy,
        .supports_context_api = wegl_display_supports_context_api,
        .get_native = sl_display_get_native,
    },

    .config = {
        .choose = wegl_config_choose,
        .destroy = wegl_config_destroy,
        .get_native = sl_config_get_native,
    },

    .context = {
        .create = wegl_context_create,
        .destroy = wegl_context_destroy,
        .get_native = sl_context_get_native,
    },

    .window = {
        .create = sl_window_create,
        .destroy = sl_window_destroy,
        .show = sl_window_show,
        .resize = sl_window_resize,
        .swap_buffers = wegl_surface_swap_buffers,
        .get_native = sl_window_get_native,
    },
};
