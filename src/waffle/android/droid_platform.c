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

#include "droid_platform.h"

#include <dlfcn.h>
#include <stdlib.h>

#include "waffle/core/wcore_error.h"
#include "waffle/linux/linux_platform.h"

#include "droid_config.h"
#include "droid_context.h"
#include "droid_display.h"
#include "droid_priv_egl.h"
#include "droid_window.h"

static const struct wcore_platform_vtbl droid_platform_vtbl;

static bool
droid_platform_destroy(struct wcore_platform *wc_self)
{
    struct droid_platform *self = droid_platform(wc_self);
    bool ok = true;

    if (!self)
        return true;

    if (self->linux)
        ok &= linux_platform_destroy(self->linux);

    ok &= wcore_platform_teardown(wc_self);
    free(self);
    return ok;
}

struct wcore_platform*
droid_platform_create(void)
{
    struct droid_platform *self;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_platform_init(&self->wcore);
    if (!ok)
        goto error;

    self->linux = linux_platform_create();
    if (!self->linux)
        goto error;

    self->wcore.vtbl = &droid_platform_vtbl;
    return &self->wcore;

error:
    droid_platform_destroy(&self->wcore);
    return NULL;
}

static bool
droid_make_current(struct wcore_platform *wc_self,
		   struct wcore_display *wc_dpy,
		   struct wcore_window *wc_window,
		   struct wcore_context *wc_ctx)
{
    return egl_make_current(droid_display(wc_dpy)->egl,
                            wc_window ? droid_window(wc_window)->egl : NULL,
                            wc_ctx ? droid_context(wc_ctx)->egl : NULL);
}

static void*
droid_get_proc_address(
        struct wcore_platform *wc_self,
        const char *name)
{
    return eglGetProcAddress(name);
}

static bool
droid_dl_can_open(
        struct wcore_platform *wc_self,
        int32_t waffle_dl)
{
    return linux_platform_dl_can_open(droid_platform(wc_self)->linux,
                                      waffle_dl);
}

static void*
droid_dl_sym(
        struct wcore_platform *wc_self,
        int32_t waffle_dl,
        const char *name)
{
    return linux_platform_dl_sym(droid_platform(wc_self)->linux,
                                 waffle_dl, name);
}

static const struct wcore_platform_vtbl droid_platform_vtbl = {
    .destroy = droid_platform_destroy,

    .make_current = droid_make_current,
    .get_proc_address = droid_get_proc_address,
    .dl_can_open = droid_dl_can_open,
    .dl_sym = droid_dl_sym,

    .display = {
        .connect = droid_display_connect,
        .destroy = droid_display_disconnect,
        .supports_context_api = droid_display_supports_context_api,
        .get_native = droid_display_get_native,
    },

    .config = {
        .choose = droid_config_choose,
        .destroy = droid_config_destroy,
        .get_native = droid_config_get_native,
    },

    .context = {
        .create = droid_context_create,
        .destroy = droid_context_destroy,
        .get_native = droid_context_get_native,
    },

    .window = {
        .create = droid_window_create,
        .destroy = droid_window_destroy,
        .show = droid_window_show,
        .swap_buffers = droid_window_swap_buffers,
        .get_native = droid_window_get_native,
    },
};
