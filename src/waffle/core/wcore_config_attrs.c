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
};

static void
wcore_config_attrs_set_defaults(struct wcore_config_attrs *attrs)
{
    memcpy(attrs, &wcore_config_attrs_default_all, sizeof(*attrs));
}

bool
wcore_config_attrs_parse(
      const int32_t waffle_attrib_list[],
      struct wcore_config_attrs *attrs)
{
    const int32_t *i;

    wcore_config_attrs_set_defaults(attrs);

    if (!waffle_attrib_list) {
        // Nothing to parse. Just return defaults.
        return attrs;
    }

    for (i = waffle_attrib_list; *i != 0; i += 2) {
        int32_t w_attr = i[0];
        int32_t w_value = i[1];

        switch (w_attr) {
            case WAFFLE_CONTEXT_API:
                // Don't process this attribute yet. But don't emit an error
                // either. Just silently ignore it. This allows some tests to
                // be preemptively fixed before wcore_config_attrs_parse
                // requires this attribute.
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

    return attrs;
}

/// @}
