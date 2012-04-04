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

#include "egl_no_native.h"

#include <stdlib.h>
#include <string.h>

#include <waffle/waffle_enum.h>
#include <waffle/waffle_attrib_list.h>
#include <waffle/core/wcore_error.h>

void
egl_get_error(const char *egl_func_call)
{
    EGLint egl_error_code = eglGetError();
    const char *egl_error_name;

    switch (egl_error_code) {
#define CASE(x) case x: egl_error_name = #x; break
        CASE(EGL_FALSE);
        CASE(EGL_TRUE);
        CASE(EGL_DONT_CARE);
        CASE(EGL_SUCCESS);
        CASE(EGL_NOT_INITIALIZED);
        CASE(EGL_BAD_ACCESS);
        CASE(EGL_BAD_ALLOC);
        CASE(EGL_BAD_ATTRIBUTE);
        CASE(EGL_BAD_CONFIG);
        CASE(EGL_BAD_CONTEXT);
        CASE(EGL_BAD_CURRENT_SURFACE);
        CASE(EGL_BAD_DISPLAY);
        CASE(EGL_BAD_MATCH);
        CASE(EGL_BAD_NATIVE_PIXMAP);
        CASE(EGL_BAD_NATIVE_WINDOW);
        CASE(EGL_BAD_PARAMETER);
        CASE(EGL_BAD_SURFACE);
        CASE(EGL_CONTEXT_LOST);
        default: egl_error_name = ""; break;
#undef CASE
    }

    wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                 "%s failed with error %s(0x%x)",
                 egl_func_call,
                 egl_error_name,
                 egl_error_code);
}

bool
egl_terminate(EGLDisplay dpy)
{
    bool ok = eglTerminate(dpy);
    if (!ok)
        egl_get_error("eglTerminate");
    return ok;
}

bool
egl_destroy_surface(
        EGLDisplay dpy,
        EGLSurface surface)
{
    bool ok = eglDestroySurface(dpy, surface);
    if (!ok)
        egl_get_error("eglDestroySurface");
    return ok;
}

bool
egl_bind_api(int32_t waffle_gl_api)
{
    bool ok = true;

    switch (waffle_gl_api) {
        case WAFFLE_GL:
            ok &= eglBindAPI(EGL_OPENGL_API);
            break;
        case WAFFLE_GLES1:
        case WAFFLE_GLES2:
            ok &= eglBindAPI(EGL_OPENGL_ES_API);
            break;
        default:
            wcore_error_internal("gl_api has bad value 0x%x", waffle_gl_api);
            return false;
    }

    if (!ok)
        egl_get_error("eglBindAPI");
    return ok;
}

EGLContext
egl_create_context(
        EGLDisplay dpy,
        EGLConfig config,
        EGLContext share_context,
        int32_t waffle_gl_api)
{
    EGLint attrib_list[3];

    switch (waffle_gl_api) {
        case WAFFLE_GL:
            attrib_list[0] = EGL_NONE;
            break;
        case WAFFLE_GLES1:
            attrib_list[0] = EGL_CONTEXT_CLIENT_VERSION;
            attrib_list[1] = 1;
            attrib_list[2] = EGL_NONE;
            break;
        case WAFFLE_GLES2:
            attrib_list[0] = EGL_CONTEXT_CLIENT_VERSION;
            attrib_list[1] = 2;
            attrib_list[2] = EGL_NONE;
            break;
        default:
            wcore_error_internal("bad value for gl_api (0x%x)", waffle_gl_api);
            return EGL_NO_CONTEXT;
    }

    EGLContext ctx = eglCreateContext(dpy, config, share_context, attrib_list);
    if (!ctx)
        egl_get_error("eglCreateContext");

    return ctx;
}

bool
egl_destroy_context(
        EGLDisplay dpy,
        EGLContext ctx)
{
    bool ok = eglDestroyContext(dpy, ctx);
    if (!ok)
        egl_get_error("eglDestroyContext");
    return ok;
}

bool
egl_make_current(
        EGLDisplay dpy,
        EGLSurface surface,
        EGLContext ctx)
{
    bool ok = eglMakeCurrent(dpy, surface, surface, ctx);
    if (!ok)
        egl_get_error("eglMakeCurrent");
    return ok;
}

bool
egl_swap_buffers(
        EGLDisplay dpy,
        EGLSurface surface)
{
    bool ok = eglSwapBuffers(dpy, surface);
    if (!ok)
        egl_get_error("eglSwapBuffers");
    return ok;
}

bool
egl_get_render_buffer_attrib(
        const int32_t waffle_config_attrib_list[],
        EGLint *egl_render_buffer_attrib)
{
    int32_t w_value;
    waffle_attrib_list_get_with_default(waffle_config_attrib_list,
                                        WAFFLE_DOUBLE_BUFFERED,
                                        &w_value,
                                        true);

    switch (w_value) {
        case 0: *egl_render_buffer_attrib = EGL_SINGLE_BUFFER;  return true;
        case 1: *egl_render_buffer_attrib = EGL_BACK_BUFFER;    return true;

        default:
            wcore_errorf(WAFFLE_BAD_ATTRIBUTE,
                         "WAFFLE_DOUBLE_BUFFERED has bad value 0x%x", w_value);
            return false;
    }
}