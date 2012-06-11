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

#include <waffle/waffle_enum.h>
#include <waffle/waffle_gl_misc.h>

#include <waffle/core/wcore_error.h>
#include <waffle/linux/linux_platform.h>

#include "glx_display.h"
#include "glx_platform.h"

static const struct wcore_display_vtbl glx_display_wcore_vtbl;

static bool
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

    const char *s = glXQueryExtensionsString(self->x11.xlib,
                                             self->x11.screen);
    if (!s) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "glXQueryExtensionsString failed");
        return false;
    }

    self->extensions.ARB_create_context                     = waffle_is_extension_in_string(s, "GLX_ARB_create_context");
    self->extensions.ARB_create_context_profile             = waffle_is_extension_in_string(s, "GLX_ARB_create_context_profile");
    self->extensions.EXT_create_context_es2_profile         = waffle_is_extension_in_string(s, "GLX_EXT_create_context_es2_profile");

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

    self->wcore.vtbl = &glx_display_wcore_vtbl;
    return &self->wcore;

error:
    glx_display_destroy(&self->wcore);
    return NULL;
}

static bool
glx_display_supports_context_api(struct wcore_display *wc_self,
                                 int32_t context_api)
{
    struct glx_display *self = glx_display(wc_self);
    struct glx_platform *plat = glx_platform(wc_self->platform);

    switch (context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            return true;
        case WAFFLE_CONTEXT_OPENGL_ES1:
            return false;
        case WAFFLE_CONTEXT_OPENGL_ES2:
            return self->extensions.EXT_create_context_es2_profile
                   && linux_platform_dl_can_open(plat->linux,
                                                 WAFFLE_DL_OPENGL_ES2);
        default:
            wcore_error_internal("waffle_context_api has bad value %#x",
                                 context_api);
            return false;
    }
}

static union waffle_native_display*
glx_display_get_native(struct wcore_display *wc_self)
{
    struct glx_display *self = glx_display(wc_self);
    struct waffle_glx_display *n_dpy;

    n_dpy = wcore_malloc(sizeof(*n_dpy));
    if (n_dpy == NULL)
        return NULL;

    n_dpy->xlib_display = self->x11.xlib;

    return (union waffle_native_display*) n_dpy;
}

static const struct wcore_display_vtbl glx_display_wcore_vtbl = {
    .destroy = glx_display_destroy,
    .get_native = glx_display_get_native,
    .supports_context_api = glx_display_supports_context_api,
};
