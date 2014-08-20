// Copyright 2014 Emil Velikov
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

#include <assert.h>
#include <stdlib.h>
#include <windows.h>

#include "wcore_error.h"

#include "wgl_config.h"
#include "wgl_context.h"
#include "wgl_display.h"
#include "wgl_error.h"
#include "wgl_window.h"

bool
wgl_context_destroy(struct wcore_context *wc_self)
{
    struct wgl_context *self = wgl_context(wc_self);
    bool ok = true;

    if (!self)
        return true;

    if (self->hglrc)
        ok &= wglDeleteContext(self->hglrc);

    ok &= wcore_context_teardown(wc_self);
    free(self);
    return ok;
}


/// @brief Fill @a attrib_list, which will be given to wglCreateContextAttribsARB().
///
/// This does not validate the `config->context_*` attributes. That validation
/// occurred during waffle_config_choose().
static bool
wgl_context_fill_attrib_list(struct wgl_config *config,
                             int attrib_list[])
{
    struct wcore_config_attrs *attrs = &config->wcore.attrs;
    int i = 0;
    int context_flags = 0;

    // XXX: Check if the following workaround is relevant under Windows.

    // As a workaround for NVidia, do not specify
    // WGL_CONTEXT_MAJOR_VERSION_ARB and WGL_CONTEXT_MINOR_VERSION_ARB in the
    // call to wglCreateContextAttribsARB if the user requested an OpenGL
    // context of unspecified version or if the user explicitly requested an
    // OpenGL 1.0 context.
    //
    // Calling wglCreateContextAttribARB with MAJOR=1 and MINOR=0, according
    // to the spec, is equivalent to calling it with MAJOR and MINOR
    // unspecified.  From the WGL_ARB_create_context spec:
    //
    //     If an attribute is not specified in <attrib_list>,
    //     then the default value specified below is used instead.
    //
    //     The default values for WGL_CONTEXT_MAJOR_VERSION_ARB and
    //     WGL_CONTEXT_MINOR_VERSION_ARB are 1 and 0 respectively. In this
    //     case, implementations will typically return the most recent version
    //     of OpenGL they support which is backwards compatible with OpenGL 1.0
    //     (e.g. 3.0, 3.1 + GL_ARB_compatibility, or 3.2 compatibility profile)
    //
    // However, NVidia's libGL, circa 2012-12-19, is not compliant. Calling
    // wglCreateContextAttribsARB with MAJOR=1 and MINOR=0 returns an OpenGL
    // 2.1 context. Calling it with MAJOR and MINOR unspecified returns
    // a context of the latest supported OpenGL version.
    if (!(wcore_config_attrs_version_eq(attrs, 10) &&
          attrs->context_api == WAFFLE_CONTEXT_OPENGL))
    {
        attrib_list[i++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
        attrib_list[i++] = attrs->context_major_version;

        attrib_list[i++] = WGL_CONTEXT_MINOR_VERSION_ARB;
        attrib_list[i++] = attrs->context_minor_version;
    }

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            if (wcore_config_attrs_version_ge(attrs, 32)) {
                switch (attrs->context_profile) {
                    case WAFFLE_CONTEXT_CORE_PROFILE:
                        attrib_list[i++] = WGL_CONTEXT_PROFILE_MASK_ARB;
                        attrib_list[i++] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
                        break;
                    case WAFFLE_CONTEXT_COMPATIBILITY_PROFILE:
                        attrib_list[i++] = WGL_CONTEXT_PROFILE_MASK_ARB;
                        attrib_list[i++] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
                        break;
                    default:
                        assert(false);
                        break;
                }
            }

            if (attrs->context_forward_compatible) {
                context_flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
            }

            break;
        case WAFFLE_CONTEXT_OPENGL_ES1:
        case WAFFLE_CONTEXT_OPENGL_ES2:
        case WAFFLE_CONTEXT_OPENGL_ES3:
            attrib_list[i++] = WGL_CONTEXT_PROFILE_MASK_ARB;
            attrib_list[i++] = WGL_CONTEXT_ES_PROFILE_BIT_EXT;
            break;
    }

    if (attrs->context_debug) {
        context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
    }

    if (context_flags != 0) {
        attrib_list[i++] = WGL_CONTEXT_FLAGS_ARB;
        attrib_list[i++] = context_flags;
    }

    attrib_list[i++] = 0;
    return true;
}

static HGLRC
wgl_context_create_native(struct wgl_config *config,
                          struct wgl_context *share_ctx)
{
    struct wgl_display *dpy = wgl_display(config->wcore.display);
    HGLRC real_share_ctx = share_ctx ? share_ctx->hglrc : NULL;
    HGLRC hglrc;

    if (dpy->ARB_create_context) {
        bool ok;

        // Choose a large size to prevent accidental overflow.
        int attrib_list[64];

        ok = wgl_context_fill_attrib_list(config, attrib_list);
        if (!ok)
            return NULL;

        hglrc = dpy->wglCreateContextAttribsARB(config->window->hDC,
                                                real_share_ctx,
                                                attrib_list);
        if (!hglrc) {
            wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                         "wglCreateContextAttribsARB failed");
            return NULL;
        }
    }
    else {
        hglrc = wglCreateContext(config->window->hDC);
        if (!hglrc) {
            wcore_errorf(WAFFLE_ERROR_UNKNOWN, "wglCreateContext failed");
            return NULL;
        }
    }

    return hglrc;
}

struct wcore_context*
wgl_context_create(struct wcore_platform *wc_plat,
                   struct wcore_config *wc_config,
                   struct wcore_context *wc_share_ctx)
{
    struct wgl_config *config = wgl_config(wc_config);
    struct wgl_context *share_ctx = wgl_context(wc_share_ctx);
    struct wgl_context *self;
    int error;

    self = wcore_calloc(sizeof(*self));
    if (!self)
        return NULL;

    error = !wcore_context_init(&self->wcore, wc_config);
    if (error)
        goto fail;

    self->hglrc = wgl_context_create_native(config, share_ctx);
    if (!self->hglrc)
        goto fail;

    return &self->wcore;

fail:
    wgl_context_destroy(&self->wcore);
    return NULL;
}
