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

#include "waffle_portability.h"

#ifdef __cplusplus
extern "C" {
#endif

struct waffle_config;
struct waffle_display;

struct waffle_gbm_config;
struct waffle_glx_config;
struct waffle_x11_egl_config;
struct waffle_wayland_config;

union waffle_native_config {
    struct waffle_gbm_config *gbm;
    struct waffle_glx_config *glx;
    struct waffle_x11_egl_config *x11_egl;
    struct waffle_wayland_config *wayland;
};

WAFFLE_API struct waffle_config*
waffle_config_choose(struct waffle_display *dpy, const int32_t attrib_list[]);

WAFFLE_API bool
waffle_config_destroy(struct waffle_config *self);

WAFFLE_API union waffle_native_config*
waffle_config_get_native(struct waffle_config *self);

#ifdef __cplusplus
} // end extern "C"
#endif
