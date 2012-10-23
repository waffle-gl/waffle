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
#include <stdint.h>

#include "waffle/egl/wegl_display.h"

struct droid_surfaceflinger_container;

struct droid_display {
    /// Used by droid_surfaceflinger.cpp.
    struct droid_surfaceflinger_container *pSFContainer;
    struct wegl_display wegl;
};

static inline struct droid_display*
droid_display(struct wcore_display *wc_self)
{
    if (wc_self) {
        struct wegl_display *wegl_self = container_of(wc_self, struct wegl_display, wcore);
        return container_of(wegl_self, struct droid_display, wegl);
    }
    else {
        return NULL;
    }
}

struct wcore_display*
droid_display_connect(struct wcore_platform *wc_plat,
                      const char *name);

bool
droid_display_disconnect(struct wcore_display *wc_self);

union waffle_native_display*
droid_display_get_native(struct wcore_display *wc_self);
