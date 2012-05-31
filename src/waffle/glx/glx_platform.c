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

/// @addtogroup glx_platform
/// @{

/// @file

#include "glx_platform.h"

#include <dlfcn.h>
#include <stdlib.h>

#include <waffle/native.h>
#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_error.h>
#include <waffle/linux/linux_platform.h>

#include "glx_config.h"
#include "glx_context.h"
#include "glx_display.h"
#include "glx_dl.h"
#include "glx_gl_misc.h"
#include "glx_priv_types.h"
#include "glx_window.h"

static const struct native_dispatch glx_dispatch = {
    .display_connect = glx_display_connect,
    .display_disconnect = glx_display_disconnect,
    .display_supports_context_api = glx_display_supports_context_api,
    .config_choose = glx_config_choose,
    .config_destroy = glx_config_destroy,
    .context_create = glx_context_create,
    .context_destroy = glx_context_destroy,
    .dl_can_open = glx_dl_can_open,
    .dl_sym = glx_dl_sym,
    .window_create = glx_window_create,
    .window_destroy = glx_window_destroy,
    .window_show = glx_window_show,
    .window_swap_buffers = glx_window_swap_buffers,
    .make_current = glx_make_current,
    .get_proc_address = glx_get_proc_address,
};

union native_platform*
glx_platform_create(const struct native_dispatch **dispatch)
{
    union native_platform *self;
    NATIVE_ALLOC(self, glx);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->glx->glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddress((const uint8_t*) "glXCreateContextAttribsARB");

    self->glx->linux_ = linux_platform_create();
    if (!self->glx->linux_)
        goto error;

    *dispatch = &glx_dispatch;
    return self;

error:
    glx_platform_destroy(self);
    return NULL;
}

bool
glx_platform_destroy(union native_platform *self)
{
    bool ok = true;

    if (!self)
        return true;

    if (self->glx->linux_)
        ok &= linux_platform_destroy(self->glx->linux_);

    free(self);
    return ok;
}

/// @}
