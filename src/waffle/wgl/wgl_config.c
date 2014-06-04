// Copyright 2014 Emil Velikov
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
#include <string.h>

#include "wcore_config_attrs.h"
#include "wcore_error.h"

#include "wgl_config.h"
#include "wgl_error.h"
#include "wgl_platform.h"

bool
wgl_config_destroy(struct wcore_config *wc_self)
{
    struct wgl_config *self = wgl_config(wc_self);
    bool ok;

    if (!self)
        return true;

    ok = wcore_config_teardown(wc_self);
    free(self);
    return ok;
}

struct wcore_config*
wgl_config_choose(struct wcore_platform *wc_plat,
                  struct wcore_display *wc_dpy,
                  const struct wcore_config_attrs *attrs)
{
    struct wgl_config *self;
    bool ok;

    self = wcore_calloc(sizeof(*self));
    if (!self)
        return NULL;

    ok = wcore_config_init(&self->wcore, wc_dpy, attrs);
    if (!ok)
        goto error;

    return &self->wcore;

error:
    wgl_config_destroy(&self->wcore);
    return NULL;
}
