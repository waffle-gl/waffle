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

#define GLX_GLXEXT_PROTOTYPES

// The official glxext.h published by Khronos does not yet define this bit.
// It is an alias of GLX_CONTEXT_ES2_PROFILE_BIT_EXT.
#define GLX_CONTEXT_ES_PROFILE_BIT_EXT 0x00000004

#include <assert.h>
#include <stdlib.h>

#include "wcore_error.h"

#include "glx_config.h"
#include "glx_context.h"
#include "glx_display.h"
#include "glx_platform.h"
#include "glx_wrappers.h"

bool
glx_context_destroy(struct wcore_context *wc_self)
{
    struct glx_context *self;
    struct glx_display *dpy;
    struct glx_platform *platform;
    bool ok = true;

    if (!wc_self)
        return ok;

    self = glx_context(wc_self);
    dpy = glx_display(wc_self->display);
    platform = glx_platform(wc_self->display->platform);

    if (self->glx)
        wrapped_glXDestroyContext(platform, dpy->x11.xlib, self->glx);

    ok &= wcore_context_teardown(wc_self);
    free(self);
    return ok;
}

/// @brief Fill @a attrib_list, which will be given to glXCreateContextAttribsARB().
///
/// This does not validate the `config->context_*` attributes. That validation
/// occurred during waffle_config_choose().
static bool
glx_context_fill_attrib_list(struct glx_config *config,
                             int attrib_list[])
{
    struct wcore_config_attrs *attrs = &config->wcore.attrs;
    int i = 0;
    int context_flags = 0;

    // As a workaround for NVidia, do not specify
    // GLX_CONTEXT_MAJOR_VERSION_ARB and GLX_CONTEXT_MINOR_VERSION_ARB in the
    // call to glXCreateContextAttribsARB if the user requested an OpenGL
    // context of unspecified version or if the user explicitly requested an
    // OpenGL 1.0 context.
    //
    // Calling glXCreateContextAttribARB with MAJOR=1 and MINOR=0, according
    // to the spec, is equivalent to calling it with MAJOR and MINOR
    // unspecified.  From the GLX_ARB_create_context spec:
    //
    //     If an attribute is not specified in <attrib_list>,
    //     then the default value specified below is used instead.
    //
    //     The default values for GLX_CONTEXT_MAJOR_VERSION_ARB and
    //     GLX_CONTEXT_MINOR_VERSION_ARB are 1 and 0 respectively. In this
    //     case, implementations will typically return the most recent version
    //     of OpenGL they support which is backwards compatible with OpenGL 1.0
    //     (e.g. 3.0, 3.1 + GL_ARB_compatibility, or 3.2 compatibility profile)
    //
    // However, NVidia's libGL, circa 2012-12-19, is not compliant. Calling
    // glXCreateContextAttribsARB with MAJOR=1 and MINOR=0 returns an OpenGL
    // 2.1 context. Calling it with MAJOR and MINOR unspecified returns
    // a context of the latest supported OpenGL version.
    if (!(wcore_config_attrs_version_eq(attrs, 10) &&
          attrs->context_api == WAFFLE_CONTEXT_OPENGL))
    {
        attrib_list[i++] = GLX_CONTEXT_MAJOR_VERSION_ARB;
        attrib_list[i++] = attrs->context_major_version;

        attrib_list[i++] = GLX_CONTEXT_MINOR_VERSION_ARB;
        attrib_list[i++] = attrs->context_minor_version;
    }

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            if (wcore_config_attrs_version_ge(attrs, 32)) {
                switch (attrs->context_profile) {
                    case WAFFLE_CONTEXT_CORE_PROFILE:
                        attrib_list[i++] = GLX_CONTEXT_PROFILE_MASK_ARB;
                        attrib_list[i++] = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
                        break;
                    case WAFFLE_CONTEXT_COMPATIBILITY_PROFILE:
                        attrib_list[i++] = GLX_CONTEXT_PROFILE_MASK_ARB;
                        attrib_list[i++] = GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
                        break;
                    default:
                        assert(false);
                        break;
                }
            }

            if (attrs->context_forward_compatible) {
                context_flags |= GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
            }

            break;
        case WAFFLE_CONTEXT_OPENGL_ES1:
        case WAFFLE_CONTEXT_OPENGL_ES2:
        case WAFFLE_CONTEXT_OPENGL_ES3:
            attrib_list[i++] = GLX_CONTEXT_PROFILE_MASK_ARB;
            attrib_list[i++] = GLX_CONTEXT_ES_PROFILE_BIT_EXT;
            break;
    }

    if (attrs->context_debug) {
        context_flags |= GLX_CONTEXT_DEBUG_BIT_ARB;
    }

    if (context_flags != 0) {
        attrib_list[i++] = GLX_CONTEXT_FLAGS_ARB;
        attrib_list[i++] = context_flags;
    }

    attrib_list[i++] = 0;
    return true;
}

static GLXContext
glx_context_create_native(struct glx_config *config,
                          struct glx_context *share_ctx)
{
    GLXContext ctx;
    GLXContext real_share_ctx = share_ctx ? share_ctx->glx : NULL;
    struct glx_display *dpy = glx_display(config->wcore.display);
    struct glx_platform *platform = glx_platform(dpy->wcore.platform);

    if (dpy->ARB_create_context) {
        bool ok;

        // Choose a large size to prevent accidental overflow.
        int attrib_list[64];

        ok = glx_context_fill_attrib_list(config, attrib_list);
        if (!ok)
            return NULL;

        ctx = wrapped_glXCreateContextAttribsARB(platform,
                                                 dpy->x11.xlib,
                                                 config->glx_fbconfig,
                                                 real_share_ctx,
                                                 true /*direct?*/,
                                                 attrib_list);
        if (!ctx) {
            wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                         "glXCreateContextAttribsARB failed");
            return NULL;
        }
    }
    else {
        ctx = wrapped_glXCreateNewContext(platform,
                                          dpy->x11.xlib,
                                          config->glx_fbconfig,
                                          GLX_RGBA_TYPE,
                                          real_share_ctx,
                                          true /*direct?*/);
        if (!ctx) {
            wcore_errorf(WAFFLE_ERROR_UNKNOWN, "glXCreateContext failed");
            return NULL;
        }
    }

    return ctx;
}

struct wcore_context*
glx_context_create(struct wcore_platform *wc_plat,
                   struct wcore_config *wc_config,
                   struct wcore_context *wc_share_ctx)
{
    struct glx_context *self;
    struct glx_config *config = glx_config(wc_config);
    struct glx_context *share_ctx = glx_context(wc_share_ctx);
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_context_init(&self->wcore, wc_config);
    if (!ok)
        goto error;

    self->glx = glx_context_create_native(config, share_ctx);
    if (!self->glx)
        goto error;

    return &self->wcore;

error:
    glx_context_destroy(&self->wcore);
    return NULL;
}

union waffle_native_context*
glx_context_get_native(struct wcore_context *wc_self)
{
    struct glx_context *self = glx_context(wc_self);
    struct glx_display *dpy = glx_display(wc_self->display);
    union waffle_native_context *n_ctx;

    WCORE_CREATE_NATIVE_UNION(n_ctx, glx);
    if (!n_ctx)
        return NULL;

    n_ctx->glx->xlib_display = dpy->x11.xlib;
    n_ctx->glx->glx_context = self->glx;

    return n_ctx;
}
