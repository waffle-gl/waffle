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

/// @addtogroup wayland_platform
/// @{

/// @file

#include "wayland_platform.h"

#define _POSIX_C_SOURCE 200112 // glib feature macro for unsetenv()

#include <dlfcn.h>
#include <stdlib.h>

#include <waffle/native.h>
#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_error.h>
#include <waffle/linux/linux_platform.h>

#include "wayland_config.h"
#include "wayland_context.h"
#include "wayland_display.h"
#include "wayland_dl.h"
#include "wayland_gl_misc.h"
#include "wayland_priv_egl.h"
#include "wayland_priv_types.h"
#include "wayland_window.h"

static const struct native_dispatch wayland_dispatch = {
    .display_connect = wayland_display_connect,
    .display_disconnect = wayland_display_disconnect,
    .display_supports_context_api = wayland_display_supports_context_api,
    .config_choose = wayland_config_choose,
    .config_destroy = wayland_config_destroy,
    .context_create = wayland_context_create,
    .context_destroy = wayland_context_destroy,
    .dl_can_open = wayland_dl_can_open,
    .dl_sym = wayland_dl_sym,
    .window_create = wayland_window_create,
    .window_destroy = wayland_window_destroy,
    .window_show = wayland_window_show,
    .window_swap_buffers = wayland_window_swap_buffers,
    .make_current = wayland_make_current,
    .get_proc_address = wayland_get_proc_address,
};

union native_platform*
wayland_platform_create(const struct native_dispatch **dispatch)
{
    union native_platform *self;
    NATIVE_ALLOC(self, wl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->wl->linux_ = linux_platform_create();
    if (!self->wl->linux_)
        goto error;

    setenv("EGL_PLATFORM", "wayland", true);

    *dispatch = &wayland_dispatch;
    return self;

error:
    wayland_platform_destroy(self);
    return NULL;
}

bool
wayland_platform_destroy(union native_platform *self)
{
    bool ok = true;

    if (!self)
        return true;

    unsetenv("EGL_PLATFORM");

    if (self->wl->linux_)
        ok &= linux_platform_destroy(self->wl->linux_);

    free(self);
    return ok;
}

/// @}
