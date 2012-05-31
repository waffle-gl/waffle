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

/// @addtogroup cgl_platform
/// @{

/// @file

#include "cgl_platform.h"

#include <stdlib.h>

#include <waffle/native.h>
#include <waffle/core/wcore_error.h>

#include "cgl_config.h"
#include "cgl_context.h"
#include "cgl_display.h"
#include "cgl_dl.h"
#include "cgl_gl_misc.h"
#include "cgl_window.h"

static const struct native_dispatch cgl_dispatch = {
    .display_connect = cgl_display_connect,
    .display_disconnect = cgl_display_disconnect,
    .display_supports_context_api = cgl_display_supports_context_api,
    .config_choose = cgl_config_choose,
    .config_destroy = cgl_config_destroy,
    .context_create = cgl_context_create,
    .context_destroy = cgl_context_destroy,
    .dl_can_open = cgl_dl_can_open,
    .dl_sym = cgl_dl_sym,
    .window_create = cgl_window_create,
    .window_destroy = cgl_window_destroy,
    .window_show = cgl_window_show,
    .window_swap_buffers = cgl_window_swap_buffers,
    .make_current = cgl_make_current,
    .get_proc_address = cgl_get_proc_address,
};

union native_platform*
cgl_platform_create(const struct native_dispatch **dispatch)
{
    union native_platform *self;
    NATIVE_ALLOC(self, cgl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    *dispatch = &cgl_dispatch;
    return self;
}

bool
cgl_platform_destroy(union native_platform *self)
{
    bool ok = true;

    if (!self)
        return true;

    if (self->cgl->dl_gl)
        ok &= cgl_dl_close(self->cgl);

    free(self);
    return ok;
}

/// @}
