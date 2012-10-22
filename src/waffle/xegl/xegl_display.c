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

#include "waffle/core/wcore_error.h"
#include "waffle/core/wcore_display.h"

#include "xegl_display.h"
#include "xegl_platform.h"
#include "xegl_priv_egl.h"

static const struct wcore_display_vtbl xegl_display_wcore_vtbl;

bool
xegl_display_destroy(struct wcore_display *wc_self)
{
    struct xegl_display *self = xegl_display(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    if (self->egl)
        ok &= egl_terminate(self->egl);

    ok &= x11_display_teardown(&self->x11);
    ok &= wcore_display_teardown(&self->wcore);
    free(self);
    return ok;
}

struct wcore_display*
xegl_display_connect(
        struct wcore_platform *wc_plat,
        const char *name)
{
    struct xegl_display *self;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_display_init(&self->wcore, wc_plat);
    if (!ok)
        goto error;

    ok = x11_display_init(&self->x11, name);
    if (!ok)
        goto error;

    self->egl = xegl_egl_initialize(self->x11.xlib);
    if (!self->egl)
        goto error;

    self->wcore.vtbl = &xegl_display_wcore_vtbl;
    return &self->wcore;

error:
    xegl_display_destroy(&self->wcore);
    return NULL;
}

bool
xegl_display_supports_context_api(struct wcore_display *wc_self,
                                  int32_t waffle_context_api)
{
    return egl_supports_context_api(wc_self->platform, waffle_context_api);
}

void
xegl_display_fill_native(struct xegl_display *self,
                         struct waffle_x11_egl_display *n_dpy)
{
    n_dpy->xlib_display = self->x11.xlib;
    n_dpy->egl_display = self->egl;
}

union waffle_native_display*
xegl_display_get_native(struct wcore_display *wc_self)
{
    struct xegl_display *self = xegl_display(wc_self);
    union waffle_native_display *n_dpy;

    WCORE_CREATE_NATIVE_UNION(n_dpy, x11_egl);
    if (!n_dpy)
        return NULL;

    xegl_display_fill_native(self, n_dpy->x11_egl);
    return n_dpy;
}

static const struct wcore_display_vtbl xegl_display_wcore_vtbl = {
    .destroy = xegl_display_destroy,
    .get_native = xegl_display_get_native,
    .supports_context_api = xegl_display_supports_context_api,
};
