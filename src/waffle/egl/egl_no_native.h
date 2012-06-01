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

/// @defgroup egl_no_native egl_no_native
/// @ingroup egl
///
/// @brief Wrappers for EGL functions that lack EGLNative parameters.
/// @{

/// @file
/// This header follows the same order as the official EGL header.

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <EGL/egl.h>

struct wcore_config_attrs;
struct wcore_platform;

/// @brief Sets the waffle error with info from eglGetError().
/// @param egl_func_call Examples are "eglMakeCurrent()" and
///     "eglBindAPI(EGL_OPENGL_API)".
void
egl_get_error(const char *egl_func_call);

bool
egl_terminate(EGLDisplay dpy);

EGLConfig
egl_choose_config(
        struct wcore_platform *plat,
        EGLDisplay dpy,
        const struct wcore_config_attrs *attrs);

bool
egl_destroy_surface(
        EGLDisplay dpy,
        EGLSurface surface);

EGLContext
egl_create_context(
        EGLDisplay dpy,
        EGLConfig config,
        EGLContext share_context,
        int32_t waffle_gl_api);

bool
egl_destroy_context(
        EGLDisplay dpy,
        EGLContext ctx);

bool
egl_make_current(
        EGLDisplay dpy,
        EGLSurface surface,
        EGLContext ctx);

bool
egl_swap_buffers(
        EGLDisplay dpy,
        EGLSurface surface);

/// @brief Translate WAFFLE_DOUBLE_BUFFERED to an EGL_RENDER_BUFFER value.
///
/// @param[in] attrs was provided by the user through waffle_config_choose().
/// @param[out] egl_render_buffer_attrib is intended to be put in the
///             attrib_list of eglCreateWindowSurface().
/// @return True on success.
bool
egl_get_render_buffer_attrib(
        const struct wcore_config_attrs *attrs,
        EGLint *egl_render_buffer_attrib);

/// @brief Helper for waffle_display_supports_context_api().
bool
egl_supports_context_api(
        struct wcore_platform *plat,
        int32_t context_api);

/// @}
