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

/// @addtogroup xegl_platform
/// @{

/// @file

#include "xegl_platform.h"

#define _POSIX_C_SOURCE 200112 // glib feature macro for unsetenv()

#include <dlfcn.h>
#include <stdlib.h>

#include <waffle/native.h>
#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_error.h>
#include <waffle/linux/linux_platform.h>

#include "xegl_config.h"
#include "xegl_context.h"
#include "xegl_display.h"
#include "xegl_dl.h"
#include "xegl_gl_misc.h"
#include "xegl_priv_egl.h"
#include "xegl_priv_types.h"
#include "xegl_window.h"

static const struct native_dispatch xegl_dispatch = {
    .display_connect = xegl_display_connect,
    .display_disconnect = xegl_display_disconnect,
    .display_supports_context_api = xegl_display_supports_context_api,
    .config_choose = xegl_config_choose,
    .config_destroy = xegl_config_destroy,
    .context_create = xegl_context_create,
    .context_destroy = xegl_context_destroy,
    .dl_can_open = xegl_dl_can_open,
    .dl_sym = xegl_dl_sym,
    .window_create = xegl_window_create,
    .window_destroy = xegl_window_destroy,
    .window_show = xegl_window_show,
    .window_swap_buffers = xegl_window_swap_buffers,
    .make_current = xegl_make_current,
    .get_proc_address = xegl_get_proc_address,
};

union native_platform*
xegl_platform_create(const struct native_dispatch **dispatch)
{
    union native_platform *self;
    NATIVE_ALLOC(self, xegl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->xegl->linux_ = linux_platform_create();
    if (!self->xegl->linux_)
        goto error;

    setenv("EGL_PLATFORM", "x11", true);

    *dispatch = &xegl_dispatch;
    return self;

error:
    xegl_platform_destroy(self);
    return NULL;
}

bool
xegl_platform_destroy(union native_platform *self)
{
    bool ok = true;

    if (!self)
        return true;

    unsetenv("EGL_PLATFORM");

    if (self->xegl->linux_)
        ok &= linux_platform_destroy(self->xegl->linux_);

    free(self);
    return ok;
}

/// @}
