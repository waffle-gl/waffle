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

#include "waffle/core/wcore_config.h"
#include "waffle/core/wcore_util.h"

#include "wegl_display.h"

struct wcore_config_attrs;

struct wegl_config {
    struct wcore_config wcore;
    EGLConfig egl;

    /// The value of EGL_RENDER_BUFFER that will be set in the attrib_list
    /// of eglCreateWindowSurface().
    EGLint egl_render_buffer;

    /// The API given to waffle__config_choose(). This is later used to select
    /// the value of the EGL_RENDERABLE attribute.
    int32_t waffle_context_api;
};

DEFINE_CONTAINER_CAST_FUNC(wegl_config,
                           struct wegl_config,
                           struct wcore_config,
                           wcore)

struct wcore_config*
wegl_config_choose(struct wcore_display *wc_dpy,
                   const struct wcore_config_attrs *attrs);

bool
wegl_config_destroy(struct wcore_config *wc_config);
