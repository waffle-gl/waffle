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

#include <stdlib.h>
#include <string.h>

#include "linux_platform.h"

#include "wcore_config_attrs.h"
#include "wcore_error.h"

#include "glx_config.h"
#include "glx_display.h"
#include "glx_platform.h"
#include "glx_wrappers.h"

bool
glx_config_destroy(struct wcore_config *wc_self)
{
    bool ok = true;

    if (wc_self == NULL)
        return ok;

    ok &= wcore_config_teardown(wc_self);
    free(glx_config(wc_self));
    return ok;
}

/// @brief Check the values of `attrs->context_*`.
static bool
glx_config_check_context_attrs(struct glx_display *dpy,
                               const struct wcore_config_attrs *attrs)
{
    struct glx_platform *plat = glx_platform(dpy->wcore.platform);

    if (attrs->context_forward_compatible) {
        assert(attrs->context_api == WAFFLE_CONTEXT_OPENGL);
        assert(wcore_config_attrs_version_ge(attrs, 30));
    }

    if (attrs->context_debug && !dpy->ARB_create_context) {
        wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                     "GLX_ARB_create_context is required in order to "
                     "request a debug context");
        return false;
    }

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            if (!wcore_config_attrs_version_eq(attrs, 10) && !dpy->ARB_create_context) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "GLX_ARB_create_context is required in order to "
                             "request an OpenGL version not equal to the default "
                             "value 1.0");
                return false;
            }
            else if (wcore_config_attrs_version_ge(attrs, 32) && !dpy->ARB_create_context_profile) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "GLX_ARB_create_context_profile is required "
                             "to create a context with version >= 3.2");
                return false;
            }
            else if (wcore_config_attrs_version_ge(attrs, 32)) {
                assert(attrs->context_profile == WAFFLE_CONTEXT_CORE_PROFILE ||
                       attrs->context_profile == WAFFLE_CONTEXT_COMPATIBILITY_PROFILE);
            }

            if (attrs->context_forward_compatible && !dpy->ARB_create_context) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "GLX_ARB_create_context is required in order to "
                             "request a forward-compatible context");
                return false;
            }

            return true;

        case WAFFLE_CONTEXT_OPENGL_ES1:
            assert(wcore_config_attrs_version_eq(attrs, 10) ||
                   wcore_config_attrs_version_eq(attrs, 11));
            assert(!attrs->context_forward_compatible);

            if (!dpy->EXT_create_context_es_profile) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "GLX_EXT_create_context_es_profile is required "
                             "to create an OpenGL ES1 context");
                return false;
            }
            else if (!linux_platform_dl_can_open(plat->linux,
                                            WAFFLE_DL_OPENGL_ES1)) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "failed to open the OpenGL ES1 library");
                return false;
            }

            return true;

        case WAFFLE_CONTEXT_OPENGL_ES2:
            assert(attrs->context_major_version == 2);
            assert(!attrs->context_forward_compatible);

            if (!dpy->EXT_create_context_es2_profile) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "GLX_EXT_create_context_es2_profile is required "
                             "to create an OpenGL ES2 context");
                return false;
            }
            if (!linux_platform_dl_can_open(plat->linux,
                                            WAFFLE_DL_OPENGL_ES2)) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "failed to open the OpenGL ES2 library");
                return false;
            }
            return true;

        case WAFFLE_CONTEXT_OPENGL_ES3:
            assert(attrs->context_major_version == 3);

            if (!dpy->EXT_create_context_es_profile) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "GLX_EXT_create_context_es_profile is required "
                             "to create an OpenGL ES3 context");
                return false;
            }

            if (!linux_platform_dl_can_open(plat->linux,
                                            WAFFLE_DL_OPENGL_ES3)) {
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

struct wcore_config*
glx_config_choose(struct wcore_platform *wc_plat,
                  struct wcore_display *wc_dpy,
                  const struct wcore_config_attrs *attrs)
{
    struct glx_config *self;
    struct glx_display *dpy = glx_display(wc_dpy);
    struct glx_platform *plat = glx_platform(wc_plat);

    GLXFBConfig *configs = NULL;
    int num_configs = 0;
    XVisualInfo *vi = NULL;

    bool ok = true;

    if (!glx_config_check_context_attrs(dpy, attrs))
        return NULL;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_config_init(&self->wcore, wc_dpy, attrs);
    if (!ok)
        goto error;

    int attrib_list[] = {
        // From page 12 (18 of pdf) of the GLX 1.4 spec:
        //
        //    For GLXFBConfigs that correspond to a TrueColor or DirectColor
        //    visual, GLX BUFFER SIZE is the sum of GLX RED SIZE, GLX GREEN
        //    SIZE, GLX BLUE SIZE, and GLX ALPHA SIZE.
        GLX_BUFFER_SIZE,        attrs->rgba_size,
        GLX_RED_SIZE,           attrs->red_size,
        GLX_GREEN_SIZE,         attrs->green_size,
        GLX_BLUE_SIZE,          attrs->blue_size,
        GLX_ALPHA_SIZE,         attrs->alpha_size,

        GLX_DEPTH_SIZE,         attrs->depth_size,
        GLX_STENCIL_SIZE,       attrs->stencil_size,

        GLX_SAMPLE_BUFFERS,     attrs->sample_buffers,
        GLX_SAMPLES,            attrs->samples,

        GLX_DOUBLEBUFFER,       attrs->double_buffered,

        GLX_ACCUM_RED_SIZE,     attrs->accum_buffer,
        GLX_ACCUM_GREEN_SIZE,   attrs->accum_buffer,
        GLX_ACCUM_BLUE_SIZE,    attrs->accum_buffer,
        GLX_ACCUM_ALPHA_SIZE,   attrs->accum_buffer,

        // According to the GLX 1.4 spec Table 3.4, the default value of
        // GLX_DRAWABLE_TYPE is GLX_WINDOW_BIT. Explicitly set the default
        // here for the sake of self-documentation.
        GLX_DRAWABLE_TYPE,      GLX_WINDOW_BIT,

        0,
    };

    // Set glx_fbconfig.
    configs = wrapped_glXChooseFBConfig(plat, dpy->x11.xlib,
                                        dpy->x11.screen,
                                        attrib_list,
                                        &num_configs);
    if (!configs || num_configs == 0) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "glXChooseFBConfig returned no matching configs");
        goto error;
    }
    // Simply take the first.
    self->glx_fbconfig = configs[0];

    // Set glx_fbconfig_id.
    ok = !wrapped_glXGetFBConfigAttrib(plat, dpy->x11.xlib,
                                       self->glx_fbconfig,
                                       GLX_FBCONFIG_ID,
                                       &self->glx_fbconfig_id);
    if (!ok) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "glxGetFBConfigAttrib failed");
        goto error;
    }

    // Set xcb_visual_id.
    vi = wrapped_glXGetVisualFromFBConfig(plat, dpy->x11.xlib,
                                          self->glx_fbconfig);
    if (!vi) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "glXGetVisualInfoFromFBConfig failed with "
                     "GLXFBConfigID=0x%x\n", self->glx_fbconfig_id);
        goto error;
    }
    self->xcb_visual_id = vi->visualid;

    goto cleanup;

error:
    glx_config_destroy(&self->wcore);
    self = NULL;

cleanup:
    if (configs)
        XFree(configs);
    if (vi)
        XFree(vi);

    return &self->wcore;
}

union waffle_native_config*
glx_config_get_native(struct wcore_config *wc_self)
{
    struct glx_config *self = glx_config(wc_self);
    struct glx_display *dpy = glx_display(wc_self->display);
    union waffle_native_config *n_config;

    WCORE_CREATE_NATIVE_UNION(n_config, glx);
    if (!n_config)
        return NULL;

    n_config->glx->xlib_display = dpy->x11.xlib;
    n_config->glx->glx_fbconfig = self->glx_fbconfig;

    return n_config;
}
