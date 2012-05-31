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
