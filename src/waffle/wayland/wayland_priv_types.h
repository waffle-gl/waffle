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

/// @defgroup wayland_priv_types wayland_priv_types
/// @ingroup wayland
/// @{

/// @file

#pragma once

// WL_EGL_PLATFORM configures Mesa's <EGL/egl.h> to define native types (such
// as EGLNativeDisplay) as Wayland types rather than Xlib types.
#define WL_EGL_PLATFORM 1

#include <wayland-client.h>
#include <wayland-egl.h>

#include <EGL/egl.h>

struct linux_platform;

union native_display;
union native_platform;

struct wayland_platform {
    struct linux_platform *linux_;
};

struct wayland_display {
    union native_platform *platform;
    struct wl_display *wl_display;
    struct wl_compositor *wl_compositor;
    struct wl_shell *wl_shell;
    EGLDisplay egl_display;
};

struct wayland_config {
    union native_display *display;
    EGLConfig egl_config;

    /// The value of @c EGL_RENDER_BUFFER that will be set in the attrib_list
    /// of eglCreateWindowSurface().
    EGLint egl_render_buffer;

    int32_t waffle_context_api;
};

struct wayland_context {
    union native_display *display;
    EGLContext egl_context;
};

struct wayland_window {
    union native_display *display;
    struct wl_surface *wl_surface;
    struct wl_shell_surface *wl_shell_surface;
    struct wl_egl_window *wl_window;
    EGLSurface egl_surface;
};

/// @}
