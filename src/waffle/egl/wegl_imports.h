// Copyright 2013 Intel Corporation
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

/// @file
/// @brief Imports from EGL headers.
///
/// Waffle uses some EGL extensions that are fairly recent, so recent that some
/// systems' EGL headers lack the extensions. This header imports the needed
/// symbols from those extensions.

#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>

#ifndef EGL_VERSION_1_5
typedef intptr_t EGLAttrib;
#endif

#ifndef EGL_KHR_create_context
#define EGL_KHR_create_context 1
#define EGL_CONTEXT_MAJOR_VERSION_KHR                       EGL_CONTEXT_CLIENT_VERSION
#define EGL_CONTEXT_MINOR_VERSION_KHR                       0x30FB
#define EGL_CONTEXT_FLAGS_KHR                               0x30FC
#define EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR                 0x30FD
#define EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_KHR  0x31BD
#define EGL_NO_RESET_NOTIFICATION_KHR                       0x31BE
#define EGL_LOSE_CONTEXT_ON_RESET_KHR                       0x31BF
#define EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR                    0x00000001
#define EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR       0x00000002
#define EGL_CONTEXT_OPENGL_ROBUST_ACCESS_BIT_KHR            0x00000004
#define EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR             0x00000001
#define EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR    0x00000002
#define EGL_OPENGL_ES3_BIT_KHR                              0x00000040
#endif

#ifndef EGL_KHR_platform_android
#define EGL_KHR_platform_android 1
#define EGL_PLATFORM_ANDROID_KHR          0x3141
#endif /* EGL_KHR_platform_android */

#ifndef EGL_KHR_platform_gbm
#define EGL_KHR_platform_gbm 1
#define EGL_PLATFORM_GBM_KHR              0x31D7
#endif /* EGL_KHR_platform_gbm */

#ifndef EGL_KHR_platform_wayland
#define EGL_KHR_platform_wayland 1
#define EGL_PLATFORM_WAYLAND_KHR          0x31D8
#endif /* EGL_KHR_platform_wayland */

#ifndef EGL_KHR_platform_x11
#define EGL_KHR_platform_x11 1
#define EGL_PLATFORM_X11_KHR              0x31D5
#define EGL_PLATFORM_X11_SCREEN_KHR       0x31D6
#endif /* EGL_KHR_platform_x11 */

#ifndef EGL_MESA_platform_surfaceless
#define EGL_MESA_platform_surfaceless 1
#define EGL_PLATFORM_SURFACELESS_MESA     0x31DD
#endif /* EGL_MESA_platform_surfaceless */
