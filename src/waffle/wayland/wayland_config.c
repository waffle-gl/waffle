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

#include <stdlib.h>
#include <string.h>

#include "waffle/core/wcore_config_attrs.h"
#include "waffle/core/wcore_error.h"

#include "wayland_config.h"
#include "wayland_display.h"
#include "wayland_platform.h"
#include "wayland_priv_egl.h"

static const struct wcore_config_vtbl wayland_config_wcore_vtbl;

static bool
wayland_config_destroy(struct wcore_config *wc_self)
{
    struct wayland_config *self = wayland_config(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    ok &= wcore_config_teardown(wc_self);
    free(self);
    return ok;
}

struct wcore_config*
wayland_config_choose(struct wcore_platform *wc_plat,
                   struct wcore_display *wc_dpy,
                   const struct wcore_config_attrs *attrs)
{
    struct wayland_config *self;
    struct wayland_display *dpy = wayland_display(wc_dpy);
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_config_init(&self->wcore, wc_dpy);
    if (!ok)
        goto error;

    ok = egl_get_render_buffer_attrib(attrs, &self->egl_render_buffer);
    if (!ok)
        goto error;

    self->egl = egl_choose_config(wc_plat, dpy->egl, attrs);
    if (!self->egl)
        goto error;

    self->waffle_context_api = attrs->context_api;
    self->wcore.vtbl = &wayland_config_wcore_vtbl;
    return &self->wcore;

error:
    wayland_config_destroy(&self->wcore);
    return NULL;
}

static union waffle_native_config*
wayland_config_get_native(struct wcore_config *wc_self)
{
    struct wayland_config *self = wayland_config(wc_self);
    struct wayland_display *dpy = wayland_display(wc_self->display);
    struct waffle_wayland_config *n_config;

    n_config = wcore_malloc(sizeof(*n_config));
    if (n_config == NULL)
        return NULL;

    wayland_display_fill_native(dpy, &n_config->display);
    n_config->egl_config = self->egl;

    return (union waffle_native_config*) n_config;
}

static const struct wcore_config_vtbl wayland_config_wcore_vtbl = {
    .destroy = wayland_config_destroy,
    .get_native = wayland_config_get_native,
};
