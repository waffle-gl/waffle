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

#include "wcore_config_attrs.h"

#include <stdlib.h>
#include <string.h>

#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_error.h>

bool
wcore_config_attrs_parse(
      const int32_t waffle_attrib_list[],
      struct wcore_config_attrs *attrs)
{
    const int32_t *i;

    // Set defaults.
    memset(attrs, 0, sizeof(*attrs));
    attrs->double_buffered = true;

    if (!waffle_attrib_list) {
        // Nothing to parse. Just return defaults.
        return attrs;
    }

    for (i = waffle_attrib_list; *i != 0; i += 2) {
        int32_t w_attr = i[0];
        int32_t w_value = i[1];

        switch (w_attr) {
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

    attrs->color_buffer_size = attrs->red_size
                             + attrs->green_size
                             + attrs->blue_size
                             + attrs->alpha_size;

    return attrs;
}