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

#define WL_EGL_PLATFORM 1

#include <stdbool.h>
#include <stdint.h>

#include <EGL/egl.h>

#include "waffle.h"

#ifdef __cplusplus
extern "C" {
#endif

struct wl_compositor;
struct wl_display;
struct wl_egl_window;
struct wl_shell;
struct wl_shell_surface;
struct xdg_wm_base;
struct wl_surface;

struct waffle_wayland_display {
    struct wl_display *wl_display;
    struct wl_compositor *wl_compositor;
    // Will be NULL when compositor does not support the old wl_shell protocol
    struct wl_shell *wl_shell WAFFLE_DEPRECATED_1_07;
    EGLDisplay egl_display;
#if WAFFLE_API_VERSION >= 0x0107
    // Will be NULL when compositor does not support the new xdg-shell protocol
    struct xdg_wm_base *xdg_shell;
#endif
};

struct waffle_wayland_config {
    struct waffle_wayland_display display;
    EGLConfig egl_config;
};

struct waffle_wayland_context {
    struct waffle_wayland_display display;
    EGLContext egl_context;
};

struct waffle_wayland_window {
    struct waffle_wayland_display display;
    struct wl_surface *wl_surface;
    // Will be NULL when compositor does not support the old wl_shell protocol
    struct wl_shell_surface *wl_shell_surface WAFFLE_DEPRECATED_1_07;
    struct wl_egl_window *wl_window;
    EGLSurface egl_surface;
#if WAFFLE_API_VERSION >= 0x0107
    // Will be NULL when compositor does not support the new xdg-shell protocol
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;
#endif
};

#ifdef __cplusplus
} // end extern "C"
#endif
