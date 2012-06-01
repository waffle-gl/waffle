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

/// @addtogroup waffle_config
/// @{

/// @file

#include <waffle/waffle_config.h>

#include <stdlib.h>

#include <waffle/native.h>
#include <waffle/core/wcore_config_attrs.h>
#include <waffle/core/wcore_error.h>
#include <waffle/core/wcore_platform.h>

#include "api_priv.h"

struct waffle_config*
waffle_config_choose(
        struct waffle_display *dpy,
        const int32_t attrib_list[])
{
    struct waffle_config *self = NULL;
    struct wcore_config_attrs attrs;
    bool ok = true;

    const struct api_object *obj_list[] = {
        waffle_display_get_api_obj(dpy),
    };

    if (!api_check_entry(obj_list, 1))
        return NULL;

    self = malloc(sizeof(*self));
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    ok = wcore_config_attrs_parse(attrib_list, &attrs);
    if (!ok)
        goto error;

    self->api.display_id = dpy->api.display_id;

    self->native = api_current_platform->dispatch->
                        config_choose(dpy->native, &attrs);
    if (!self->native)
        goto error;

    return self;

error:
    free(self);
    return NULL;
}

bool
waffle_config_destroy(struct waffle_config *self)
{
    bool ok = true;

    const struct api_object *obj_list[] = {
        waffle_config_get_api_obj(self),
    };

    if (!api_check_entry(obj_list, 1))
        return false;

    ok &= api_current_platform->dispatch->config_destroy(self->native);
    free(self);
    return ok;
}

/// @}
