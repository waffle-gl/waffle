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

/// @addtogroup cgl_config
/// @{

/// @file

#include "cgl_config.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <waffle/native.h>
#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_config_attrs.h>
#include <waffle/core/wcore_error.h>

#include "cgl_error.h"

/// @brief Check the values of `attrs->context_*`.
static bool
cgl_config_check_attrs(const struct wcore_config_attrs *attrs)
{
    int context_version = 10 * attrs->context_major_version
                        + attrs->context_minor_version;

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            switch (context_version) {
                case 10:
                    return true;
                case 32:
                    switch (attrs->context_profile) {
                        case WAFFLE_CONTEXT_CORE_PROFILE:
                            return true;
                        case WAFFLE_CONTEXT_COMPATIBILITY_PROFILE:
                            wcore_errorf(WAFFLE_UNSUPPORTED_ON_PLATFORM,
                                         "CGL does not support the OpenGL 3.2 "
                                         "Compatibility Profile");
                            return false;
                        default:
                            assert(false);
                            return false;
                    }
                default:
                    wcore_errorf(WAFFLE_UNSUPPORTED_ON_PLATFORM,
                                 "On CGL, the requested OpenGL version must "
                                 "be 1.0 or 3.2");
                    assert(false);
                    return false;
            }
        case WAFFLE_CONTEXT_OPENGL_ES1:
            wcore_errorf(WAFFLE_UNSUPPORTED_ON_PLATFORM,
                         "CGL does not support OpenGL ES1");
            return false;
        case WAFFLE_CONTEXT_OPENGL_ES2:
            wcore_errorf(WAFFLE_UNSUPPORTED_ON_PLATFORM,
                         "CGL does not support OpenGL ES2");
            return false;
        default:
            assert(false);
            return false;
    }
}

static bool
cgl_config_fill_pixel_format_attrs(
        const struct wcore_config_attrs *attrs,
        CGLPixelFormatAttribute pixel_attrs[])
{
    int i = 0;
    int context_version = 10 * attrs->context_major_version
                        + attrs->context_minor_version;

    // CGL does not have an analogue for EGL_DONT_CARE. Instead, one indicates
    // "don't care" by omitting the attribute.
    #define ADD_ATTR(name, value) \
        if ((value) != WAFFLE_DONT_CARE) { \
            pixel_attrs[i++] = (name); \
            pixel_attrs[i++] = (value); \
        }

    if (context_version == 10) {
        ADD_ATTR(kCGLPFAOpenGLProfile, (int) kCGLOGLPVersion_Legacy);
    }
    else if (context_version == 32
             && attrs->context_profile == WAFFLE_CONTEXT_CORE_PROFILE) {
        ADD_ATTR(kCGLPFAOpenGLProfile, (int) kCGLOGLPVersion_3_2_Core);
    }
    else {
        wcore_error_internal("version=%d profile=%#x",
                             context_version,
                             attrs->context_profile);
        return false;
    }

    ADD_ATTR(kCGLPFAColorSize,          attrs->rgb_size);
    ADD_ATTR(kCGLPFAAlphaSize,          attrs->alpha_size);
    ADD_ATTR(kCGLPFADepthSize,          attrs->depth_size);
    ADD_ATTR(kCGLPFAStencilSize,        attrs->stencil_size);
    ADD_ATTR(kCGLPFASampleBuffers,      attrs->sample_buffers);
    ADD_ATTR(kCGLPFASamples,            attrs->samples);

    if (context_version == 10)
        ADD_ATTR(kCGLPFAAccumSize,      attrs->accum_buffer);

    if (attrs->double_buffered)
        pixel_attrs[i++] = kCGLPFADoubleBuffer;

    pixel_attrs[i++] = 0;

    #undef ADD_ATTR

    return true;
}

union native_config*
cgl_config_choose(
        union native_display *display,
        const struct wcore_config_attrs *attrs)
{
    bool ok = true;
    int error = 0;
    int ignore;
    CGLPixelFormatAttribute pixel_attrs[64];

    if (!cgl_config_check_attrs(attrs))
        return NULL;

    union native_config *self;
    NATIVE_ALLOC(self, cgl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    ok = cgl_config_fill_pixel_format_attrs(attrs, pixel_attrs);
    if (!ok)
        goto error;

    error = CGLChoosePixelFormat(pixel_attrs, &self->cgl->pixel_format, &ignore);
    if (error) {
        cgl_error_failed_func("CGLChoosePixelFormat", error);
        goto error;
    }
    if (!self->cgl->pixel_format) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "CGLChoosePixelFormat failed to find a pixel format");
        goto error;
    }

    goto end;

error:
    cgl_config_destroy(self);
    self = NULL;

end:
    return self;
}

bool
cgl_config_destroy(union native_config *self)
{
    if (!self)
        return true;

    if (self->cgl->pixel_format)
        CGLReleasePixelFormat(self->cgl->pixel_format);

    free(self);
    return true;
}

/// @}
