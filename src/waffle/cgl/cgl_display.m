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


#include <stdlib.h>

#include "wcore_error.h"

#include "cgl_display.h"

bool
cgl_display_destroy(struct wcore_display *wc_self)
{
    struct cgl_display *self = cgl_display(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    ok &= wcore_display_teardown(&self->wcore);
    free(self);
    return ok;
}


struct wcore_display*
cgl_display_connect(struct wcore_platform *wc_plat,
                    const char *name)
{
    struct cgl_display *self;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (!self)
        return NULL;

    ok = wcore_display_init(&self->wcore, wc_plat);
    if (!ok)
        goto error;

    return &self->wcore;

error:
    cgl_display_destroy(&self->wcore);
    return NULL;
}

bool
cgl_display_supports_context_api(struct wcore_display *wc_self,
                                 int32_t context_api)
{
    switch (context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            return true;
        case WAFFLE_CONTEXT_OPENGL_ES1:
            return false;
        case WAFFLE_CONTEXT_OPENGL_ES2:
            return false;
        case WAFFLE_CONTEXT_OPENGL_ES3:
            return false;
        default:
            wcore_error_internal("waffle_context_api has bad value %#x",
                                 context_api);
            return false;
    }
}
