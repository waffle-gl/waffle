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

#include "wcore_error.h"

#include "wegl_context.h"
#include "wegl_display.h"
#include "wegl_imports.h"
#include "wegl_platform.h"
#include "wegl_util.h"
#include "wegl_window.h"

void
wegl_emit_error(struct wegl_platform *plat, const char *egl_func_call)
{
    EGLint egl_error_code = plat->eglGetError();
    const char *egl_error_name;

    switch (egl_error_code) {
#define CASE(x) case x: egl_error_name = #x; break
        CASE(EGL_FALSE);
        CASE(EGL_TRUE);
        CASE(EGL_DONT_CARE);
        CASE(EGL_SUCCESS);
        CASE(EGL_NOT_INITIALIZED);
        CASE(EGL_BAD_ACCESS);
        CASE(EGL_BAD_ALLOC);
        CASE(EGL_BAD_ATTRIBUTE);
        CASE(EGL_BAD_CONFIG);
        CASE(EGL_BAD_CONTEXT);
        CASE(EGL_BAD_CURRENT_SURFACE);
        CASE(EGL_BAD_DISPLAY);
        CASE(EGL_BAD_MATCH);
        CASE(EGL_BAD_NATIVE_PIXMAP);
        CASE(EGL_BAD_NATIVE_WINDOW);
        CASE(EGL_BAD_PARAMETER);
        CASE(EGL_BAD_SURFACE);
        CASE(EGL_CONTEXT_LOST);
        default: egl_error_name = ""; break;
#undef CASE
    }

    wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                 "%s failed with error %s(0x%x)",
                 egl_func_call,
                 egl_error_name,
                 egl_error_code);
}

bool
wegl_make_current(struct wcore_platform *wc_plat,
                  struct wcore_display *wc_dpy,
                  struct wcore_window *wc_window,
                  struct wcore_context *wc_ctx)
{
    struct wegl_platform *plat = wegl_platform(wc_plat);
    EGLSurface surface = wc_window ? wegl_window(wc_window)->egl : NULL;
    bool ok;

    ok = plat->eglMakeCurrent(wegl_display(wc_dpy)->egl,
                              surface,
                              surface,
                              wc_ctx
                                  ? wegl_context(wc_ctx)->egl
                                  : NULL);
    if (!ok)
        wegl_emit_error(plat, "eglMakeCurrent");

    return ok;
}

void*
wegl_get_proc_address(struct wcore_platform *wc_self, const char *name)
{
    struct wegl_platform *self = wegl_platform(wc_self);
    return self->eglGetProcAddress(name);
}
