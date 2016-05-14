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

#include "wcore_error.h"
#include "wcore_platform.h"

#include "wegl_display.h"
#include "wegl_imports.h"
#include "wegl_util.h"
#include "wegl_platform.h"

static bool
get_apis(struct wegl_display *dpy)
{
    struct wegl_platform *plat = wegl_platform(dpy->wcore.platform);
    const char *apis = plat->eglQueryString(dpy->egl, EGL_CLIENT_APIS);

    // Our minimum requirement - EGL 1.2 ...
    if (dpy->major_version != 1 || dpy->minor_version < 2) {
        wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                     "EGL 1.2 or later is required");
        return false;
    }

    // ... plus working eglQueryString(EGL_CLIENT_APIS).
    if (!apis) {
        wegl_emit_error(plat, "eglQueryString(EGL_CLIENT_APIS)");
        return false;
    }

    // waffle_is_extension_in_string() resets the error state. That's ok,
    // however, because if we've reached this point then no error should be
    // pending emission.
    assert(wcore_error_get_code() == 0);

    if (waffle_is_extension_in_string(apis, "OpenGL_ES"))
        dpy->api_mask |= WEGL_OPENGL_ES_API;

    // Check for "OpenGL" if we're running EGL 1.4 or later.
    if (dpy->major_version == 1 && dpy->minor_version >= 4)
        if (waffle_is_extension_in_string(apis, "OpenGL"))
            dpy->api_mask |= WEGL_OPENGL_API;

    return true;
}

static bool
get_extensions(struct wegl_display *dpy)
{
    struct wegl_platform *plat = wegl_platform(dpy->wcore.platform);
    const char *extensions = plat->eglQueryString(dpy->egl, EGL_EXTENSIONS);

    if (!extensions) {
        wegl_emit_error(plat, "eglQueryString(EGL_EXTENSIONS)");
        return false;
    }

    // waffle_is_extension_in_string() resets the error state. That's ok,
    // however, because if we've reached this point then no error should be
    // pending emission.
    assert(wcore_error_get_code() == 0);

    dpy->EXT_create_context_robustness = waffle_is_extension_in_string(extensions, "EGL_EXT_create_context_robustness");
    dpy->KHR_create_context = waffle_is_extension_in_string(extensions, "EGL_KHR_create_context");

    return true;
}

/// On Linux, according to eglplatform.h, EGLNativeDisplayType and intptr_t
/// have the same size regardless of platform.
bool
wegl_display_init(struct wegl_display *dpy,
                  struct wcore_platform *wc_plat,
                  intptr_t native_display)
{
    struct wegl_platform *plat = wegl_platform(wc_plat);
    bool ok;

    ok = wcore_display_init(&dpy->wcore, wc_plat);
    if (!ok)
        goto fail;

    dpy->egl = plat->eglGetDisplay((EGLNativeDisplayType) native_display);
    if (!dpy->egl) {
        wegl_emit_error(plat, "eglGetDisplay");
        goto fail;
    }

    ok = plat->eglInitialize(dpy->egl, &dpy->major_version, &dpy->minor_version);
    if (!ok) {
        wegl_emit_error(plat, "eglInitialize");
        goto fail;
    }

    ok = get_apis(dpy);
    if (!ok)
        goto fail;

    ok = get_extensions(dpy);
    if (!ok)
        goto fail;

    return true;

fail:
    wegl_display_teardown(dpy);
    return false;
}

bool
wegl_display_teardown(struct wegl_display *dpy)
{
    struct wegl_platform *plat = wegl_platform(dpy->wcore.platform);
    bool ok = true;

    if (dpy->egl) {
        ok = plat->eglTerminate(dpy->egl);
        if (!ok)
            wegl_emit_error(plat, "eglTerminate");
    }

    return ok;
}

bool
wegl_display_supports_context_api(struct wcore_display *wc_dpy,
                                  int32_t waffle_context_api)
{
    struct wegl_display *dpy = wegl_display(wc_dpy);

    switch (waffle_context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            return !!(dpy->api_mask & WEGL_OPENGL_API);
        case WAFFLE_CONTEXT_OPENGL_ES1:
        case WAFFLE_CONTEXT_OPENGL_ES2:
            return !!(dpy->api_mask & WEGL_OPENGL_ES_API);
        case WAFFLE_CONTEXT_OPENGL_ES3:
            return !!(dpy->api_mask & WEGL_OPENGL_ES_API) &&
                   dpy->KHR_create_context;
        default:
            wcore_error_internal("waffle_context_api has bad value %#x",
                                 waffle_context_api);
            return false;
    }
}
