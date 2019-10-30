// Copyright 2016 Google
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
#include "wcore_tinfo.h"

#include "wegl_config.h"
#include "wegl_util.h"

#include "sl_display.h"
#include "sl_platform.h"
#include "sl_window.h"

bool
sl_window_destroy(struct wcore_window *wc_self)
{
    struct sl_window *self = sl_window(wegl_surface(wc_self));
    bool ok = true;

    if (!self)
        return true;

    ok &= wegl_surface_teardown(&self->wegl);
    free(self);
    return ok;
}

struct wcore_window*
sl_window_create(struct wcore_platform *wc_plat,
                   struct wcore_config *wc_config,
                   int32_t width,
                   int32_t height,
                   const intptr_t attrib_list[])
{
    struct sl_window *self;
    bool ok = true;

    if (width == -1 && height == -1) {
        wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                     "fullscreen window not supported");
        return NULL;
    }

    if (wcore_attrib_list_length(attrib_list) > 0) {
        wcore_error_bad_attribute(attrib_list[0]);
        return NULL;
    }

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wegl_pbuffer_init(&self->wegl, wc_config, width, height);
    if (!ok)
        goto error;

    self->wc_config = wc_config;

    return &self->wegl.wcore;

error:
    sl_window_destroy(&self->wegl.wcore);
    return NULL;
}

bool
sl_window_show(struct wcore_window *wc_self)
{
    return true;
}

bool
sl_window_resize(struct wcore_window *wc_self,
                 int32_t width, int32_t height)
{
    struct wcore_display *wc_dpy = wc_self->display;
    struct wcore_platform *wc_plat = wc_self->display->platform;
    struct sl_window *self = sl_window(wegl_surface(wc_self));
    struct wegl_surface new_wegl;
    struct wcore_context *wc_ctx;
    struct wcore_tinfo *tinfo;
    bool ok = true;

    // Create a new pbuffer for the resized window.
    ok = wegl_pbuffer_init(&new_wegl, self->wc_config, width, height);
    if (!ok)
        return false;

    tinfo = wcore_tinfo_get();
    wc_ctx = tinfo->current_context;

    ok = wegl_make_current(wc_plat, wc_dpy, &new_wegl.wcore, wc_ctx);
    if (!ok)
        goto error;

    // Everything went fine, so teardown the old pbuffer, and set the new one.
    wegl_surface_teardown(&self->wegl);
    self->wegl = new_wegl;
    return true;

error:
    wegl_surface_teardown(&new_wegl);
    return false;
}
