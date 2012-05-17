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
    .window_swap_buffers = xegl_window_swap_buffers,
    .make_current = xegl_make_current,
    .get_proc_address = xegl_get_proc_address,
};

union native_platform*
xegl_platform_create(
        int gl_api,
        const struct native_dispatch **dispatch)
{
    union native_platform *self;
    NATIVE_ALLOC(self, xegl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    switch (gl_api) {
        case WAFFLE_OPENGL:
        case WAFFLE_OPENGL_ES1:
        case WAFFLE_OPENGL_ES2:
            break;
        default:
            wcore_error_internal("gl_api has bad value 0x%x", gl_api);
            goto error;
    }

    self->xegl->gl_api = gl_api;
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
