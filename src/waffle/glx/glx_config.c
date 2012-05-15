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

/// @addtogroup glx_config
/// @{

/// @file

#include "glx_config.h"

#include <stdlib.h>
#include <string.h>

#include <waffle/native.h>
#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_config_attrs.h>
#include <waffle/core/wcore_error.h>
#include <waffle/linux/linux_platform.h>

#include "glx_priv_types.h"

/// @brief Check the values of `attrs->context_*`.
static bool
glx_config_check_context_attrs(
        struct glx_display *dpy,
        const struct wcore_config_attrs *attrs)
{
    struct glx_platform *platform = dpy->platform->glx;
    int version = 10 * attrs->context_major_version
                + attrs->context_minor_version;

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            if (version != 10 && !dpy->extensions.ARB_create_context) {
                wcore_errorf(WAFFLE_UNSUPPORTED_ON_PLATFORM,
                             "GLX_ARB_create_context is required in order to "
                             "request a GL version not equal to the default "
                             "value 1.0");
                return false;
            }
            else if (version >= 32 && !dpy->extensions.EXT_create_context_es2_profile) {
                wcore_errorf(WAFFLE_UNSUPPORTED_ON_PLATFORM,
                             "GLX_EXT_create_context_es2_profile is required "
                             "to create a context with version >= 3.2");
                return false;
            }
            else if (version >= 32 && attrs->context_profile == WAFFLE_NONE) {
                wcore_errorf(WAFFLE_BAD_ATTRIBUTE,
                             "a profile must be specified when the GL version "
                             "is >= 3.2");
                return false;
            }
            return true;
        case WAFFLE_CONTEXT_OPENGL_ES1:
            wcore_errorf(WAFFLE_UNSUPPORTED_ON_PLATFORM,
                         "GLX does not support OpenGL ES1");
            return false;
        case WAFFLE_CONTEXT_OPENGL_ES2:
            if (!dpy->extensions.EXT_create_context_es2_profile) {
                wcore_errorf(WAFFLE_UNSUPPORTED_ON_PLATFORM,
                             "GLX_EXT_create_context_es2_profile is required "
                             "to create an OpenGL ES2 context");
                return false;
            }
            if (!linux_platform_dl_can_open(platform->linux_,
                                            WAFFLE_DL_OPENGL_ES2)) {
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

union native_config*
glx_config_choose(
        union native_display *dpy,
        const struct wcore_config_attrs *attrs)
{
    bool ok = true;

    GLXFBConfig *configs = NULL;
    int num_configs;
    XVisualInfo *vi = NULL;

    if (!glx_config_check_context_attrs(dpy->glx, attrs))
        return NULL;

    int attrib_list[] = {
        GLX_BUFFER_SIZE,        attrs->color_buffer_size,
        GLX_RED_SIZE,           attrs->red_size,
        GLX_GREEN_SIZE,         attrs->green_size,
        GLX_BLUE_SIZE,          attrs->blue_size,
        GLX_ALPHA_SIZE,         attrs->alpha_size,

        GLX_DEPTH_SIZE,         attrs->depth_size,
        GLX_STENCIL_SIZE,       attrs->stencil_size,

        GLX_SAMPLE_BUFFERS,     attrs->sample_buffers,
        GLX_SAMPLES,            attrs->samples,

        GLX_DOUBLEBUFFER,       attrs->double_buffered,

        // According to the GLX 1.4 spec Table 3.4, the default value of
        // GLX_DRAWABLE_TYPE is GLX_WINDOW_BIT. Explicitly set the default
        // here for the sake of self-documentation.
        GLX_DRAWABLE_TYPE,      GLX_WINDOW_BIT,

        0,
    };

    union native_config *self;
    NATIVE_ALLOC(self, glx);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->glx->display = dpy;

    // Set glx_fbconfig.
    configs = glXChooseFBConfig(dpy->glx->xlib_display,
                                DefaultScreen(dpy->glx->xlib_display),
                                attrib_list,
                                &num_configs);
    if (!configs || num_configs == 0) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "glXChooseFBConfig returned no matching configs");
        goto error;
    }
    // Simply take the first.
    self->glx->glx_fbconfig = configs[0];

    // Set glx_fbconfig_id.
    ok = !glXGetFBConfigAttrib(dpy->glx->xlib_display,
                               self->glx->glx_fbconfig,
                               GLX_FBCONFIG_ID,
                               &self->glx->glx_fbconfig_id);
    if (!ok) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR, "glxGetFBConfigAttrib failed");
        goto error;
    }

    // Set xcb_visual_id.
    vi = glXGetVisualFromFBConfig(dpy->glx->xlib_display,
                                  self->glx->glx_fbconfig);
    if (!vi) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "glXGetVisualInfoFromFBConfig failed with "
                     "GLXFBConfigID=0x%x\n", self->glx->glx_fbconfig_id);
        goto error;
    }
    self->glx->xcb_visual_id = vi->visualid;

    // Set context attributes.
    self->glx->context_api                  = attrs->context_api;
    self->glx->context_major_version        = attrs->context_major_version;
    self->glx->context_minor_version        = attrs->context_minor_version;
    self->glx->context_profile              = attrs->context_profile;

    goto end;

error:
    WCORE_ERROR_DISABLED({
        glx_config_destroy(self);
        self = NULL;
    });

end:
    if (configs)
        XFree(configs);
    if (vi)
        XFree(vi);

    return self;

}

bool
glx_config_destroy(union native_config *self)
{
    free(self);
    return true;
}

/// @}
