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

/// @brief Check the `wcore_config_attrs.context_` attributes.
static bool
egl_config_check_context_attrs(
        struct linux_platform *platform,
        const struct wcore_config_attrs *attrs)
{
    int version = 10 * attrs->context_major_version
                + attrs->context_minor_version;

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            if (version != 10) {
                wcore_errorf(WAFFLE_UNSUPPORTED_ON_PLATFORM,
                             "on EGL, the requested version of OpenGL must be "
                             "the default value 1.0");
                return false;
            }
            if (!linux_platform_dl_can_open(platform, WAFFLE_DL_OPENGL)) {
                wcore_errorf(WAFFLE_UNSUPPORTED_ON_PLATFORM,
                             "failed to open the OpenGL library");
                return false;
            }
            return true;
        case WAFFLE_CONTEXT_OPENGL_ES1:
            if (!linux_platform_dl_can_open(platform, WAFFLE_DL_OPENGL_ES1)) {
                wcore_errorf(WAFFLE_UNSUPPORTED_ON_PLATFORM,
                             "failed to open the OpenGL ES1 library");
                return false;
            }
            return true;
        case WAFFLE_CONTEXT_OPENGL_ES2:
            if (!linux_platform_dl_can_open(platform, WAFFLE_DL_OPENGL_ES2)) {
                wcore_errorf(WAFFLE_UNSUPPORTED_ON_PLATFORM,
                             "failed to open the OpenGL ES2 library");
                return false;
            }
            return true;
        default:
            wcore_error_internal("context_api has bad value %#x",
                                 attrs->context_api);
            return false;
    }
}

EGLConfig
egl_choose_config(
        struct linux_platform *platform,
        EGLDisplay dpy,
        const struct wcore_config_attrs *attrs)
{
    bool ok = true;

    if (!egl_config_check_context_attrs(platform, attrs))
        return false;

    if (attrs->accum_buffer) {
        wcore_errorf(WAFFLE_UNSUPPORTED_ON_PLATFORM,
                     "accum buffers do not exist on EGL");
        return false;
    }

    // WARNING: If you resize attrib_list, then update renderable_index.
    const int renderable_index = 19;

    EGLint attrib_list[] = {
        EGL_BUFFER_SIZE,            attrs->rgba_size,
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

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            attrib_list[renderable_index] = EGL_OPENGL_BIT;
            break;
        case WAFFLE_CONTEXT_OPENGL_ES1:
            attrib_list[renderable_index] = EGL_OPENGL_ES_BIT;
            break;
        case WAFFLE_CONTEXT_OPENGL_ES2:
            attrib_list[renderable_index] = EGL_OPENGL_ES2_BIT;
            break;
        default:
            wcore_error_internal("waffle_context_api has bad value %#x",
                                 attrs->context_api);
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

static bool
egl_bind_api(int32_t waffle_context_api)
{
    bool ok = true;

    switch (waffle_context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            ok &= eglBindAPI(EGL_OPENGL_API);
            break;
        case WAFFLE_CONTEXT_OPENGL_ES1:
        case WAFFLE_CONTEXT_OPENGL_ES2:
            ok &= eglBindAPI(EGL_OPENGL_ES_API);
            break;
        default:
            wcore_error_internal("waffle_context_api has bad value #x%x",
                                 waffle_context_api);
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
        int32_t waffle_context_api)
{
    bool ok = true;
    EGLint attrib_list[3];

    switch (waffle_context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            attrib_list[0] = EGL_NONE;
            break;
        case WAFFLE_CONTEXT_OPENGL_ES1:
            attrib_list[0] = EGL_CONTEXT_CLIENT_VERSION;
            attrib_list[1] = 1;
            attrib_list[2] = EGL_NONE;
            break;
        case WAFFLE_CONTEXT_OPENGL_ES2:
            attrib_list[0] = EGL_CONTEXT_CLIENT_VERSION;
            attrib_list[1] = 2;
            attrib_list[2] = EGL_NONE;
            break;
        default:
            wcore_error_internal("waffle_context_api has bad value %#x",
                                 waffle_context_api);
            return EGL_NO_CONTEXT;
    }

    ok = egl_bind_api(waffle_context_api);
    if (!ok)
        return false;

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
    if (attrs->double_buffered)
        *egl_render_buffer_attrib = EGL_BACK_BUFFER;
    else
        *egl_render_buffer_attrib = EGL_SINGLE_BUFFER;

    return true;
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
