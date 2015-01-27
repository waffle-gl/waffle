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

#include "wcore_attrib_list.h"
#include "wcore_error.h"

#include "wegl_config.h"

#include "droid_display.h"
#include "droid_surfaceflingerlink.h"
#include "droid_window.h"

struct wcore_window*
droid_window_create(struct wcore_platform *wc_plat,
                    struct wcore_config *wc_config,
                    int32_t width,
                    int32_t height,
                    const intptr_t attrib_list[])
{
    struct droid_window *self;
    struct wegl_config *config = wegl_config(wc_config);
    struct droid_display *dpy = droid_display(wc_config->display);
    bool ok = true;

    if (wcore_attrib_list_length(attrib_list) > 0) {
        wcore_error_bad_attribute(attrib_list[0]);
    }

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    self->pANWContainer = droid_create_surface(width, height,
                                               dpy->pSFContainer);
    if (!self->pANWContainer)
        goto error_droid_create_surface;

    ok = wegl_window_init(&self->wegl, wc_config,
                          (intptr_t) *((intptr_t*)(self->pANWContainer)));
    if (!ok)
        goto error;

    return &self->wegl.wcore;

error:
    droid_window_destroy(&self->wegl.wcore);
error_droid_create_surface:
    return NULL;
}

bool
droid_window_destroy(struct wcore_window *wc_self)
{
    struct droid_window *self = droid_window(wc_self);
    struct droid_display *dpy;
    bool ok = true;

    if (!self)
        return ok;

    dpy = droid_display(self->wegl.wcore.display);

    ok &= wegl_window_teardown(&self->wegl);
    droid_destroy_surface(dpy->pSFContainer, self->pANWContainer);
    free(self);
    return ok;
}

bool
droid_window_show(struct wcore_window *wc_self)
{
    struct droid_window *self = droid_window(wc_self);
    struct droid_display *dpy;

    if (!self)
        return false;

    dpy = droid_display(wc_self->display);

    return droid_show_surface(dpy->pSFContainer, self->pANWContainer);
}

bool
droid_window_resize(struct wcore_window *wc_self,
                    int32_t width,
                    int32_t height)
{
    struct droid_window *self = droid_window(wc_self);
    struct droid_display *dpy;

    if (!self)
        return false;

    dpy = droid_display(wc_self->display);

    return droid_resize_surface(dpy->pSFContainer, self->pANWContainer,
        width, height);
}
