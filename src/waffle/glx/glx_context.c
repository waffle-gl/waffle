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

#include "glx_context.h"

#include <stdlib.h>

#include <waffle/native.h>
#include <waffle/core/wcore_error.h>

#include "glx_priv_types.h"

union native_context*
glx_context_create(
        union native_config *config,
        union native_context *share_ctx)
{
    union native_display *dpy = config->glx->display;

    union native_context *self;
    NATIVE_ALLOC(self, glx);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->glx->display = config->glx->display;
    self->glx->glx_context = glXCreateNewContext(dpy->glx->xlib_display,
                                                 config->glx->glx_fbconfig,
                                                 GLX_RGBA_TYPE,
                                                 share_ctx
                                                   ? share_ctx->glx->glx_context
                                                   : NULL,
                                                 true /*direct?*/);

    if (!self->glx->glx_context) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR, "glXCreateContext failed");
        free(self);
        return NULL;
    }

    return self;
}

bool
glx_context_destroy(union native_context *self)
{
    if (!self)
        return true;

    union native_display *dpy = self->glx->display;

    if (self->glx->glx_context)
        glXDestroyContext(dpy->glx->xlib_display,
                          self->glx->glx_context);

    free(self);
    return true;
}
