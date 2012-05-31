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

/// @addtogroup glx_display
/// @{

/// @file

#include "glx_display.h"

#include <stdlib.h>

#include <GL/glx.h>

#include <waffle/native.h>
#include <waffle/waffle_enum.h>
#include <waffle/waffle_gl_misc.h>
#include <waffle/core/wcore_error.h>
#include <waffle/linux/linux_platform.h>
#include <waffle/x11/x11.h>

#include "glx_priv_types.h"

static bool
glx_display_set_extensions(struct glx_display *self)
{

    const char *s = glXQueryExtensionsString(self->xlib_display,
                                             self->screen);
    if (!s) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "glXQueryExtensionsString failed");
        return false;
    }

    self->extensions.ARB_create_context                     = waffle_is_extension_in_string(s, "GLX_ARB_create_context");
    self->extensions.ARB_create_context_profile             = waffle_is_extension_in_string(s, "GLX_ARB_create_context_profile");
    self->extensions.EXT_create_context_es2_profile         = waffle_is_extension_in_string(s, "GLX_EXT_create_context_es2_profile");

    return true;
}

union native_display*
glx_display_connect(
        union native_platform *platform,
        const char *name)
{
    bool ok = true;

    union native_display *self;
    NATIVE_ALLOC(self, glx);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->glx->platform = platform;

    ok &= x11_display_connect(name,
                              &self->glx->xlib_display,
                              &self->glx->xcb_connection);
    if (!ok)
        goto error;

    ok = glx_display_set_extensions(self->glx);
    if (!ok)
        goto error;

    return self;

error:
    glx_display_disconnect(self);
    return NULL;
}

bool
glx_display_disconnect(union native_display *self)
{
    bool ok = true;

    if (!self)
        return true;

    if (self->glx->xlib_display)
        ok &= x11_display_disconnect(self->glx->xlib_display);

    free(self);
    return ok;
}

bool
glx_display_supports_context_api(
        union native_display *self,
        int32_t context_api)
{
    struct linux_platform *linux_plat = self->glx->platform->glx->linux_;

    switch (context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            return true;
        case WAFFLE_CONTEXT_OPENGL_ES1:
            return false;
        case WAFFLE_CONTEXT_OPENGL_ES2:
            return self->glx->extensions.EXT_create_context_es2_profile
                   && linux_platform_dl_can_open(linux_plat,
                                                 WAFFLE_DL_OPENGL_ES2);
        default:
            wcore_error_internal("waffle_context_api has bad value %#x",
                                 context_api);
            return false;
    }
}

/// @}
