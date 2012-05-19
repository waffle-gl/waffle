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

/// @addtogroup wcore_config_attrs
/// @{

/// @file

#include "wcore_config_attrs.h"

#include <stdlib.h>
#include <string.h>

#include <waffle/waffle_attrib_list.h>
#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_error.h>

/// @brief Attribute defaults that apply to all API's.
static const struct wcore_config_attrs wcore_config_attrs_default_all = {
    .color_buffer_size      = 0,
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
            wcore_errorf(WAFFLE_BAD_ATTRIBUTE,
                         "attribute WAFFLE_CONTEXT_API has bad value %#x",
                         context_api);
            return false;
    }
}

/// @brief Check the context attributes.
///
/// The validation done here is independent of the native platform. The native
/// platform may need to do additional checking. For example, GLX should
/// reject the attribute list if the API is GLES1.
static bool
wcore_config_attrs_check_context(struct wcore_config_attrs *attrs)
{
    int version = 10 * attrs->context_major_version
                + attrs->context_minor_version;

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL: {
            switch (attrs->context_profile) {
                case WAFFLE_NONE:
                case WAFFLE_CONTEXT_CORE_PROFILE:
                case WAFFLE_CONTEXT_COMPATIBILITY_PROFILE:
                    break;
                default:
                    goto bad_profile;
            }

            if (version < 10) {
                wcore_errorf(WAFFLE_BAD_ATTRIBUTE,
                             "the minimum OpenGL version is 1.0");
                return false;
            }
            else if (version >= 32 && attrs->context_profile == WAFFLE_NONE) {
                wcore_errorf(WAFFLE_BAD_ATTRIBUTE,
                             "a profile must be given when the OpenGL "
                             "version is >= 3.2");
                return false;
            }

            return true;
        }
        case WAFFLE_CONTEXT_OPENGL_ES1: {
            if (version != 10) {
                wcore_errorf(WAFFLE_BAD_ATTRIBUTE,
                             "the context version must be 1.0 for OpenGL ES1");
                return false;
            }

            if (attrs->context_profile != WAFFLE_NONE)
                goto bad_profile;

            return true;
        }
        case WAFFLE_CONTEXT_OPENGL_ES2: {
            if (version != 20) {
                wcore_errorf(WAFFLE_BAD_ATTRIBUTE,
                             "the context version must 2.0 for OpenGL ES2");
                return false;
            }

            if (attrs->context_profile != WAFFLE_NONE)
                goto bad_profile;

            return true;
        }
        default:
            wcore_errorf(WAFFLE_BAD_ATTRIBUTE,
                         "attribute WAFFLE_CONTEXT_API has bad value %#x",
                         attrs->context_api);
            return false;
    }

bad_profile:
    wcore_errorf(WAFFLE_BAD_ATTRIBUTE,
                 "WAFFLE_CONTEXT_PROFILE has bad value %#x",
                 attrs->context_profile);
    return false;
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
            case WAFFLE_RED_SIZE:
                attrs->red_size = w_value;
                break;
            case WAFFLE_GREEN_SIZE:
                attrs->green_size = w_value;
                break;
            case WAFFLE_BLUE_SIZE:
                attrs->blue_size = w_value;
                break;
            case WAFFLE_ALPHA_SIZE:
                attrs->alpha_size = w_value;
                break;
            case WAFFLE_DEPTH_SIZE:
                attrs->depth_size = w_value;
                break;
            case WAFFLE_STENCIL_SIZE:
                attrs->stencil_size = w_value;
                break;
            case WAFFLE_SAMPLE_BUFFERS:
                attrs->sample_buffers = w_value;
                break;
            case WAFFLE_SAMPLES:
                attrs->samples = w_value;
                break;
            case WAFFLE_DOUBLE_BUFFERED:
                switch (w_value) {
                    case 0:
                        attrs->double_buffered = false;
                        break;
                    case 1:
                        attrs->double_buffered = true;
                        break;
                    default:
                        wcore_errorf(WAFFLE_BAD_ATTRIBUTE,
                                     "WAFFLE_DOUBLE_BUFFERED has bad value "
                                     "0x%x", w_value);
                        return false;
                }
                break;
            case WAFFLE_ACCUM_BUFFER:
                switch (w_value) {
                    case 0:
                        attrs->accum_buffer = false;
                        break;
                    case 1:
                        attrs->accum_buffer = true;
                        break;
                    default:
                        wcore_errorf(WAFFLE_BAD_ATTRIBUTE,
                                     "WAFFLE_ACCUM_BUFFER has bad value %#x",
                                     w_value);
                        return false;
                }
                break;
            default:
                wcore_errorf(WAFFLE_BAD_ATTRIBUTE,
                             "unrecognized attribute 0x%x at attrib_list[%d]",
                             w_attr, i - waffle_attrib_list);
                return false;
        }
    }

    attrs->color_buffer_size = 0;
    if (attrs->red_size != WAFFLE_DONT_CARE)
        attrs->color_buffer_size += attrs->red_size;
    if (attrs->green_size != WAFFLE_DONT_CARE)
        attrs->color_buffer_size += attrs->green_size;
    if (attrs->blue_size != WAFFLE_DONT_CARE)
        attrs->color_buffer_size += attrs->blue_size;
    if (attrs->alpha_size != WAFFLE_DONT_CARE)
        attrs->color_buffer_size += attrs->alpha_size;

    if (!wcore_config_attrs_check_context(attrs))
        return false;

    return attrs;

error_context_api_required:
    wcore_errorf(WAFFLE_BAD_ATTRIBUTE,
                 "the WAFFLE_CONTEXT_API attribute is required");
    return false;
}

/// @}
