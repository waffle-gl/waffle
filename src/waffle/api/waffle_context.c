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
