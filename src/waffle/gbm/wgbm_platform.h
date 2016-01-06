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

#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <gbm.h>

#undef linux

#include "wegl_platform.h"
#include "wcore_util.h"

#define GBM_FUNCTIONS(f) \
    f(struct gbm_device * , gbm_create_device            , (int fd)) \
    f(int                 , gbm_device_get_fd            , (struct gbm_device *dev)) \
    f(void                , gbm_device_destroy           , (struct gbm_device *gbm)) \
    f(struct gbm_surface *, gbm_surface_create           , (struct gbm_device *gbm, uint32_t width, uint32_t height, uint32_t format, uint32_t flags)) \
    f(void                , gbm_surface_destroy          , (struct gbm_surface *surface)) \
    f(struct gbm_bo *     , gbm_surface_lock_front_buffer, (struct gbm_surface *surface)) \
    f(void                , gbm_surface_release_buffer   , (struct gbm_surface *surface, struct gbm_bo *bo))

struct linux_platform;

struct wgbm_platform {
    struct wegl_platform wegl;
    struct linux_platform *linux;

    // GBM function pointers
    void *gbmHandle;

#define DECLARE(type, function, args) type (*function) args;
    GBM_FUNCTIONS(DECLARE)
#undef DECLARE
};

DEFINE_CONTAINER_CAST_FUNC(wgbm_platform,
                           struct wgbm_platform,
                           struct wegl_platform,
                           wegl)

bool
wgbm_platform_init(struct wgbm_platform *self);

bool
wgbm_platform_teardown(struct wgbm_platform *self);

struct wcore_platform*
wgbm_platform_create(void);

bool
wgbm_platform_destroy(struct wcore_platform *wc_self);

bool
wgbm_dl_can_open(struct wcore_platform *wc_self,
                 int32_t waffle_dl);

void*
wgbm_dl_sym(struct wcore_platform *wc_self,
            int32_t waffle_dl,
            const char *name);
