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

/// @addtogroup xegl_context
/// @{

/// @file

#include "xegl_context.h"

#include <stdlib.h>

#include <waffle/native.h>
#include <waffle/core/wcore_error.h>

#include "xegl_priv_egl.h"
#include "xegl_priv_types.h"

union native_context*
xegl_context_create(
        union native_config *config,
        union native_context *share_ctx)
{
    union native_display *dpy = config->xegl->display;
    union native_platform *platform = dpy->xegl->platform;

    union native_context *self;
    NATIVE_ALLOC(self, xegl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->xegl->display = config->xegl->display;
    self->xegl->egl_context = egl_create_context(
                                  dpy->xegl->egl_display,
                                  config->xegl->egl_config,
                                  share_ctx
                                      ? share_ctx->xegl->egl_context
                                      : NULL,
                                  platform->xegl->gl_api);

    if (!self->xegl->egl_context) {
        free(self);
        return NULL;
    }

    return self;
}

bool
xegl_context_destroy(union native_context *self)
{
    if (!self)
        return true;

    bool ok = true;
    union native_display *dpy = self->xegl->display;

    if (self->xegl->egl_context)
        ok &= egl_destroy_context(dpy->xegl->egl_display,
                                  self->xegl->egl_context);
    free(self);
    return ok;
}

/// @}
