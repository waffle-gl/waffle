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
                                  config->xegl->waffle_context_api);

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
