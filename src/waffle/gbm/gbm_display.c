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
#define _GNU_SOURCE

#include <stdlib.h>

#include <gbm.h>
#include <libudev.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>

#include <waffle/core/wcore_error.h>
#include <waffle/core/wcore_display.h>

#include "gbm_display.h"
#include "gbm_platform.h"
#include "gbm_priv_egl.h"

static const struct wcore_display_vtbl gbm_display_wcore_vtbl;

static bool
gbm_display_destroy(struct wcore_display *wc_self)
{
    return false;
}

struct wcore_display*
gbm_display_connect(struct wcore_platform *wc_plat,
                    const char *name)
{
    return NULL;
}


static bool
gbm_display_supports_context_api(struct wcore_display *wc_self,
                                 int32_t waffle_context_api)
{
    return false;
}

static union waffle_native_display*
gbm_display_get_native(struct wcore_display *wc_self)
{
    return NULL;
}

static const struct wcore_display_vtbl gbm_display_wcore_vtbl = {
    .destroy = gbm_display_destroy,
    .get_native = gbm_display_get_native,
    .supports_context_api = gbm_display_supports_context_api,
};
