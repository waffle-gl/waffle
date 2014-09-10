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
#define _POSIX_C_SOURCE 200112 // glib feature macro for unsetenv()

#include <stdlib.h>

#include "waffle_wayland.h"

#include "wcore_error.h"

#include "linux_platform.h"

#include "wegl_config.h"
#include "wegl_context.h"
#include "wegl_platform.h"
#include "wegl_util.h"

#include "wayland_display.h"
#include "wayland_platform.h"
#include "wayland_window.h"

static const struct wcore_platform_vtbl wayland_platform_vtbl;

static bool
wayland_platform_destroy(struct wcore_platform *wc_self)
{
    struct wayland_platform *self = wayland_platform(wegl_platform(wc_self));
    bool ok = true;

    if (!self)
        return true;

    unsetenv("EGL_PLATFORM");

    if (self->linux)
        ok &= linux_platform_destroy(self->linux);

    ok &= wegl_platform_teardown(&self->wegl);
    free(self);
    return ok;
}

struct wcore_platform*
wayland_platform_create(void)
{
    struct wayland_platform *self;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wegl_platform_init(&self->wegl);
    if (!ok)
        goto error;

    self->linux = linux_platform_create();
    if (!self->linux)
        goto error;

    setenv("EGL_PLATFORM", "wayland", true);

    self->wegl.wcore.vtbl = &wayland_platform_vtbl;
    return &self->wegl.wcore;

error:
    wayland_platform_destroy(&self->wegl.wcore);
    return NULL;
}

static bool
wayland_dl_can_open(struct wcore_platform *wc_self,
                             int32_t waffle_dl)
{
    struct wayland_platform *self = wayland_platform(wegl_platform(wc_self));
    return linux_platform_dl_can_open(self->linux, waffle_dl);
}

static void*
wayland_dl_sym(struct wcore_platform *wc_self,
                        int32_t waffle_dl,
                        const char *name)
{
    struct wayland_platform *self = wayland_platform(wegl_platform(wc_self));
    return linux_platform_dl_sym(self->linux, waffle_dl, name);
}

static union waffle_native_config*
wayland_config_get_native(struct wcore_config *wc_config)
{
    struct wegl_config *config = wegl_config(wc_config);
    struct wayland_display *dpy = wayland_display(wc_config->display);
    union waffle_native_config *n_config;

    WCORE_CREATE_NATIVE_UNION(n_config, wayland);
    if (!n_config)
        return NULL;

    wayland_display_fill_native(dpy, &n_config->wayland->display);
    n_config->wayland->egl_config = config->egl;

    return n_config;
}

static union waffle_native_context*
wayland_context_get_native(struct wcore_context *wc_ctx)
{
    struct wayland_display *dpy = wayland_display(wc_ctx->display);
    struct wegl_context *ctx = wegl_context(wc_ctx);
    union waffle_native_context *n_ctx;

    WCORE_CREATE_NATIVE_UNION(n_ctx, wayland);
    if (!n_ctx)
        return NULL;

    wayland_display_fill_native(dpy, &n_ctx->wayland->display);
    n_ctx->wayland->egl_context = ctx->egl;

    return n_ctx;
}

static const struct wcore_platform_vtbl wayland_platform_vtbl = {
    .destroy = wayland_platform_destroy,

    .make_current = wegl_make_current,
    .get_proc_address = wegl_get_proc_address,
    .dl_can_open = wayland_dl_can_open,
    .dl_sym = wayland_dl_sym,

    .display = {
        .connect = wayland_display_connect,
        .destroy = wayland_display_destroy,
        .supports_context_api = wegl_display_supports_context_api,
        .get_native = wayland_display_get_native,
    },

    .config = {
        .choose = wegl_config_choose,
        .destroy = wegl_config_destroy,
        .get_native = wayland_config_get_native,
    },

    .context = {
        .create = wegl_context_create,
        .destroy = wegl_context_destroy,
        .get_native = wayland_context_get_native,
    },

    .window = {
        .create = wayland_window_create,
        .destroy = wayland_window_destroy,
        .show = wayland_window_show,
        .swap_buffers = wayland_window_swap_buffers,
        .resize = wayland_window_resize,
        .get_native = wayland_window_get_native,
    },
};
