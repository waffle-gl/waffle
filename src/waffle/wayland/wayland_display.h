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

#include <waffle/core/wcore_display.h>
#include <waffle/core/wcore_util.h>
#include <waffle/native/waffle_wayland.h>

struct wcore_platform;
struct wl_display;
struct wl_compositor;
struct wl_shell;

struct wayland_display {
    struct wcore_display wcore;

    struct wl_display *wl_display;
    struct wl_compositor *wl_compositor;
    struct wl_shell *wl_shell;

    EGLDisplay egl;
};

DEFINE_CONTAINER_CAST_FUNC(wayland_display,
                           struct wayland_display,
                           struct wcore_display,
                           wcore)

struct wcore_display*
wayland_display_connect(struct wcore_platform *wc_plat,
                        const char *name);

void
wayland_display_fill_native(struct wayland_display *self,
                            struct waffle_wayland_display *n_dpy);
