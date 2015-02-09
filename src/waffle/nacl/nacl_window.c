// Copyright 2014 Intel Corporation
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

#include "wcore_error.h"
#include "wcore_attrib_list.h"
#include "nacl_config.h"
#include "nacl_display.h"
#include "nacl_window.h"
#include "nacl_platform.h"

bool
nacl_window_destroy(struct wcore_window *wc_self)
{
    struct nacl_window *self = nacl_window(wc_self);
    bool ok = true;

    if (!wc_self)
        return ok;

    ok &= wcore_window_teardown(wc_self);
    free(self);
    return ok;
}

struct wcore_window*
nacl_window_create(struct wcore_platform *wc_plat,
                   struct wcore_config *wc_config,
                   int width,
                   int height,
                   const intptr_t attrib_list[])

{
    struct nacl_window *self;
    bool ok = true;

    if (wcore_attrib_list_length(attrib_list) > 0) {
        wcore_error_bad_attribute(attrib_list[0]);
        return NULL;
    }

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_window_init(&self->wcore, wc_config);
    if (!ok)
        goto error;

    if (!ok)
        goto error;

    return &self->wcore;

error:
    nacl_window_destroy(&self->wcore);
    return NULL;
}

bool
nacl_window_show(struct wcore_window *wc_self)
{
    return true;
}

bool
nacl_window_resize(struct wcore_window *wc_self,
                   int32_t width, int32_t height)
{
    return false;
}

bool
nacl_window_swap_buffers(struct wcore_window *wc_self)
{
    return false;
}
