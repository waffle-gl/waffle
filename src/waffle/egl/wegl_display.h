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

#include <EGL/egl.h>

#include "wcore_display.h"

struct wcore_display;

enum wegl_supported_api {
    WEGL_OPENGL_API = 1 << 0,
    WEGL_OPENGL_ES_API = 1 << 1,
};

struct wegl_display {
    struct wcore_display wcore;
    EGLDisplay egl;
    enum wegl_supported_api api_mask;
    bool EXT_create_context_robustness;
    bool KHR_create_context;
    EGLint major_version;
    EGLint minor_version;
};

DEFINE_CONTAINER_CAST_FUNC(wegl_display,
                           struct wegl_display,
                           struct wcore_display,
                           wcore)

bool
wegl_display_init(struct wegl_display *dpy,
                  struct wcore_platform *wc_plat,
                  intptr_t native_display);

bool
wegl_display_teardown(struct wegl_display *dpy);

bool
wegl_display_supports_context_api(struct wcore_display *wc_dpy,
                                  int32_t waffle_context_api);
