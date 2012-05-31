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

/// @addtogroup glx_context
/// @{

/// @file

#define GLX_GLXEXT_PROTOTYPES

#include "glx_context.h"

#include <assert.h>
#include <stdlib.h>

#include <waffle/native.h>
#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_error.h>

#include "glx_priv_types.h"

enum {
    WAFFLE_GLX_CONTEXT_ATTRS_LENGTH = 7,
};

/// @brief Fill @a attrib_list, which will be given to glXCreateContextAttribsARB().
///
/// This does not validate the `config->context_*` attributes. That validation
/// occurred during waffle_config_choose().
static bool
glx_context_fill_attrib_list(
        struct glx_config *config,
        int attrib_list[])
{
    struct glx_display *dpy = config->display->glx;
    int i = 0;

    attrib_list[i++] = GLX_CONTEXT_MAJOR_VERSION_ARB;
    attrib_list[i++] = config->context_major_version;

    attrib_list[i++] = GLX_CONTEXT_MINOR_VERSION_ARB;
    attrib_list[i++] = config->context_minor_version;

    if (dpy->extensions.ARB_create_context_profile) {
        attrib_list[i++] = GLX_CONTEXT_PROFILE_MASK_ARB;

        switch (config->context_api) {
            case WAFFLE_CONTEXT_OPENGL:
                switch (config->context_profile) {
                    case WAFFLE_CONTEXT_CORE_PROFILE:
                        attrib_list[i++] = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
                        break;
                    case WAFFLE_CONTEXT_COMPATIBILITY_PROFILE:
                        attrib_list[i++] = GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
                        break;
                    case WAFFLE_NONE:
                        attrib_list[i++] = 0;
                        break;
                }
                break;
            case WAFFLE_CONTEXT_OPENGL_ES2:
                attrib_list[i++] = GLX_CONTEXT_ES2_PROFILE_BIT_EXT;
                break;
        }
    }

    attrib_list[i++] = GLX_NONE;
    return true;
}

static GLXContext
glx_context_create_native(
        struct glx_config *config,
        GLXContext share_ctx)
{
    GLXContext ctx;
    struct glx_display *dpy = config->display->glx;
    struct glx_platform *platform = dpy->platform->glx;

    if (dpy->extensions.ARB_create_context) {
        bool ok;

        int attrib_list[WAFFLE_GLX_CONTEXT_ATTRS_LENGTH];
        ok = glx_context_fill_attrib_list(config, attrib_list);
        if (!ok)
            return false;

        ctx = platform->glXCreateContextAttribsARB(dpy->xlib_display,
                                                   config->glx_fbconfig,
                                                   share_ctx,
                                                   true /*direct?*/,
                                                   attrib_list);
        if (!ctx) {
            wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                         "glXCreateContextAttribsARB failed");
            return NULL;
        }
    }
    else {
        ctx = glXCreateNewContext(dpy->xlib_display,
                                  config->glx_fbconfig,
                                  GLX_RGBA_TYPE,
                                  share_ctx,
                                  true /*direct?*/);
        if (!ctx) {
            wcore_errorf(WAFFLE_UNKNOWN_ERROR, "glXCreateContext failed");
            return NULL;
        }
    }

    return ctx;
}

union native_context*
glx_context_create(
        union native_config *config,
        union native_context *share_ctx)
{
    union native_display *dpy = config->glx->display;

    union native_context *self;
    NATIVE_ALLOC(self, glx);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->glx->display = dpy;
    self->glx->glx_context = glx_context_create_native(
                                config->glx,
                                share_ctx ? share_ctx->glx->glx_context : NULL);
    if (!self->glx->glx_context)
        goto error;

    return self;

error:
    free(self);
    return NULL;
}

bool
glx_context_destroy(union native_context *self)
{
    if (!self)
        return true;

    union native_display *dpy = self->glx->display;

    if (self->glx->glx_context)
        glXDestroyContext(dpy->glx->xlib_display,
                          self->glx->glx_context);

    free(self);
    return true;
}

/// @}
