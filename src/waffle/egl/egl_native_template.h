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

/// @file
/// @brief Wrappers for EGL functions that have EGLNative parameters.
///
/// This header follows the same order as the official EGL header.

#pragma once

#include <EGL/egl.h>

#ifndef NATIVE_EGL
#   error NATIVE_EGL is not defined
#endif

/// @brief Wraps eglGetDisplay() and eglInitialize().
EGLDisplay
NATIVE_EGL(initialize)(EGLNativeDisplayType native_dpy);

EGLSurface
NATIVE_EGL(create_window_surface)(
        EGLDisplay dpy,
        EGLConfig config,
        EGLNativeWindowType native_window,
        EGLint render_buffer_attrib);