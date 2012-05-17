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
