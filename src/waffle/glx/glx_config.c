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

#include "glx_priv_types.h"

union native_config*
glx_config_choose(
        union native_display *dpy,
        const struct wcore_config_attrs *attrs)
{
    bool ok = true;

    GLXFBConfig *configs = NULL;
    int num_configs;
    XVisualInfo *vi = NULL;

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