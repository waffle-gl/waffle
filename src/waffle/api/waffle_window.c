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

#include "api_priv.h"

#include "wcore_config.h"
#include "wcore_error.h"
#include "wcore_platform.h"
#include "wcore_window.h"

WAFFLE_API struct waffle_window*
waffle_window_create(
        struct waffle_config *config,
        int width, int height)
{
    struct wcore_window *wc_self;
    struct wcore_config *wc_config = wcore_config(config);

    const struct api_object *obj_list[] = {
        wc_config ? &wc_config->api : NULL,
    };

    if (!api_check_entry(obj_list, 1))
        return NULL;

    wc_self = api_platform->vtbl->window.create(api_platform,
                                                wc_config,
                                                width,
                                                height);
    if (!wc_self)
        return NULL;

    return waffle_window(wc_self);
}

WAFFLE_API bool
waffle_window_destroy(struct waffle_window *self)
{
    struct wcore_window *wc_self = wcore_window(self);

    const struct api_object *obj_list[] = {
        wc_self ? &wc_self->api : NULL,
    };

    if (!api_check_entry(obj_list, 1))
        return false;

    return api_platform->vtbl->window.destroy(wc_self);
}

WAFFLE_API bool
waffle_window_show(struct waffle_window *self)
{
    struct wcore_window *wc_self = wcore_window(self);

    const struct api_object *obj_list[] = {
        wc_self ? &wc_self->api : NULL,
    };

    if (!api_check_entry(obj_list, 1))
        return false;

    return api_platform->vtbl->window.show(wc_self);
}

WAFFLE_API bool
waffle_window_resize(
		struct waffle_window *self,
		int32_t width,
		int32_t height)
{
    struct wcore_window *wc_self = wcore_window(self);

    const struct api_object *obj_list[] = {
        wc_self ? &wc_self->api : NULL,
    };

    if (!api_check_entry(obj_list, 1))
        return false;

    if (api_platform->vtbl->window.resize) {
        return api_platform->vtbl->window.resize(wc_self, width, height);
    }
    else {
        wcore_error(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
        return false;
    }
}

WAFFLE_API bool
waffle_window_swap_buffers(struct waffle_window *self)
{
    struct wcore_window *wc_self = wcore_window(self);

    const struct api_object *obj_list[] = {
        wc_self ? &wc_self->api : NULL,
    };

    if (!api_check_entry(obj_list, 1))
        return false;

    return api_platform->vtbl->window.swap_buffers(wc_self);
}

WAFFLE_API union waffle_native_window*
waffle_window_get_native(struct waffle_window *self)
{
    struct wcore_window *wc_self = wcore_window(self);

    const struct api_object *obj_list[] = {
        wc_self ? &wc_self->api : NULL,
    };

    if (!api_check_entry(obj_list, 1))
        return NULL;

    if (api_platform->vtbl->window.get_native) {
        return api_platform->vtbl->window.get_native(wc_self);
    }
    else {
        wcore_error(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
        return NULL;
    }
}
