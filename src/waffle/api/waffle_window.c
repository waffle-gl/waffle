// Copyright 2012 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
        waffle_config_cast_to_api_object(config),
    };

    struct waffle_window *self;

    if (!api_check_entry(obj_list, 1))
        return false;

    self = malloc(sizeof(*self));
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->api.platform_id = api_current_platform->id;
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
        waffle_window_cast_to_api_object(self),
        0,
    };

    if (!api_check_entry(obj_list, 1))
        return false;

    ok &= api_current_platform->dispatch->window_destroy(self->native);
    free(self);
    return ok;
}

bool
waffle_window_swap_buffers(struct waffle_window *self)
{
    const struct api_object *obj_list[] = {
        waffle_window_cast_to_api_object(self),
    };

    if (!api_check_entry(obj_list, 1))
        return false;

    return api_current_platform->dispatch->
            window_swap_buffers(self->native);
}
