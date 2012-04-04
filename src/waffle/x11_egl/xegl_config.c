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

#include "xegl_config.h"

#include <stdlib.h>
#include <string.h>

#include <waffle/native.h>
#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_error.h>

#include "xegl_priv_egl.h"
#include "xegl_priv_types.h"

union native_config*
xegl_config_choose(
        union native_display *dpy,
        const struct wcore_config_attrs *attrs)
{
    union native_platform *platform = dpy->xegl->platform;
    bool ok = true;

    union native_config *self;
    NATIVE_ALLOC(self, xegl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->xegl->display = dpy;

    ok &= egl_get_render_buffer_attrib(attrs, &self->xegl->egl_render_buffer);
    if (!ok)
        goto error;
    self->xegl->egl_config = egl_choose_config(dpy->xegl->egl_display,
                                               attrs,
                                               platform->xegl->gl_api);
    if (!self->xegl->egl_config)
        goto error;
    ok &= eglGetConfigAttrib(dpy->xegl->egl_display,
                             self->xegl->egl_config,
                             EGL_NATIVE_VISUAL_ID,
                             (EGLint*) &self->xegl->xcb_visual_id);
    if (!ok) {
        egl_get_error("eglGetConfigAttrib(EGL_NATIVE_VISUAL_ID)");
        goto error;
    }

    return self;

error:
    free(self);
    return NULL;
}

bool
xegl_config_destroy(union native_config *self)
{
    free(self);
    return true;
}
