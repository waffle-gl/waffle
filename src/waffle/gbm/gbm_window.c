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

#define __GBM__ 1

#include <stdlib.h>
#include <string.h>

#include <gbm.h>

#include <waffle/core/wcore_error.h>
#include <waffle_gbm.h>

#include "gbm_config.h"
#include "gbm_display.h"
#include "gbm_priv_egl.h"
#include "gbm_window.h"

static const struct wcore_window_vtbl gbm_window_wcore_vtbl;

static bool
gbm_window_destroy(struct wcore_window *wc_self)
{
    return false;
}

struct wcore_window*
gbm_window_create(struct wcore_platform *wc_plat,
                  struct wcore_config *wc_config,
                  int width,
                  int height)
{
    return NULL;
}


static bool
gbm_window_show(struct wcore_window *wc_self)
{
    return false;
}

static bool
gbm_window_swap_buffers(struct wcore_window *wc_self)
{
    return false;
}

static union waffle_native_window*
gbm_window_get_native(struct wcore_window *wc_self)
{
    return NULL;
}

static const struct wcore_window_vtbl gbm_window_wcore_vtbl = {
    .destroy = gbm_window_destroy,
    .get_native = gbm_window_get_native,
    .show = gbm_window_show,
    .swap_buffers = gbm_window_swap_buffers,
};
