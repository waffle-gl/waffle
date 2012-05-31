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
