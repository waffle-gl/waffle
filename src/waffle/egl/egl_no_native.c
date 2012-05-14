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

/// @addtogroup egl_no_native
/// @{

#include "egl_no_native.h"

#include <stdlib.h>
#include <string.h>

#include <waffle/waffle_enum.h>
#include <waffle/waffle_attrib_list.h>
#include <waffle/core/wcore_config_attrs.h>
#include <waffle/core/wcore_error.h>
#include <waffle/linux/linux_platform.h>

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

EGLConfig
egl_choose_config(
        EGLDisplay dpy,
        const struct wcore_config_attrs *attrs,
        int32_t waffle_gl_api)
{
    bool ok = true;

    // WARNING: If you resize attrib_list, then update renderable_index.
    const int renderable_index = 19;

    EGLint attrib_list[] = {
        EGL_BUFFER_SIZE,            attrs->color_buffer_size,
        EGL_RED_SIZE,               attrs->red_size,
        EGL_GREEN_SIZE,             attrs->green_size,
        EGL_BLUE_SIZE,              attrs->blue_size,
        EGL_ALPHA_SIZE,             attrs->alpha_size,

        EGL_DEPTH_SIZE,             attrs->depth_size,
        EGL_STENCIL_SIZE,           attrs->stencil_size,

        EGL_SAMPLE_BUFFERS,         attrs->sample_buffers,
        EGL_SAMPLES,                attrs->samples,

        EGL_RENDERABLE_TYPE,        31415926,

        // According to the EGL 1.4 spec Table 3.4, the default value of
        // EGL_SURFACE_BIT is EGL_WINDOW_BIT.  Explicitly set the default here for
        // the sake of self-documentation.
        EGL_SURFACE_TYPE,           EGL_WINDOW_BIT,
        EGL_NONE,
    };

    switch (waffle_gl_api) {
        case WAFFLE_OPENGL:
            attrib_list[renderable_index] = EGL_OPENGL_BIT;
            break;
        case WAFFLE_OPENGL_ES1:
            attrib_list[renderable_index] = EGL_OPENGL_ES_BIT;
            break;
        case WAFFLE_OPENGL_ES2:
            attrib_list[renderable_index] = EGL_OPENGL_ES2_BIT;
            break;
        default:
            wcore_error_internal("gl_api has bad value 0x%x", waffle_gl_api);
            goto end;
    }

    EGLConfig config = NULL;
    EGLint num_configs = 0;
    ok &= eglChooseConfig(dpy, attrib_list, &config, 1, &num_configs);
    if (!ok) {
        egl_get_error("eglChooseConfig");
        goto end;
    }
    else if (num_configs == 0) {
        ok = false;
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "eglChooseConfig found no matching configs");
        goto end;
    }

end:
    return config;
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
        case WAFFLE_OPENGL:
            ok &= eglBindAPI(EGL_OPENGL_API);
            break;
        case WAFFLE_OPENGL_ES1:
        case WAFFLE_OPENGL_ES2:
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
        case WAFFLE_OPENGL:
            attrib_list[0] = EGL_NONE;
            break;
        case WAFFLE_OPENGL_ES1:
            attrib_list[0] = EGL_CONTEXT_CLIENT_VERSION;
            attrib_list[1] = 1;
            attrib_list[2] = EGL_NONE;
            break;
        case WAFFLE_OPENGL_ES2:
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
        const struct wcore_config_attrs *attrs,
        EGLint *egl_render_buffer_attrib)
{
    switch (attrs->double_buffered) {
        case true:
            *egl_render_buffer_attrib = EGL_BACK_BUFFER;
            return true;
        case false:
            *egl_render_buffer_attrib = EGL_SINGLE_BUFFER;
            return true;
        default:
            wcore_error_internal("%s", "attrs->double_buffered has bad value");
            return false;
    }
}

bool
egl_supports_context_api(
        struct linux_platform *platform,
        int32_t context_api)
{
    int32_t waffle_dl;

    switch (context_api) {
        case WAFFLE_CONTEXT_OPENGL:     waffle_dl = WAFFLE_DL_OPENGL;      break;
        case WAFFLE_CONTEXT_OPENGL_ES1: waffle_dl = WAFFLE_DL_OPENGL_ES1;  break;
        case WAFFLE_CONTEXT_OPENGL_ES2: waffle_dl = WAFFLE_DL_OPENGL_ES2;  break;

        default:
            wcore_error_internal("context_api has bad value %#x",
                                 context_api);
            return false;
    }

    return linux_platform_dl_can_open(platform, waffle_dl);
}

/// @}
