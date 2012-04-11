// Copyright 2012 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

// WL_EGL_PLATFORM configures Mesa's <EGL/egl.h> to define native types (such
// as EGLNativeDisplay) as Wayland types rather than Xlib types.
#define WL_EGL_PLATFORM 1

#include <wayland-client.h>
#include <wayland-egl.h>

#include <EGL/egl.h>

union native_display;
union native_platform;

struct wayland_platform {
    /// @brief One of WAFFLE_{GL,GLES1,GLES2}.
    int gl_api;

    /// @brief The GL library obtained with dlopen().
    void *libgl;

    /// @brief For example, "libGLESv2.so".
    const char *libgl_name;
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
