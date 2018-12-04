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

#include "api_priv.h"

#include "wcore_error.h"
#include "wcore_platform.h"

struct wcore_platform* cgl_platform_create(void);
struct wcore_platform* droid_platform_create(void);
struct wcore_platform* glx_platform_create(void);
struct wcore_platform* wayland_platform_create(void);
struct wcore_platform* xegl_platform_create(void);
struct wcore_platform* wgbm_platform_create(void);
struct wcore_platform* wgl_platform_create(void);
struct wcore_platform* nacl_platform_create(void);
struct wcore_platform* sl_platform_create(void);
struct wcore_platform* qnx_platform_create(void);

static bool
waffle_init_parse_attrib_list(
        const int32_t attrib_list[],
        int *platform)
{
    bool found_platform = false;

    for (const int32_t *i = attrib_list; *i != 0; i += 2) {
        const int32_t attr = i[0];
        const int32_t value = i[1];

        switch (attr) {
            case WAFFLE_PLATFORM:
                switch (value) {
                    #define CASE_DEFINED_PLATFORM(name) \
                        case WAFFLE_PLATFORM_##name : \
                            found_platform = true; \
                            *platform = value; \
                            break;

                    #define CASE_UNDEFINED_PLATFORM(name) \
                        case WAFFLE_PLATFORM_##name : \
                            wcore_errorf(WAFFLE_ERROR_BUILT_WITHOUT_SUPPORT, \
                                         "waffle was built without support for WAFFLE_PLATFORM_" #name); \
                            return false;

#ifdef WAFFLE_HAS_ANDROID
                    CASE_DEFINED_PLATFORM(ANDROID)
#else
                    CASE_UNDEFINED_PLATFORM(ANDROID)
#endif

#ifdef WAFFLE_HAS_CGL
                    CASE_DEFINED_PLATFORM(CGL)
#else
                    CASE_UNDEFINED_PLATFORM(CGL)
#endif

#ifdef WAFFLE_HAS_GLX
                    CASE_DEFINED_PLATFORM(GLX)
#else
                    CASE_UNDEFINED_PLATFORM(GLX)
#endif

#ifdef WAFFLE_HAS_WAYLAND
                    CASE_DEFINED_PLATFORM(WAYLAND)
#else
                    CASE_UNDEFINED_PLATFORM(WAYLAND)
#endif

#ifdef WAFFLE_HAS_X11_EGL
                    CASE_DEFINED_PLATFORM(X11_EGL)
#else
                    CASE_UNDEFINED_PLATFORM(X11_EGL)
#endif

#ifdef WAFFLE_HAS_GBM
                    CASE_DEFINED_PLATFORM(GBM)
#else
                    CASE_UNDEFINED_PLATFORM(GBM)
#endif

#ifdef WAFFLE_HAS_WGL
                    CASE_DEFINED_PLATFORM(WGL)
#else
                    CASE_UNDEFINED_PLATFORM(WGL)
#endif

#ifdef WAFFLE_HAS_NACL
                    CASE_DEFINED_PLATFORM(NACL)
#else
                    CASE_UNDEFINED_PLATFORM(NACL)
#endif

#ifdef WAFFLE_HAS_SURFACELESS_EGL
                    CASE_DEFINED_PLATFORM(SURFACELESS_EGL)
#else
                    CASE_UNDEFINED_PLATFORM(SURFACELESS_EGL)
#endif

#ifdef WAFFLE_HAS_QNX
                    CASE_DEFINED_PLATFORM(QNX)
#else
                    CASE_UNDEFINED_PLATFORM(QNX)
#endif

                    default:
                        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                                     "WAFFLE_PLATFORM has bad value 0x%x",
                                     value);
                        return false;

                    #undef CASE_DEFINED_PLATFORM
                    #undef CASE_UNDEFINED_PLATFORM
                }

                break;
            default:
                wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                             "bad attribute name %#x", attr);
                return false;
                break;
        }
    }

    if (!found_platform) {
        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                     "attribute list is missing WAFFLE_PLATFORM");
        return false;
    }

    return true;
}

static struct wcore_platform*
waffle_init_create_platform(int32_t waffle_platform)
{
    struct wcore_platform *wc_platform = NULL;

    switch (waffle_platform) {
#ifdef WAFFLE_HAS_ANDROID
        case WAFFLE_PLATFORM_ANDROID:
            wc_platform = droid_platform_create();
            break;
#endif
#ifdef WAFFLE_HAS_CGL
        case WAFFLE_PLATFORM_CGL:
            wc_platform = cgl_platform_create();
            break;
#endif
#ifdef WAFFLE_HAS_GLX
        case WAFFLE_PLATFORM_GLX:
            wc_platform = glx_platform_create();
            break;
#endif
#ifdef WAFFLE_HAS_WAYLAND
        case  WAFFLE_PLATFORM_WAYLAND:
            wc_platform = wayland_platform_create();
            break;
#endif
#ifdef WAFFLE_HAS_X11_EGL
        case WAFFLE_PLATFORM_X11_EGL:
            wc_platform = xegl_platform_create();
            break;
#endif
#ifdef WAFFLE_HAS_GBM
        case WAFFLE_PLATFORM_GBM:
            wc_platform = wgbm_platform_create();
            break;
#endif
#ifdef WAFFLE_HAS_WGL
        case WAFFLE_PLATFORM_WGL:
            wc_platform = wgl_platform_create();
            break;
#endif
#ifdef WAFFLE_HAS_NACL
        case WAFFLE_PLATFORM_NACL:
            wc_platform = nacl_platform_create();
            break;
#endif
#ifdef WAFFLE_HAS_SURFACELESS_EGL
        case WAFFLE_PLATFORM_SURFACELESS_EGL:
            wc_platform = sl_platform_create();
            break;
#endif
#ifdef WAFFLE_HAS_QNX
        case WAFFLE_PLATFORM_QNX:
            wc_platform = qnx_platform_create();
            break;
#endif
        default:
            assert(false);
            return NULL;
    }

    if (wc_platform)
        wc_platform->waffle_platform = waffle_platform;

    return wc_platform;
}

WAFFLE_API bool
waffle_init(const int32_t *attrib_list)
{
    bool ok = true;
    int platform;

    wcore_error_reset();

    if (api_platform) {
        wcore_error(WAFFLE_ERROR_ALREADY_INITIALIZED);
        return false;
    }

    ok &= waffle_init_parse_attrib_list(attrib_list, &platform);
    if (!ok)
        return false;

    api_platform = waffle_init_create_platform(platform);
    if (!api_platform)
        return false;

    return true;
}

WAFFLE_API bool
waffle_teardown(void)
{
    bool ok = true;

    wcore_error_reset();

    if (!api_platform) {
        wcore_error(WAFFLE_ERROR_NOT_INITIALIZED);
        return false;
    }

    ok &= api_platform->vtbl->destroy(api_platform);
    if (!ok)
        return false;

    api_platform = NULL;
    return true;
}
