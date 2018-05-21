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

#include "wcore_attrib_list.h"
#include "wcore_error.h"
#include "wegl_config.h"
#include "wegl_surface.h"
#include "qnx_display.h"
#include "qnx_display_priv.h"
#include "qnx_window.h"
#include "qnx_window_priv.h"

struct wcore_window*
qnx_window_create(struct wcore_platform *wc_plat,
                  struct wcore_config *wc_config,
                  int32_t width,
                  int32_t height,
                  const intptr_t attrib_list[])
{
    struct qnx_display *dpy = qnx_display(wegl_display(wc_config->display));
    struct qnx_window *self;

    if (wcore_attrib_list_length(attrib_list) > 0) {
        wcore_error_bad_attribute(attrib_list[0]);
    }

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    self->priv = qnx_window_priv_create(dpy, width, height);
    if (self->priv == NULL)
        goto error;

    bool ok = wegl_window_init(&self->wegl, wc_config,
                          (intptr_t) *((intptr_t*)(&self->priv->screen_win)));
    if (!ok)
        goto error;

    return &self->wegl.wcore;

error:
    qnx_window_destroy(&self->wegl.wcore);
    return NULL;
}

bool
qnx_window_destroy(struct wcore_window *wc_self)
{
    bool ok = true;
    struct qnx_window *self = qnx_window(wegl_surface(wc_self));

    if (self == NULL)
        return ok;

    ok &= wegl_surface_teardown(&self->wegl);
    ok &= qnx_window_priv_destroy(self->priv);

    free(self);
    return ok;
}

bool
qnx_window_show(struct wcore_window *wc_self)
{
    struct qnx_window *self = qnx_window(wegl_surface(wc_self));

    if (self == NULL)
        return true;

    return qnx_window_priv_show(self->priv);
}

bool
qnx_window_resize(struct wcore_window *wc_self,
                  int32_t width,
                  int32_t height)
{
    struct qnx_window *self = qnx_window(wegl_surface(wc_self));

    if (self == NULL)
        return true;

    return qnx_window_priv_resize(self->priv, width, height);
}
