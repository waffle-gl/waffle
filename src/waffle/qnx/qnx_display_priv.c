// Copyright 2014 Intel Corporation
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

#include <stdbool.h>
#include <stdint.h>
#include "wcore_util.h"
#include "wcore_error.h"
#include "qnx_display_priv.h"

struct qnx_display_priv*
qnx_display_priv_create(void)
{
    int ndisplays = 0;
    int rc;
    struct qnx_display_priv *self;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    rc = screen_create_context(&self->screen_ctx, SCREEN_APPLICATION_CONTEXT);
    if (rc != 0) {
        wcore_error_errno("screen_create_context failed");
        goto error;
    }

    rc = screen_get_context_property_iv(self->screen_ctx,
                                        SCREEN_PROPERTY_DISPLAY_COUNT,
                                        &ndisplays
                                       );
    if (rc != 0) {
        wcore_error_errno("SCREEN_PROPERTY_DISPLAY_COUNT failed");
        goto error;
    }

    return self;
error:
    qnx_display_priv_destroy(self);
    return NULL;
}

bool
qnx_display_priv_destroy(struct qnx_display_priv* self)
{
    int rc = 0;

    if (!self)
        return true;

    rc = screen_destroy_context(self->screen_ctx);
    if (rc != 0) {
        wcore_error_errno("screen_destroy_context failed");
    }

    free(self);
    return (rc == 0);
}
