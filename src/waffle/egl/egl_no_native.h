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

struct linux_platform;
struct wcore_config_attrs;

/// @brief Sets the waffle error with info from eglGetError().
/// @param egl_func_call Examples are "eglMakeCurrent()" and
///     "eglBindAPI(EGL_OPENGL_API)".
void
egl_get_error(const char *egl_func_call);

bool
egl_terminate(EGLDisplay dpy);

EGLConfig
egl_choose_config(
        struct linux_platform *platform,
        EGLDisplay dpy,
        const struct wcore_config_attrs *attrs);

bool
egl_destroy_surface(
        EGLDisplay dpy,
        EGLSurface surface);

bool
egl_bind_api(int32_t waffle_gl_api);

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
        struct linux_platform *platform,
        int32_t context_api);

/// @}
