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

#include "droid_display.h"
#include "droid_platform.h"

#include <stdlib.h>

#include "waffle/linux/linux_platform.h"
#include "waffle/core/wcore_error.h"

#include "droid_surfaceflingerlink.h"

struct wcore_display*
droid_display_connect(struct wcore_platform *wc_plat,
                        const char *name)
{
    bool ok = true;
    struct droid_display *self;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    self->pSFContainer = droid_init_gl();

    if (self->pSFContainer == NULL)
        goto error;

    ok = wegl_display_init(&self->wegl, wc_plat,
                           (intptr_t) EGL_DEFAULT_DISPLAY);
    if (!ok)
        goto error;

    return &self->wegl.wcore;

error:
    droid_display_disconnect(&self->wegl.wcore);
    return NULL;
}

bool
droid_display_disconnect(struct wcore_display *wc_self)
{
    struct droid_display *self = droid_display(wc_self);
    bool ok = true;

    if (!self)
        return true;

    if (self->pSFContainer)
        droid_deinit_gl(self->pSFContainer);

    ok &= wegl_display_teardown(&self->wegl);
    free(self);
    return ok;
}

union waffle_native_display*
droid_display_get_native(struct wcore_display *wc_self)
{
    wcore_error(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
    return NULL;
}
