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

/// @defgroup xegl_priv_types xegl_priv_types
/// @ingroup xegl
/// @{

/// @file

#pragma once

#include <EGL/egl.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>

struct linux_platform;

union native_display;
union native_platform;

struct xegl_platform {
    struct linux_platform *linux_;
};

struct xegl_display {
    union native_platform *platform;
    Display *xlib_display;
    xcb_connection_t *xcb_connection;
    EGLDisplay egl_display;
};

struct xegl_config {
    union native_display *display;
    EGLConfig egl_config;
    xcb_visualid_t xcb_visual_id;

    /// The value of @c EGL_RENDER_BUFFER that will be set in the attrib_list
    /// of eglCreateWindowSurface().
    EGLint egl_render_buffer;

    int32_t waffle_context_api;
};

struct xegl_context {
    union native_display *display;
    EGLContext egl_context;
};

struct xegl_window {
    union native_display *display;
    xcb_window_t xcb_window;
    EGLSurface egl_surface;
};

/// @}
