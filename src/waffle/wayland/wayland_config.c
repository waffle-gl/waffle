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

/// @addtogroup wayland_config
/// @{

/// @file

#include "wayland_config.h"

#include <stdlib.h>
#include <string.h>

#include <waffle/native.h>
#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_config_attrs.h>
#include <waffle/core/wcore_error.h>

#include "wayland_priv_egl.h"
#include "wayland_priv_types.h"

union native_config*
wayland_config_choose(
        union native_display *dpy,
        const struct wcore_config_attrs *attrs)
{
    union native_platform *platform = dpy->wl->platform;
    bool ok = true;

    union native_config *self;
    NATIVE_ALLOC(self, wl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->wl->display = dpy;

    ok &= egl_get_render_buffer_attrib(attrs, &self->wl->egl_render_buffer);
    if (!ok)
        goto error;

    self->wl->egl_config = egl_choose_config(platform->wl->linux_,
                                             dpy->wl->egl_display,
                                             attrs);
    if (!self->wl->egl_config)
        goto error;

    self->wl->waffle_context_api = attrs->context_api;

    return self;

error:
    free(self);
    return NULL;
}

bool
wayland_config_destroy(union native_config *self)
{
    free(self);
    return true;
}

/// @}
