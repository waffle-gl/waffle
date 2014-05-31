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

#include <assert.h>
#include <stdlib.h>

#include "wcore_error.h"

#include "cgl_config.h"
#include "cgl_context.h"
#include "cgl_error.h"

bool
cgl_context_destroy(struct wcore_context *wc_self)
{
    struct cgl_context *self = cgl_context(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    [self->ns release];
    ok &= wcore_context_teardown(wc_self);
    free(self);
    return ok;
}

struct wcore_context*
cgl_context_create(struct wcore_platform *wc_plat,
                   struct wcore_config *wc_config,
                   struct wcore_context *wc_share_ctx)
{
    struct cgl_context *self;
    struct cgl_config *config = cgl_config(wc_config);
    struct cgl_context *share_ctx = cgl_context(wc_share_ctx);

    CGLContextObj cgl_self = NULL;
    CGLContextObj cgl_share_ctx = NULL;

    int error = 0;

    self = wcore_calloc(sizeof(*self));
    if (!self)
        return NULL;

    error = !wcore_context_init(&self->wcore, wc_config);
    if (error)
        goto fail;

    if (share_ctx)
        cgl_share_ctx = [share_ctx->ns CGLContextObj];

    error = CGLCreateContext(config->pixel_format, cgl_share_ctx, &cgl_self);
    if (error) {
        cgl_error_failed_func("CGLCreateContext", error);
        goto fail;
    }

    self->ns = [[NSOpenGLContext alloc] initWithCGLContextObj:cgl_self];
    if (!self->ns) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "NSOpenGLContext.initWithCGLContextObj failed");
        goto fail;
    }

    // The NSOpenGLContext now owns the CGLContext.
    CGLReleaseContext(cgl_self);

    return &self->wcore;

fail:
    cgl_context_destroy(&self->wcore);
    return NULL;
}
