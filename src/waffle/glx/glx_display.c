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

#include "wcore_error.h"

#include "linux_platform.h"

#include "glx_display.h"
#include "glx_platform.h"
#include "glx_wrappers.h"

bool
glx_display_destroy(struct wcore_display *wc_self)
{
    struct glx_display *self = glx_display(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    ok &= x11_display_teardown(&self->x11);
    ok &= wcore_display_teardown(&self->wcore);
    free(self);
    return ok;
}

static bool
glx_display_set_extensions(struct glx_display *self)
{
    struct glx_platform *platform = glx_platform(self->wcore.platform);
    const char *s = wrapped_glXQueryExtensionsString(platform,
                                                     self->x11.xlib,
                                                     self->x11.screen);
    if (!s) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "glXQueryExtensionsString failed");
        return false;
    }

    self->ARB_create_context                     = waffle_is_extension_in_string(s, "GLX_ARB_create_context");
    self->ARB_create_context_profile             = waffle_is_extension_in_string(s, "GLX_ARB_create_context_profile");
    self->EXT_create_context_es_profile          = waffle_is_extension_in_string(s, "GLX_EXT_create_context_es_profile");

    // The GLX_EXT_create_context_es2_profile spec, version 4 2012/03/28,
    // states that GLX_EXT_create_context_es_profile is an alias of
    // GLX_EXT_create_context_es2_profile and requires that both names must be
    // exported together for backwards compatibility with clients that expect
    // the es2_profile name.
    if (self->EXT_create_context_es_profile) {
        self->EXT_create_context_es2_profile = true;
    }
    else {
        // Assume that GLX does not implement version 3 of the extension, in
        // which case the ES contexts GLX is capable of creating is ES2.
        self->EXT_create_context_es2_profile = waffle_is_extension_in_string(s, "GLX_EXT_create_context_es2_profile");
    }

    return true;
}

struct wcore_display*
glx_display_connect(struct wcore_platform *wc_plat,
                    const char *name)
{
    struct glx_display *self;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_display_init(&self->wcore, wc_plat);
    if (!ok)
        goto error;

    ok = x11_display_init(&self->x11, name);
    if (!ok)
        goto error;

    ok = glx_display_set_extensions(self);
    if (!ok)
        goto error;

    return &self->wcore;

error:
    glx_display_destroy(&self->wcore);
    return NULL;
}

bool
glx_display_supports_context_api(struct wcore_display *wc_self,
                                 int32_t context_api)
{
    struct glx_display *self = glx_display(wc_self);
    struct glx_platform *plat = glx_platform(wc_self->platform);

    switch (context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            return true;
        case WAFFLE_CONTEXT_OPENGL_ES1:
            return self->EXT_create_context_es_profile
                   && linux_platform_dl_can_open(plat->linux,
                                                 WAFFLE_DL_OPENGL_ES1);
        case WAFFLE_CONTEXT_OPENGL_ES2:
            return self->EXT_create_context_es2_profile
                   && linux_platform_dl_can_open(plat->linux,
                                                 WAFFLE_DL_OPENGL_ES2);
        case WAFFLE_CONTEXT_OPENGL_ES3:
            return self->EXT_create_context_es_profile
                   && linux_platform_dl_can_open(plat->linux,
                                                 WAFFLE_DL_OPENGL_ES3);
        default:
            wcore_error_internal("waffle_context_api has bad value %#x",
                                 context_api);
            return false;
    }
}

union waffle_native_display*
glx_display_get_native(struct wcore_display *wc_self)
{
    struct glx_display *self = glx_display(wc_self);
    union waffle_native_display *n_dpy;

    WCORE_CREATE_NATIVE_UNION(n_dpy, glx);
    if (!n_dpy)
        return NULL;

    n_dpy->glx->xlib_display = self->x11.xlib;

    return n_dpy;
}
