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

/// @addtogroup waffle_window
/// @{

/// @file

#include <waffle/waffle_window.h>

#include <stdlib.h>

#include <waffle/native.h>
#include <waffle/core/wcore_error.h>
#include <waffle/core/wcore_platform.h>

#include "api_priv.h"

struct waffle_window*
waffle_window_create(
        struct waffle_config *config,
        int width, int height)
{
    const struct api_object *obj_list[] = {
        waffle_config_get_api_obj(config),
    };

    struct waffle_window *self;

    if (!api_check_entry(obj_list, 1))
        return false;

    self = malloc(sizeof(*self));
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->api.display_id = config->api.display_id;

    self->native = api_current_platform->dispatch->
                        window_create(config->native, width, height);

    if (!self->native) {
        free(self);
        return NULL;
    }

    return self;
}

bool
waffle_window_destroy(struct waffle_window *self)
{
    bool ok = true;

    const struct api_object *obj_list[] = {
        waffle_window_get_api_obj(self),
        0,
    };

    if (!api_check_entry(obj_list, 1))
        return false;

    ok &= api_current_platform->dispatch->window_destroy(self->native);
    free(self);
    return ok;
}

WAFFLE_API bool
waffle_window_show(struct waffle_window *self)
{
    const struct api_object *obj_list[] = {
        waffle_window_get_api_obj(self),
    };

    if (!api_check_entry(obj_list, 1))
        return false;

    return api_current_platform->dispatch->
                window_show(self->native);
}

bool
waffle_window_swap_buffers(struct waffle_window *self)
{
    const struct api_object *obj_list[] = {
        waffle_window_get_api_obj(self),
    };

    if (!api_check_entry(obj_list, 1))
        return false;

    return api_current_platform->dispatch->
            window_swap_buffers(self->native);
}

/// @}
