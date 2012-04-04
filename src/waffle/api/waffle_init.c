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

#include <waffle/waffle_init.h>

#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_error.h>
#include <waffle/core/wcore_platform.h>

#include "api_priv.h"

static bool
waffle_init_parse_attrib_list(
        const int32_t attrib_list[],
        int *platform,
        int *gl_api)
{
    const int32_t *i;

    // Set defaults.
    *platform = WAFFLE_PLATFORM_DEFAULT;
    *gl_api = WAFFLE_GL_API_DEFAULT;

    for (i = attrib_list; *i != 0; i += 2) {
        const int32_t attr = i[0];
        const int32_t value = i[1];

        switch (attr) {
            case WAFFLE_PLATFORM:
                switch (value) {
#ifdef WAFFLE_HAS_ANDROID
                    case WAFFLE_PLATFORM_ANDROID:
#endif
#ifdef WAFFLE_HAS_COCOA
                    case WAFFLE_PLATFORM_COCOA:
#endif
#ifdef WAFFLE_HAS_GLX
                    case WAFFLE_PLATFORM_GLX:
#endif
#ifdef WAFFLE_HAS_WAYLAND
                    case  WAFFLE_PLATFORM_WAYLAND:
#endif
#ifdef WAFFLE_HAS_X11_EGL
                    case WAFFLE_PLATFORM_X11_EGL:
#endif
                        *platform = value;
                        break;
                    default:
                        wcore_errorf(WAFFLE_BAD_ATTRIBUTE,
                                     "WAFFLE_PLATFORM has bad value 0x%x",
                                     value);
                        return false;
                }

                break;
            case WAFFLE_GL_API:
                switch (value) {
                    case WAFFLE_GL:
                    case WAFFLE_GLES1:
                    case WAFFLE_GLES2:
                        *gl_api = value;
                        break;
                    default:
                        wcore_errorf(WAFFLE_BAD_ATTRIBUTE,
                                     "WAFFLE_GL_API has bad value 0x%x",
                                     value);
                        return false;
                }
                break;
            default:
                wcore_errorf(WAFFLE_BAD_ATTRIBUTE, "bad attribute 0x%x", attr);
                return false;
                break;
        }
    }

    return true;
}

bool
waffle_init(const int32_t *attrib_list)
{
    bool ok = true;
    int platform;
    int gl_api;

    if (api_current_platform) {
        wcore_error(WAFFLE_ALREADY_INITIALIZED);
        return false;
    }

    ok &= waffle_init_parse_attrib_list(attrib_list, &platform, &gl_api);
    if (!ok)
        return false;

    api_current_platform = wcore_platform_create(platform, gl_api);
    if (!api_current_platform)
        return false;

    return true;
}

bool
waffle_finish(void)
{
    bool ok = wcore_platform_destroy(api_current_platform);
    api_current_platform = NULL;
    return ok;
}
