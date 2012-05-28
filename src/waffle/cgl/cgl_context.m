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
