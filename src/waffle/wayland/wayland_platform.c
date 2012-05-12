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

#include "wayland_config.h"
#include "wayland_context.h"
#include "wayland_display.h"
#include "wayland_gl_misc.h"
#include "wayland_priv_egl.h"
#include "wayland_priv_types.h"
#include "wayland_window.h"

static const struct native_dispatch wayland_dispatch = {
    .display_connect = wayland_display_connect,
    .display_disconnect = wayland_display_disconnect,
    .config_choose = wayland_config_choose,
    .config_destroy = wayland_config_destroy,
    .context_create = wayland_context_create,
    .context_destroy = wayland_context_destroy,
    .window_create = wayland_window_create,
    .window_destroy = wayland_window_destroy,
    .window_swap_buffers = wayland_window_swap_buffers,
    .make_current = wayland_make_current,
    .get_proc_address = wayland_get_proc_address,
    .dlsym_gl = wayland_dlsym_gl,
};

union native_platform*
wayland_platform_create(
        int gl_api,
        const struct native_dispatch **dispatch)
{
    bool ok = true;

    union native_platform *self;
    NATIVE_ALLOC(self, wl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->wl->gl_api = gl_api;

    switch (gl_api) {
        case WAFFLE_OPENGL:      self->wl->libgl_name = "libGL.so";        break;
        case WAFFLE_OPENGL_ES1:  self->wl->libgl_name = "libGLESv1_CM.so"; break;
        case WAFFLE_OPENGL_ES2:  self->wl->libgl_name = "libGLESv2.so";    break;
        default:
            wcore_error_internal("gl_api has bad value 0x%x", gl_api);
            goto error;
    }

    setenv("EGL_PLATFORM", "wayland", true);
    ok &= egl_bind_api(gl_api);
    if (!ok)
        goto error;

    self->wl->libgl = dlopen(self->wl->libgl_name, RTLD_LAZY);
    if (!self->wl->libgl) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "dlopen(\"%s\") failed", self->wl->libgl_name);
        goto error;
    }

    *dispatch = &wayland_dispatch;
    return self;

error:
    WCORE_ERROR_DISABLED({
        wayland_platform_destroy(self);
    });
    return NULL;
}

bool
wayland_platform_destroy(union native_platform *self)
{
    int error = 0;

    if (!self)
        return true;

    unsetenv("EGL_PLATFORM");

    if (self->wl->libgl) {
        error |= dlclose(self->wl->libgl);
        if (error) {
            wcore_errorf(WAFFLE_UNKNOWN_ERROR, "dlclose() failed on \"%s\"",
                         self->wl->libgl_name);
        }
    }

    free(self);
    return !error;
}

/// @}
