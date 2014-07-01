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

#include "wcore_error.h"
#include "wcore_display.h"
#include "wcore_platform.h"
#include "wcore_util.h"

WAFFLE_API struct waffle_display*
waffle_display_connect(const char *name)
{
    struct wcore_display *wc_self;

    if (!api_check_entry(NULL, 0))
        return NULL;

    wc_self = api_platform->vtbl->display.connect(api_platform, name);
    if (!wc_self)
        return NULL;

    return waffle_display(wc_self);
}

WAFFLE_API bool
waffle_display_disconnect(struct waffle_display *self)
{
    struct wcore_display *wc_self = wcore_display(self);

    const struct api_object *obj_list[] = {
        wc_self ? &wc_self->api : NULL,
    };

    if (!api_check_entry(obj_list, 1))
        return false;

    return api_platform->vtbl->display.destroy(wc_self);
}

WAFFLE_API bool
waffle_display_supports_context_api(
        struct waffle_display *self,
        int32_t context_api)
{
    struct wcore_display *wc_self = wcore_display(self);

    const struct api_object *obj_list[] = {
        wc_self ? &wc_self->api : NULL,
    };

    if (!api_check_entry(obj_list, 1))
        return false;

    switch (context_api) {
        case WAFFLE_CONTEXT_OPENGL:
        case WAFFLE_CONTEXT_OPENGL_ES1:
        case WAFFLE_CONTEXT_OPENGL_ES2:
        case WAFFLE_CONTEXT_OPENGL_ES3:
            break;
        default:
            wcore_errorf(WAFFLE_ERROR_BAD_PARAMETER,
                         "context_api has bad value %#x", context_api);
            return false;
    }

    return api_platform->vtbl->display.supports_context_api(wc_self,
                                                            context_api);
}

WAFFLE_API union waffle_native_display*
waffle_display_get_native(struct waffle_display *self)
{
    struct wcore_display *wc_self = wcore_display(self);

    const struct api_object *obj_list[] = {
        wc_self ? &wc_self->api : NULL,
    };

    if (!api_check_entry(obj_list, 1))
        return NULL;

    if (api_platform->vtbl->display.get_native) {
        return api_platform->vtbl->display.get_native(wc_self);
    }
    else {
        wcore_error(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
        return NULL;
    }
}
