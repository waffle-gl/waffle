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

#include "linux_platform.h"
#include "qnx_platform.h"
#include "wegl_platform.h"
#include "qnx_display.h"
#include "qnx_display_priv.h"

struct wcore_display*
qnx_display_connect(struct wcore_platform *wc_plat,
                    const char *name)
{
    bool ok = true;
    struct qnx_display *self;

    (void) name;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    self->priv = qnx_display_priv_create();
    if (self->priv == NULL)
        goto error;

    ok = wegl_display_init(&self->wegl, wc_plat, EGL_DEFAULT_DISPLAY);
    if (!ok)
        goto error;

    return &self->wegl.wcore;

error:
    qnx_display_disconnect(&self->wegl.wcore);
    return NULL;
}

bool
qnx_display_disconnect(struct wcore_display *wc_self)
{
    bool ok = true;
    struct qnx_display *self = qnx_display(wegl_display(wc_self));

    if (!self)
        return ok;

    qnx_display_priv_destroy(self->priv);

    ok &= wegl_display_teardown(&self->wegl);
    free(self);

    return ok;
}
