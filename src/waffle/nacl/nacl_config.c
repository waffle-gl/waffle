// Copyright 2014 Intel Corporation
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

#include "ppapi/c/pp_graphics_3d.h"

#include "wcore_config_attrs.h"
#include "wcore_error.h"

#include "nacl_config.h"

bool
nacl_config_destroy(struct wcore_config *wc_self)
{
    bool ok = true;

    if (wc_self == NULL)
        return ok;

    ok &= wcore_config_teardown(wc_self);
    free(nacl_config(wc_self));
    return ok;
}

struct wcore_config*
nacl_config_choose(struct wcore_platform *wc_plat,
                  struct wcore_display *wc_dpy,
                  const struct wcore_config_attrs *attrs)
{
    struct nacl_config *self;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    // Currently only OpenGL ES 2.0 is supported.
    if (attrs->context_api != WAFFLE_CONTEXT_OPENGL_ES2) {
        wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                     "NaCl does no support context type %s.",
                     wcore_enum_to_string(attrs->context_api));
        goto error;
    }

    unsigned attr = 0;

    // Max amount of attribs is hardcoded in nacl_config.h (64)
#define PUSH_ATTRIB(a, val) \
    if (val != WAFFLE_DONT_CARE) {\
        self->attribs[attr++] = a; \
        self->attribs[attr++] = val;\
    }

    PUSH_ATTRIB(PP_GRAPHICS3DATTRIB_ALPHA_SIZE,     attrs->alpha_size);
    PUSH_ATTRIB(PP_GRAPHICS3DATTRIB_BLUE_SIZE,      attrs->blue_size);
    PUSH_ATTRIB(PP_GRAPHICS3DATTRIB_GREEN_SIZE,     attrs->green_size);
    PUSH_ATTRIB(PP_GRAPHICS3DATTRIB_RED_SIZE,       attrs->red_size);
    PUSH_ATTRIB(PP_GRAPHICS3DATTRIB_DEPTH_SIZE,     attrs->depth_size);
    PUSH_ATTRIB(PP_GRAPHICS3DATTRIB_STENCIL_SIZE,   attrs->stencil_size);
    PUSH_ATTRIB(PP_GRAPHICS3DATTRIB_SAMPLES,        attrs->samples);
    PUSH_ATTRIB(PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS, attrs->sample_buffers);

    // Note, we have to have at least 1x1 size so that initial context
    // backing surface creation will succeed without errors. Later on
    // it is resized by window creation/resize.
    PUSH_ATTRIB(PP_GRAPHICS3DATTRIB_WIDTH,  1);
    PUSH_ATTRIB(PP_GRAPHICS3DATTRIB_HEIGHT, 1);
    PUSH_ATTRIB(PP_GRAPHICS3DATTRIB_NONE, 0);

#undef PUSH_ATTRIB

    ok = wcore_config_init(&self->wcore, wc_dpy, attrs);
    if (!ok)
        goto error;

    return &self->wcore;

error:
    nacl_config_destroy(&self->wcore);
    self = NULL;
    return NULL;
}
