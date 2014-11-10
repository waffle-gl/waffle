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

#include <xcb/xcb.h>

#include "wcore_error.h"

#include "wegl_config.h"
#include "wegl_platform.h"
#include "wegl_util.h"

#include "xegl_display.h"
#include "xegl_window.h"

bool
xegl_window_destroy(struct wcore_window *wc_self)
{
    struct xegl_window *self = xegl_window(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    ok &= wegl_window_teardown(&self->wegl);
    ok &= x11_window_teardown(&self->x11);
    free(self);
    return ok;
}

struct wcore_window*
xegl_window_create(struct wcore_platform *wc_plat,
                   struct wcore_config *wc_config,
                   int width,
                   int height)
{
    struct xegl_window *self;
    struct xegl_display *dpy = xegl_display(wc_config->display);
    struct wegl_config *config = wegl_config(wc_config);
    struct wegl_platform *plat = wegl_platform(wc_plat);
    xcb_visualid_t visual;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = plat->eglGetConfigAttrib(dpy->wegl.egl,
                                  config->egl,
                                  EGL_NATIVE_VISUAL_ID,
                                  (EGLint*) &visual);
    if (!ok) {
        wegl_emit_error(plat, "eglGetConfigAttrib(EGL_NATIVE_VISUAL_ID)");
        goto error;
    }

    ok = x11_window_init(&self->x11,
                         &dpy->x11,
                         visual,
                         width,
                         height);
    if (!ok)
        goto error;

    ok = wegl_window_init(&self->wegl,
                          &config->wcore,
                          (intptr_t) self->x11.xcb);
    if (!ok)
        goto error;

    return &self->wegl.wcore;

error:
    xegl_window_destroy(&self->wegl.wcore);
    return NULL;
}

bool
xegl_window_show(struct wcore_window *wc_self)
{
    return x11_window_show(&xegl_window(wc_self)->x11);
}

bool
xegl_window_resize(struct wcore_window *wc_self,
                   int32_t width, int32_t height)
{
   return x11_window_resize(&xegl_window(wc_self)->x11, width, height);
}

union waffle_native_window*
xegl_window_get_native(struct wcore_window *wc_self)
{
    struct xegl_window *self = xegl_window(wc_self);
    struct xegl_display *dpy = xegl_display(wc_self->display);
    union waffle_native_window *n_window;

    WCORE_CREATE_NATIVE_UNION(n_window, x11_egl);
    if (!n_window)
        return NULL;

    xegl_display_fill_native(dpy, &n_window->x11_egl->display);
    n_window->x11_egl->xlib_window = self->x11.xcb;
    n_window->x11_egl->egl_surface = self->wegl.egl;;

    return n_window;
}
