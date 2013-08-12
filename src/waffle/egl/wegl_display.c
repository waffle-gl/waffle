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
#include "wegl_util.h"

static bool
get_extensions(struct wegl_display *dpy)
{
    const char *extensions = eglQueryString(dpy->egl, EGL_EXTENSIONS);

    if (!extensions) {
	wegl_emit_error("eglQueryString(EGL_EXTENSIONS");
	return false;
    }

    // waffle_is_extension_in_string() resets the error state. That's ok,
    // however, because if we've reached this point then no error should be
    // pending emission.
    assert(wcore_error_get_code() == 0);

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
    bool ok;
    EGLint major, minor;

    ok = wcore_display_init(&dpy->wcore, wc_plat);
    if (!ok)
        goto fail;

    dpy->egl = eglGetDisplay((EGLNativeDisplayType) native_display);
    if (!dpy->egl) {
        wegl_emit_error("eglGetDisplay");
        goto fail;
    }

    ok = eglInitialize(dpy->egl, &major, &minor);
    if (!ok) {
        wegl_emit_error("eglInitialize");
        goto fail;
    }

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
    bool ok = true;

    if (dpy->egl) {
        ok = eglTerminate(dpy->egl);
        if (!ok)
            wegl_emit_error("eglTerminate");
    }

    return ok;
}

bool
wegl_display_supports_context_api(struct wcore_display *wc_dpy,
                                  int32_t waffle_context_api)
{
    struct wegl_display *dpy = wegl_display(wc_dpy);
    struct wcore_platform *wc_plat = dpy->wcore.platform;
    int32_t waffle_dl;

    switch (waffle_context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            waffle_dl = WAFFLE_DL_OPENGL;
            break;
        case WAFFLE_CONTEXT_OPENGL_ES1:
            waffle_dl = WAFFLE_DL_OPENGL_ES1;
            break;
        case WAFFLE_CONTEXT_OPENGL_ES2:
            waffle_dl = WAFFLE_DL_OPENGL_ES2;
            break;
        case WAFFLE_CONTEXT_OPENGL_ES3:
            if (!dpy->KHR_create_context)
                return false;

            waffle_dl = WAFFLE_DL_OPENGL_ES3;
            break;
        default:
            wcore_error_internal("waffle_context_api has bad value %#x",
                                 waffle_context_api);
            return false;
    }

    return wc_plat->vtbl->dl_can_open(wc_plat, waffle_dl);
}
