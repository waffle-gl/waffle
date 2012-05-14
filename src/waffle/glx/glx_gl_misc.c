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

/// @addtogroup glx_gl_misc
/// @{

/// @file

#include "glx_gl_misc.h"

#include <waffle/native.h>
#include <waffle/linux/linux_platform.h>

#include "glx_priv_types.h"

bool
glx_make_current(
        union native_display *dpy,
        union native_window *window,
        union native_context *ctx)
{
    return glXMakeCurrent(dpy->glx->xlib_display,
                          window ? window->glx->glx_window : 0,
                          ctx ? ctx->glx->glx_context : 0);
}

void*
glx_get_proc_address(
        union native_platform *native,
        const char *name)
{
    return glXGetProcAddress((const uint8_t*) name);
}

/// @}
