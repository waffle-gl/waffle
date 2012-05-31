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

/// @addtogroup cgl_context
/// @{

/// @file

#include "cgl_context.h"

#include <assert.h>
#include <stdlib.h>

#include <waffle/native.h>
#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_error.h>

#include "cgl_config.h"
#include "cgl_error.h"

union native_context*
cgl_context_create(
        union native_config *config,
        union native_context *share_ctx)
{
    int error = 0;
    CGLContextObj real_cgl_ctx = NULL;
    CGLContextObj real_cgl_share_ctx = NULL;

    union native_context *self;
    NATIVE_ALLOC(self, cgl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    if (share_ctx)
        real_cgl_share_ctx = [share_ctx->cgl->ns_context CGLContextObj];

    error = CGLCreateContext(config->cgl->pixel_format,
                             real_cgl_share_ctx,
                             &real_cgl_ctx);
    if (error) {
        cgl_error_failed_func("CGLCreateContext", error);
        goto fail;
    }

    self->cgl->ns_context = [[[NSOpenGLContext alloc]
                            initWithCGLContextObj:real_cgl_ctx]
                            autorelease];
    if (!self->cgl->ns_context) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "NSOpenGLContext.initWithCGLContextObj failed");
        goto fail;
    }

    // The NSOpenGLContext now owns the CGLContext.
    CGLReleaseContext(real_cgl_ctx);

    goto end;

fail:
    cgl_context_destroy(self);
    self = NULL;

end:
    return self;
}

bool
cgl_context_destroy(union native_context *self)
{
    if (!self)
        return true;

    [self->cgl->ns_context release];
    free(self);
    return true;
}

/// @}
