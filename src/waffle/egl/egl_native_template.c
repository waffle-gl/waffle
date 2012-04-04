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

#include "egl_native_template.h"

#include <stdbool.h>
#include <stdlib.h>

#include <waffle/waffle_enum.h>
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