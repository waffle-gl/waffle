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

/// @addtogroup wcore_config_attrs
/// @{

/// @file

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "waffle_attrib_list.h"
#include "waffle_enum.h"

#include "wcore_config_attrs.h"
#include "wcore_error.h"

/// @brief Attribute defaults that apply to all API's.
static const struct wcore_config_attrs wcore_config_attrs_default_all = {
    .rgba_size      = 0,
    .red_size               = WAFFLE_DONT_CARE,
    .green_size             = WAFFLE_DONT_CARE,
    .blue_size              = WAFFLE_DONT_CARE,
    .alpha_size             = WAFFLE_DONT_CARE,

    .depth_size             = WAFFLE_DONT_CARE,
    .stencil_size           = WAFFLE_DONT_CARE,

    .sample_buffers         = 0,
    .samples                = 0,

    .double_buffered        = true,

    .accum_buffer           = false,
};

static bool
wcore_config_attrs_set_defaults(
        int32_t context_api,
        struct wcore_config_attrs *attrs)
{
    memcpy(attrs, &wcore_config_attrs_default_all, sizeof(*attrs));

    switch (context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            attrs->context_major_version = 1;
            attrs->context_minor_version = 0;
            attrs->context_profile = WAFFLE_CONTEXT_CORE_PROFILE;
            return true;
        case WAFFLE_CONTEXT_OPENGL_ES1:
            attrs->context_major_version = 1;
            attrs->context_minor_version = 0;
            attrs->context_profile = WAFFLE_NONE;
            return true;
        case WAFFLE_CONTEXT_OPENGL_ES2:
            attrs->context_major_version = 2;
            attrs->context_minor_version = 0;
            attrs->context_profile = WAFFLE_NONE;
            return true;
        default:
            wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                         "WAFFLE_CONTEXT_API has bad value %#x",
                         context_api);
            return false;
    }
}

/// @brief Check context attributes when api is OpenGL.
static bool
check_gl_context(struct wcore_config_attrs *attrs)
{
    if (attrs->context_full_version < 10) {
        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                     "for OpenGL, the requested context version "
                     "must be >= 1.0");
        return false;
    }
    else if (attrs->context_full_version >= 32 &&
             attrs->context_profile != WAFFLE_CONTEXT_CORE_PROFILE &&
             attrs->context_profile != WAFFLE_CONTEXT_COMPATIBILITY_PROFILE) {
        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                     "for OpenGL >= 3.2,"
                     "WAFFLE_CONTEXT_PROFILE must be either "
                     "WAFFLE_CONTEXT_CORE_PROFILE or "
                     "WAFFLE_CONTEXT_COMPATIBILITY_PROFILE");
        return false;
    }
    else if (attrs->context_full_version >= 32
             && attrs->context_profile == WAFFLE_CONTEXT_CORE_PROFILE
             && attrs->accum_buffer) {
        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                     "core profiles do not support accumulation "
                     "buffers");
        return false;
    }

    return true;
}
/// @brief Check context attributes when api is OpenGL ESx.
static bool
check_es_context(struct wcore_config_attrs *attrs)
{
    if (attrs->context_profile != WAFFLE_NONE) {
        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                     "for OpenGL ES, WAFFLE_CONTEXT_PROFILE must be "
                     "WAFFLE_NONE");
        return false;
    }

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL_ES1:
            if (attrs->context_full_version != 10 &&
                attrs->context_full_version != 11)
            {
                wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                             "for OpenGL ES1, the requested context version "
                             "must be 1.0 or 1.1");
                return false;
            }

            return true;
        case WAFFLE_CONTEXT_OPENGL_ES2:
            if (attrs->context_major_version != 2) {
                wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                             "for OpenGL ES2, the requested major context "
                             "version must be 2");
                return false;
            }

            return true;
        default:
            assert(0);
            return false;
    }
}

/// @brief Check the context attributes.
///
/// The validation done here is independent of the native platform. The native
/// platform may need to do additional checking. For example, GLX should
/// reject the attribute list if the API is GLES1.
static bool
check_context(struct wcore_config_attrs *attrs)
{
    if (attrs->context_minor_version < 0) {
        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                     "context minor version must be >= 0");
        return false;
    }

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            return check_gl_context(attrs);
        case WAFFLE_CONTEXT_OPENGL_ES1:
        case WAFFLE_CONTEXT_OPENGL_ES2:
            return check_es_context(attrs);
        default:
            wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                         "WAFFLE_CONTEXT_API has bad value %#x",
                         attrs->context_api);
            return false;
    }
}

bool
wcore_config_attrs_parse(
      const int32_t waffle_attrib_list[],
      struct wcore_config_attrs *attrs)
{
    bool ok;
    const int32_t *i;
    int32_t context_api;

    if (waffle_attrib_list == NULL)
        goto error_context_api_required;

    ok = waffle_attrib_list_get(waffle_attrib_list,
                                WAFFLE_CONTEXT_API,
                                &context_api);
    if (!ok)
        goto error_context_api_required;

    ok = wcore_config_attrs_set_defaults(context_api, attrs);
    if (!ok)
        return false;

    for (i = waffle_attrib_list; *i != 0; i += 2) {
        int32_t w_attr = i[0];
        int32_t w_value = i[1];

        switch (w_attr) {

            #define CASE_INT(enum_name, struct_memb)                           \
                case enum_name:                                                \
                    if (w_value < -1) {                                        \
                        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,               \
                                     #enum_name " has bad value %d", w_value); \
                                     return false;                             \
                    }                                                          \
                    attrs->struct_memb = w_value;                              \
                    break;

            #define CASE_BOOL(enum_name, struct_memb)                              \
                case enum_name:                                                    \
                    if (w_value != true && w_value != false) {                     \
                        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,                   \
                                     #enum_name " has bad value 0x%x", w_value);   \
                                     return false;                                 \
                    }                                                              \
                    attrs->struct_memb = w_value;                                  \
                    break;

            case WAFFLE_CONTEXT_API:
                attrs->context_api = w_value;
                break;
            case WAFFLE_CONTEXT_MAJOR_VERSION:
                attrs->context_major_version = w_value;
                break;
            case WAFFLE_CONTEXT_MINOR_VERSION:
                attrs->context_minor_version = w_value;
                break;
            case WAFFLE_CONTEXT_PROFILE:
                attrs->context_profile = w_value;
                break;

            CASE_INT(WAFFLE_RED_SIZE, red_size)
            CASE_INT(WAFFLE_GREEN_SIZE, green_size)
            CASE_INT(WAFFLE_BLUE_SIZE, blue_size)
            CASE_INT(WAFFLE_ALPHA_SIZE, alpha_size)
            CASE_INT(WAFFLE_DEPTH_SIZE, depth_size)
            CASE_INT(WAFFLE_STENCIL_SIZE, stencil_size)
            CASE_INT(WAFFLE_SAMPLES, samples)

            CASE_BOOL(WAFFLE_SAMPLE_BUFFERS, sample_buffers);
            CASE_BOOL(WAFFLE_DOUBLE_BUFFERED, double_buffered);
            CASE_BOOL(WAFFLE_ACCUM_BUFFER, accum_buffer);

            default:
                wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                             "unrecognized attribute 0x%x at attrib_list[%d]",
                             w_attr, i - waffle_attrib_list);
                return false;
        }

        #undef CASE_INT
        #undef CASE_BOOL
    }

    attrs->rgb_size = 0;
    if (attrs->red_size != WAFFLE_DONT_CARE)
        attrs->rgb_size += attrs->red_size;
    if (attrs->green_size != WAFFLE_DONT_CARE)
        attrs->rgb_size += attrs->green_size;
    if (attrs->blue_size != WAFFLE_DONT_CARE)
        attrs->rgb_size += attrs->blue_size;

    attrs->rgba_size = attrs->rgb_size;
    if (attrs->alpha_size != WAFFLE_DONT_CARE)
        attrs->rgba_size += attrs->alpha_size;

    attrs->context_full_version = 10 * attrs->context_major_version
                                + attrs->context_minor_version;

    if (!check_context(attrs))
        return false;

    return true;

error_context_api_required:
    wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                 "attribute WAFFLE_CONTEXT_API is required but missing");
    return false;
}

/// @}
