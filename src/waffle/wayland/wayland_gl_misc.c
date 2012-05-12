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

/// @addtogroup wayland_gl_misc
/// @{

/// @file

#include "wayland_gl_misc.h"

#include <dlfcn.h>

#include <waffle/native.h>
#include <waffle/waffle_enum.h>
#include <waffle/linux/linux_platform.h>

#include "wayland_priv_egl.h"
#include "wayland_priv_types.h"

bool
wayland_make_current(
        union native_display *dpy,
        union native_window *window,
        union native_context *ctx)
{
    return egl_make_current(dpy->wl->egl_display,
                            window ? window->wl->egl_surface : 0,
                            ctx ? ctx->wl->egl_context : 0);
}

void*
wayland_get_proc_address(
        union native_platform *native,
        const char *name)
{
    return eglGetProcAddress(name);
}

void*
wayland_dlsym_gl(
        union native_platform *native,
        int32_t waffle_dl,
        const char *name)
{
    return linux_platform_dl_sym(native->wl->linux_, waffle_dl, name);
}

/// @}
