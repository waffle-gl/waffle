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

/// @addtogroup waffle_display
/// @{

/// @file

#include <waffle/waffle_display.h>

#include <stdlib.h>

#include <waffle/native.h>
#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_error.h>
#include <waffle/core/wcore_platform.h>

#include "api_priv.h"

struct waffle_display*
waffle_display_connect(const char *name)
{
    struct waffle_display *self;

    if (!api_check_entry(NULL, 0))
        return NULL;

    self = malloc(sizeof(*self));
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->api.platform_id = api_current_platform->id;
    self->api.object_id = api_new_object_id();
    self->api.display_id = self->api.object_id;

    self->native = api_current_platform->dispatch->
                    display_connect(api_current_platform->native, name);

    if (!self->native) {
        free(self);
        return NULL;
    }

    return self;
}

bool
waffle_display_disconnect(struct waffle_display *self)
{
    bool ok = true;

    const struct api_object *obj_list[] = {
        waffle_display_cast_to_api_object(self),
    };

    if (!api_check_entry(obj_list, 1))
        return false;

    ok &= api_current_platform->dispatch->display_disconnect(self->native);
    free(self);
    return ok;
}

bool
waffle_display_supports_context_api(
        struct waffle_display *self,
        int32_t context_api)
{
    const struct api_object *obj_list[] = {
        waffle_display_cast_to_api_object(self),
    };

    if (!api_check_entry(obj_list, 1))
        return false;

    switch (context_api) {
        case WAFFLE_CONTEXT_OPENGL:
        case WAFFLE_CONTEXT_OPENGL_ES1:
        case WAFFLE_CONTEXT_OPENGL_ES2:
            break;
        default:
            wcore_errorf(WAFFLE_BAD_PARAMETER,
                         "context_api has bad value %#x", context_api);
            return false;
    }

    return api_current_platform->dispatch->
            display_supports_context_api(self->native, context_api);
}

/// @}
