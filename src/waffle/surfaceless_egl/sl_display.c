// Copyright 2016 Google
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

#include "wcore_error.h"

#include "sl_display.h"
#include "sl_platform.h"

bool
sl_display_destroy(struct wcore_display *wc_self)
{
    struct sl_display *self = sl_display(wegl_display(wc_self));
    bool ok = true;

    if (!self)
        return ok;

    ok &= wegl_display_teardown(&self->wegl);
    free(self);
    return ok;
}

struct wcore_display*
sl_display_connect(struct wcore_platform *wc_plat, const char *name)
{
    struct sl_display *self;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    if (name != NULL) {
        wcore_errorf(WAFFLE_ERROR_BAD_PARAMETER,
                     "parameter 'name' is not NULL");
        goto fail;
    }

    ok = wegl_display_init(&self->wegl, wc_plat, NULL);
    if (!ok)
        goto fail;

    return &self->wegl.wcore;

fail:
    sl_display_destroy(&self->wegl.wcore);
    return NULL;
}
