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

/// @addtogroup wayland_context
/// @{

/// @file

#include "wayland_context.h"

#include <stdlib.h>

#include <waffle/native.h>
#include <waffle/core/wcore_error.h>

#include "wayland_priv_egl.h"
#include "wayland_priv_types.h"

union native_context*
wayland_context_create(
        union native_config *config,
        union native_context *share_ctx)
{
    union native_display *dpy = config->wl->display;

    union native_context *self;
    NATIVE_ALLOC(self, wl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->wl->display = config->wl->display;
    self->wl->egl_context = egl_create_context(
                                  dpy->wl->egl_display,
                                  config->wl->egl_config,
                                  share_ctx
                                      ? share_ctx->wl->egl_context
                                      : NULL,
                                  config->wl->waffle_context_api);

    if (!self->wl->egl_context) {
        free(self);
        return NULL;
    }

    return self;
}

bool
wayland_context_destroy(union native_context *self)
{
    if (!self)
        return true;

    bool ok = true;
    union native_display *dpy = self->wl->display;

    if (self->wl->egl_context)
        ok &= egl_destroy_context(dpy->wl->egl_display,
                                  self->wl->egl_context);
    free(self);
    return ok;
}

/// @}
