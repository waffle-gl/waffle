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

/// @addtogroup egl_native_template
/// @{

#include "egl_native_template.h"

#include <stdbool.h>
#include <stdlib.h>

#include "waffle_enum.h"
#include <waffle/core/wcore_error.h>

#include "egl_no_native.h"

EGLDisplay
NATIVE_EGL(initialize)(EGLNativeDisplayType native_dpy)
{
    bool ok = true;

    EGLDisplay dpy = eglGetDisplay(native_dpy);
    if (dpy == EGL_NO_DISPLAY) {
        egl_get_error("eglGetDisplay");
        return 0;
    }

    ok &= eglInitialize(dpy, NULL, NULL);
    if (!ok) {
        egl_get_error("eglInitialize");
        eglTerminate(dpy);
        return 0;
    }

    return dpy;
}

EGLSurface
NATIVE_EGL(create_window_surface)(
        EGLDisplay dpy,
        EGLConfig config,
        EGLNativeWindowType native_window,
        EGLint render_buffer_attrib)
{
    EGLint attrib_list[] = {
        EGL_RENDER_BUFFER, render_buffer_attrib,
        EGL_NONE,
    };

    EGLSurface surf = eglCreateWindowSurface(dpy, config, native_window,
                                             attrib_list);
    if (!surf)
        egl_get_error("eglCreateWindowSurface");

    return surf;
}

/// @}
