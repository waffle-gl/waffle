// Copyright 2014 Intel Corporation
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
#include <stdio.h>

#include "nacl_dl.h"
#include "nacl_platform.h"

static const struct wcore_platform_vtbl nacl_platform_vtbl;

static bool
nacl_platform_destroy(struct wcore_platform *wc_self)
{
    struct nacl_platform *self = nacl_platform(wc_self);
    bool ok = true;

    if (!self)
        return true;

    ok &= wcore_platform_teardown(wc_self);

    nacl_container_teardown(self->nacl);

    if (self->dl_gl)
        ok &= nacl_dl_close(&self->wcore);

    free(self);
    return ok;
}

static bool
nacl_platform_make_current(struct wcore_platform *wc_self,
                           struct wcore_display *wc_dpy,
                           struct wcore_window *wc_window,
                           struct wcore_context *wc_ctx)
{
    bool release = (!wc_window && !wc_ctx);
    return nacl_container_context_makecurrent(nacl_platform(wc_self)->nacl,
                            release);
}

struct wcore_platform*
nacl_platform_create(void)
{
    struct nacl_platform *self;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_platform_init(&self->wcore);
    if (!ok)
        goto error;

    self->nacl = nacl_container_init();
    if (!self->nacl)
        goto error;

    self->wcore.vtbl = &nacl_platform_vtbl;
    return &self->wcore;

error:
    nacl_platform_destroy(&self->wcore);
    return NULL;
}

static const struct wcore_platform_vtbl nacl_platform_vtbl = {
    .destroy = nacl_platform_destroy,

    .make_current = nacl_platform_make_current,
    .dl_can_open = nacl_dl_can_open,
    .dl_sym = nacl_dl_sym,

    .display = {
        .connect = nacl_display_connect,
        .destroy = nacl_display_destroy,
        .supports_context_api = nacl_display_supports_context_api,
        .get_native = NULL,
    },

    .config = {
        .choose = nacl_config_choose,
        .destroy = nacl_config_destroy,
        .get_native = NULL,
    },

    .context = {
        .create = nacl_context_create,
        .destroy = nacl_context_destroy,
        .get_native = NULL,
    },

    .window = {
        .create = nacl_window_create,
        .destroy = nacl_window_destroy,
        .show = nacl_window_show,
        .swap_buffers = nacl_window_swap_buffers,
        .resize = nacl_window_resize,
        .get_native = NULL,
    },
};
