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

#include "xegl_gl_misc.h"

#include <dlfcn.h>

#include <waffle/native.h>

#include "xegl_priv_egl.h"
#include "xegl_priv_types.h"

bool
xegl_make_current(
        union native_window *window,
        union native_context *ctx)
{
    union native_display *dpy = window->xegl->display;
    bool ok = egl_make_current(dpy->xegl->egl_display,
                               window->xegl->egl_surface,
                               ctx->xegl->egl_context);
    return ok;
}

void*
xegl_get_proc_address(
        union native_platform *native,
        const char *name)
{
    return eglGetProcAddress(name);
}

void*
xegl_dlsym_gl(
        union native_platform *native,
        const char *name)
{
    return dlsym(native->xegl->libgl, name);
}