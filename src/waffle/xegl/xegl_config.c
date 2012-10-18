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

#include <stdlib.h>
#include <string.h>

#include "waffle/core/wcore_config_attrs.h"
#include "waffle/core/wcore_error.h"

#include "xegl_config.h"
#include "xegl_display.h"
#include "xegl_platform.h"
#include "xegl_priv_egl.h"

static const struct wcore_config_vtbl xegl_config_wcore_vtbl;

static bool
xegl_config_destroy(struct wcore_config *wc_self)
{
    struct xegl_config *self = xegl_config(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    ok &= wcore_config_teardown(wc_self);
    free(self);
    return ok;
}

struct wcore_config*
xegl_config_choose(struct wcore_platform *wc_plat,
                   struct wcore_display *wc_dpy,
                   const struct wcore_config_attrs *attrs)
{
    struct xegl_config *self;
    struct xegl_display *dpy = xegl_display(wc_dpy);
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

    ok = eglGetConfigAttrib(dpy->egl,
                            self->egl,
                            EGL_NATIVE_VISUAL_ID,
                            (EGLint*) &self->xcb_visual_id);
    if (!ok) {
        egl_get_error("eglGetConfigAttrib(EGL_NATIVE_VISUAL_ID)");
        goto error;
    }

    self->waffle_context_api = attrs->context_api;
    self->wcore.vtbl = &xegl_config_wcore_vtbl;
    return &self->wcore;

error:
    xegl_config_destroy(&self->wcore);
    return NULL;
}

static union waffle_native_config*
xegl_config_get_native(struct wcore_config *wc_self)
{
    struct xegl_config *self = xegl_config(wc_self);
    struct xegl_display *dpy = xegl_display(wc_self->display);
    union waffle_native_config *n_config;

    WCORE_CREATE_NATIVE_UNION(n_config, x11_egl);
    if (!n_config)
        return NULL;

    xegl_display_fill_native(dpy, &n_config->x11_egl->display);
    n_config->x11_egl->egl_config = self->egl;

    return n_config;
}

static const struct wcore_config_vtbl xegl_config_wcore_vtbl = {
    .destroy = xegl_config_destroy,
    .get_native = xegl_config_get_native,
};
