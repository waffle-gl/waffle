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

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "wcore_config_attrs.h"
#include "wcore_error.h"
#include "wcore_platform.h"

#include "wegl_config.h"
#include "wegl_display.h"
#include "wegl_imports.h"
#include "wegl_platform.h"
#include "wegl_util.h"

/// @brief Check the WAFFLE_CONTEXT_* attributes.
static bool
check_context_attrs(struct wegl_display *dpy,
                    const struct wcore_config_attrs *attrs)
{
    struct wcore_platform *plat = dpy->wcore.platform;

    if (attrs->context_forward_compatible) {
        assert(attrs->context_api == WAFFLE_CONTEXT_OPENGL);
        assert(wcore_config_attrs_version_ge(attrs, 30));
    }

    if (attrs->context_debug && !dpy->KHR_create_context) {
        wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                     "EGL_KHR_create_context is required in order to "
                     "request a debug context");
        return false;
    }

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            if (!wcore_config_attrs_version_eq(attrs, 10) && !dpy->KHR_create_context) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "KHR_EXT_create_context is required in order to "
                             "request an OpenGL version not equal to the default "
                             "value 1.0");
                return false;
            }

            if (wcore_config_attrs_version_ge(attrs, 32)) {
                assert(attrs->context_profile == WAFFLE_CONTEXT_CORE_PROFILE ||
                       attrs->context_profile == WAFFLE_CONTEXT_COMPATIBILITY_PROFILE);
            }

            if (attrs->context_forward_compatible && !dpy->KHR_create_context) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "EGL_KHR_create_context is required in order to "
                             "request a forward-compatible context");
                return false;
            }

            if (!plat->vtbl->dl_can_open(plat, WAFFLE_DL_OPENGL)) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "failed to open the OpenGL library");
                return false;
            }

            return true;

        case WAFFLE_CONTEXT_OPENGL_ES1:
            if (!plat->vtbl->dl_can_open(plat, WAFFLE_DL_OPENGL_ES1)) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "failed to open the OpenGL ES1 library");
                return false;
            }

            return true;

        case WAFFLE_CONTEXT_OPENGL_ES2:
            if (!plat->vtbl->dl_can_open(plat, WAFFLE_DL_OPENGL_ES2)) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "failed to open the OpenGL ES2 library");
                return false;
            }

            return true;

        case WAFFLE_CONTEXT_OPENGL_ES3:
            if (!dpy->KHR_create_context) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "EGL_KHR_create_context is required to request "
                             "an OpenGL ES3 context");
                return false;
            }

            if (!plat->vtbl->dl_can_open(plat, WAFFLE_DL_OPENGL_ES3)) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "failed to open the OpenGL ES3 library");
                return false;
            }

            return true;

        default:
            wcore_error_internal("context_api has bad value %#x",
                                 attrs->context_api);
            return false;
    }
}

static EGLConfig
choose_real_config(struct wegl_display *dpy,
                   const struct wcore_config_attrs *attrs)
{
    struct wegl_platform *plat = wegl_platform(dpy->wcore.platform);
    EGLConfig config = NULL;
    bool ok = true;

    if (attrs->accum_buffer) {
        wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                     "accum buffers do not exist on EGL");
        return NULL;
    }

    // WARNING: If you resize attrib_list, then update renderable_index.
    const int renderable_index = 19;

    EGLint attrib_list[] = {
        // From page 17 of the EGL 1.4 spec:
        //
        //     EGL_BUFFER_SIZE gives the total of the color component bits of
        //     the color buffer2 For an RGB color buffer, the total is the sum
        //     of EGL_RED_SIZE, EGL_GREEN_SIZE, EGL_BLUE_SIZE, and
        //     EGL_ALPHA_SIZE.
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
        case WAFFLE_CONTEXT_OPENGL_ES3:
            attrib_list[renderable_index] = EGL_OPENGL_ES3_BIT_KHR;
            break;
        default:
            wcore_error_internal("waffle_context_api has bad value %#x",
                                 attrs->context_api);
            return NULL;
    }

    EGLint num_configs = 0;
    ok &= plat->eglChooseConfig(dpy->egl,
                                attrib_list, &config, 1, &num_configs);
    if (!ok) {
        wegl_emit_error(plat, "eglChooseConfig");
        return NULL;
    }
    else if (num_configs == 0) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "eglChooseConfig found no matching configs");
        return NULL;
    }

    return config;
}

struct wcore_config*
wegl_config_choose(struct wcore_platform *wc_plat,
                   struct wcore_display *wc_dpy,
                   const struct wcore_config_attrs *attrs)
{
    struct wegl_display *dpy = wegl_display(wc_dpy);
    struct wegl_config *config;
    bool ok;

    config = wcore_calloc(sizeof(*config));
    if (!config)
        return NULL;

    ok = wcore_config_init(&config->wcore, wc_dpy, attrs);
    if (!ok)
        goto fail;

    if (!check_context_attrs(dpy, attrs))
        goto fail;

    config->egl = choose_real_config(dpy, attrs);
    if (!config->egl)
        goto fail;

    return &config->wcore;

fail:
    wegl_config_destroy(&config->wcore);
    return NULL;
}

bool
wegl_config_destroy(struct wcore_config *wc_config)
{
    struct wegl_config *config = wegl_config(wc_config);
    bool result = true;

    if (!config)
        return true;

    result &= wcore_config_teardown(wc_config);
    free(config);
    return result;
}
