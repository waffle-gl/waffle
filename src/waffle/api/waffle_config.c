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

#include "api_priv.h"

#include "wcore_config_attrs.h"
#include "wcore_config.h"
#include "wcore_display.h"
#include "wcore_error.h"
#include "wcore_platform.h"

WAFFLE_API struct waffle_config*
waffle_config_choose(
        struct waffle_display *dpy,
        const int32_t attrib_list[])
{
    struct wcore_config *wc_self;
    struct wcore_display *wc_dpy = wcore_display(dpy);
    struct wcore_config_attrs attrs;
    bool ok = true;

    const struct api_object *obj_list[] = {
        wc_dpy ? &wc_dpy->api : NULL,
    };

    if (!api_check_entry(obj_list, 1))
        return NULL;

    ok = wcore_config_attrs_parse(attrib_list, &attrs);
    if (!ok)
        return NULL;

    wc_self = api_platform->vtbl->config.choose(api_platform, wc_dpy, &attrs);
    if (!wc_self)
        return NULL;

    return waffle_config(wc_self);
}

WAFFLE_API bool
waffle_config_destroy(struct waffle_config *self)
{
    struct wcore_config *wc_self = wcore_config(self);

    const struct api_object *obj_list[] = {
        wc_self ? &wc_self->api : NULL,
    };

    if (!api_check_entry(obj_list, 1))
        return false;

    return api_platform->vtbl->config.destroy(wc_self);
}

WAFFLE_API union waffle_native_config*
waffle_config_get_native(struct waffle_config *self)
{
    struct wcore_config *wc_self = wcore_config(self);

    const struct api_object *obj_list[] = {
        wc_self ? &wc_self->api : NULL,
    };

    if (!api_check_entry(obj_list, 1))
        return NULL;

    if (api_platform->vtbl->config.get_native) {
        return api_platform->vtbl->config.get_native(wc_self);
    }
    else {
        wcore_error(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
        return NULL;
    }
}
