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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "wcore_config_attrs.h"
#include "wcore_error.h"

#include "cgl_config.h"
#include "cgl_error.h"
#include "cgl_platform.h"

#include <AvailabilityMacros.h>

#ifndef kCGLPFAOpenGLProfile
#   define kCGLPFAOpenGLProfile     99
#   define kCGLOGLPVersion_3_2_Core 0x3200
#   define kCGLOGLPVersion_Legacy   0x1000
#endif

bool
cgl_config_destroy(struct wcore_config *wc_self)
{
    bool ok = true;

    if (wc_self == NULL)
        return ok;

    ok &= wcore_config_teardown(wc_self);
    free(cgl_config(wc_self));
    return ok;
}

/// @brief Check the values of `attrs->context_*`.
static bool
cgl_config_check_attrs(const struct cgl_platform *plat,
                       const struct wcore_config_attrs *attrs)
{
    if (attrs->context_forward_compatible) {
        // As of 2013-09-16 and up to Mac OS 10.8 Mountain Lion, CGL does not
        // support forward-compatible contexts.
        wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                     "CGL does not support forward-compatible contexts");
        return false;
    }

    if (attrs->context_debug) {
        // As of 2013-09-16 and up to Mac OS 10.8 Mountain Lion, CGL does not
        // support debug contexts.
        wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                     "CGL does not support debug contexts");
        return false;
    }

    // Emulate EGL_KHR_create_context, which allows the implementation to
    // return a context of the latest supported flavor that is
    // backwards-compatibile with the requested flavor.
    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            if (attrs->context_profile == WAFFLE_CONTEXT_COMPATIBILITY_PROFILE) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "CGL does not support OpenGL compatibility "
                             "profiles");
                return false;
            }

            if (wcore_config_attrs_version_gt(attrs, 32)) {
                if (plat->system_version_full >= 0x0a09) {
                    wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                                 "Waffle CGL was built only with Mac OS 10.8 "
                                 "capabilities, and so cannot create an "
                                 "OpenGL > 3.2 context");
                    return false;
                }
                else {
                    wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                                 "this Mac OS version cannot create an "
                                 "OpenGL > 3.2 context");
                    return false;
                }
            }
            else if (wcore_config_attrs_version_eq(attrs, 32)) {
                assert(attrs->context_profile == WAFFLE_CONTEXT_CORE_PROFILE);

                if (plat->system_version_full < 0x0a07)
                    goto error_need_os_10_7;

                return true;
            }
            else if (wcore_config_attrs_version_eq(attrs, 31)) {
                // Emulate EGL_KHR_create_context, which allows
                // implementations to promote OpenGL 3.1 to
                // OpenGL 3.2 Core Profile.
                assert(attrs->context_profile == WAFFLE_NONE);

                if (plat->system_version_full < 0x0a07)
                    goto error_need_os_10_7;

                return true;
            }
            else if (wcore_config_attrs_version_gt(attrs, 21)) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "CGL does not support OpenGL %d.%d contexts",
                             attrs->context_major_version,
                             attrs->context_minor_version);
                return false;
            }
            else {
                assert(wcore_config_attrs_version_ge(attrs, 10));
                return true;
            }
        case WAFFLE_CONTEXT_OPENGL_ES1:
            wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                         "CGL does not support OpenGL ES1");
            return false;
        case WAFFLE_CONTEXT_OPENGL_ES2:
            wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                         "CGL does not support OpenGL ES2");
            return false;
        case WAFFLE_CONTEXT_OPENGL_ES3:
            wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                         "CGL does not support OpenGL ES3");
            return false;
        default:
            assert(false);
            return false;
    }

error_need_os_10_7:
    wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                 "Mac OS >= 10.7 is required to create an OpenGL "
                 "%d.%d context",
                 attrs->context_major_version,
                 attrs->context_minor_version);
    return false;
}

static bool
cgl_config_fill_pixel_format_attrs(
        const struct cgl_platform *plat,
        const struct wcore_config_attrs *attrs,
        CGLPixelFormatAttribute pixel_attrs[])
{
    int i = 0;

    // CGL does not have an analogue for EGL_DONT_CARE. Instead, one indicates
    // "don't care" by omitting the attribute.
    #define ADD_ATTR(name, value) \
        if ((int)(value) != WAFFLE_DONT_CARE) { \
            pixel_attrs[i++] = (name); \
            pixel_attrs[i++] = (value); \
        }

    // Emulate EGL_KHR_create_context, which allows the implementation to
    // return a context of the latest supported flavor that is
    // backwards-compatibile with the requested flavor.
    if (plat->system_version_full >= 0x0a07) {
        if (wcore_config_attrs_version_eq(attrs, 32)) {
            ADD_ATTR(kCGLPFAOpenGLProfile, (int) kCGLOGLPVersion_3_2_Core);
        }
        else if (wcore_config_attrs_version_eq(attrs, 31)) {
            ADD_ATTR(kCGLPFAOpenGLProfile, (int) kCGLOGLPVersion_3_2_Core);
        }
        else if (wcore_config_attrs_version_le(attrs, 21)) {
            ADD_ATTR(kCGLPFAOpenGLProfile, (int) kCGLOGLPVersion_Legacy);
        }
        else {
            wcore_error_internal("version=%d.%d profile=%#x",
                                 attrs->context_version_major,
                                 attrs->context_version_minor,
                                 attrs->context_profile);
            return false;
        }
    }
    else {
        // The OS doesn't recognize attribute kCGLPFAOpenGLProfile.
        assert(wcore_config_attrs_version_le(attrs, 21));
    }

    ADD_ATTR(kCGLPFAColorSize,          attrs->rgb_size);
    ADD_ATTR(kCGLPFAAlphaSize,          attrs->alpha_size);
    ADD_ATTR(kCGLPFADepthSize,          attrs->depth_size);
    ADD_ATTR(kCGLPFAStencilSize,        attrs->stencil_size);
    ADD_ATTR(kCGLPFASampleBuffers,      attrs->sample_buffers);
    ADD_ATTR(kCGLPFASamples,            attrs->samples);

    if (wcore_config_attrs_version_eq(attrs, 10))
        ADD_ATTR(kCGLPFAAccumSize,      attrs->accum_buffer);

    if (attrs->double_buffered)
        pixel_attrs[i++] = kCGLPFADoubleBuffer;

    pixel_attrs[i++] = 0;

    #undef ADD_ATTR

    return true;
}

struct wcore_config*
cgl_config_choose(struct wcore_platform *wc_plat,
                  struct wcore_display *wc_dpy,
                  const struct wcore_config_attrs *attrs)
{
    struct cgl_platform *plat = cgl_platform(wc_plat);
    struct cgl_config *self;
    bool ok = true;
    int error = 0;
    int ignore;
    CGLPixelFormatAttribute pixel_attrs[64];

    if (!cgl_config_check_attrs(plat, attrs))
        return NULL;

    self = calloc(1, sizeof(*self));
    if (!self) {
        wcore_error(WAFFLE_ERROR_BAD_ALLOC);
        return NULL;
    }

    ok = wcore_config_init(&self->wcore, wc_dpy, attrs);
    if (!ok)
        goto error;

    ok = cgl_config_fill_pixel_format_attrs(plat, attrs, pixel_attrs);
    if (!ok)
        goto error;

    error = CGLChoosePixelFormat(pixel_attrs, &self->pixel_format, &ignore);
    if (error) {
        cgl_error_failed_func("CGLChoosePixelFormat", error);
        goto error;
    }
    if (!self->pixel_format) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "CGLChoosePixelFormat failed to find a pixel format");
        goto error;
    }

    return &self->wcore;

error:
    cgl_config_destroy(&self->wcore);
    return NULL;
}
