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

/// @addtogroup waffle_context
/// @{

/// @file

#include <waffle/waffle_context.h>

#include <stdlib.h>

#include <waffle/native.h>
#include <waffle/core/wcore_error.h>
#include <waffle/core/wcore_platform.h>

#include "api_priv.h"

struct waffle_context*
waffle_context_create(
        struct waffle_config *config,
        struct waffle_context *shared_ctx)
{
    struct waffle_context *self;
    int obj_list_length;

    const struct api_object *obj_list[] = {
        waffle_config_cast_to_api_object(config),
        waffle_context_cast_to_api_object(shared_ctx),
    };

    if (shared_ctx)
        obj_list_length = 2;
    else
        obj_list_length = 1;

    if (!api_check_entry(obj_list, obj_list_length))
        return false;

    self = malloc(sizeof(*self));
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->api.platform_id = api_current_platform->id;
    self->api.object_id = api_new_object_id();
    self->api.display_id = config->api.display_id;

    self->native = api_current_platform->dispatch->
                        context_create(config->native,
                                       shared_ctx ? shared_ctx->native : NULL);
    if (!self->native) {
        free(self);
        return NULL;
    }

    return self;
}

bool
waffle_context_destroy(struct waffle_context *self)
{
    bool ok = true;

    const struct api_object *obj_list[] = {
        waffle_context_cast_to_api_object(self),
    };

    if (!api_check_entry(obj_list, 1))
        return false;

    ok &= api_current_platform->dispatch->context_destroy(self->native);
    free(self);
    return ok;
}

/// @}
