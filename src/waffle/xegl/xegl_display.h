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

#include "waffle_x11_egl.h"

#include "x11_display.h"

#include "wegl_display.h"

struct wcore_platform;

struct xegl_display {
    struct x11_display x11;
    struct wegl_display wegl;
};

static inline struct xegl_display*
xegl_display(struct wcore_display *wc_self)
{
    if (wc_self) {
        struct wegl_display *wegl_self = container_of(wc_self, struct wegl_display, wcore);
        return container_of(wegl_self, struct xegl_display, wegl);
    }
    else {
        return NULL;
    }
}

struct wcore_display*
xegl_display_connect(struct wcore_platform *wc_plat,
                     const char *name);

bool
xegl_display_destroy(struct wcore_display *wc_self);

void
xegl_display_fill_native(struct xegl_display *self,
                         struct waffle_x11_egl_display *n_dpy);

union waffle_native_display*
xegl_display_get_native(struct wcore_display *wc_self);
