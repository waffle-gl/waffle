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

#include "wegl_config.h"
#include "wegl_display.h"
#include "wegl_imports.h"
#include "wegl_platform.h"
#include "wegl_util.h"
#include "wegl_window.h"

/// On Linux, according to eglplatform.h, EGLNativeDisplayType and intptr_t
/// have the same size regardless of platform.
bool
wegl_window_init(struct wegl_window *window,
                 struct wcore_config *wc_config,
                 intptr_t native_window)
{
    struct wegl_config *config = wegl_config(wc_config);
    struct wegl_display *dpy = wegl_display(wc_config->display);
    struct wegl_platform *plat = wegl_platform(dpy->wcore.platform);
    EGLint egl_render_buffer;
    bool ok;

    ok = wcore_window_init(&window->wcore, wc_config);
    if (!ok)
        goto fail;

    if (config->wcore.attrs.double_buffered)
        egl_render_buffer = EGL_BACK_BUFFER;
    else
        egl_render_buffer = EGL_SINGLE_BUFFER;

    EGLint attrib_list[] = {
        EGL_RENDER_BUFFER, egl_render_buffer,
        EGL_NONE,
    };

    window->egl = plat->eglCreateWindowSurface(dpy->egl,
                                               config->egl,
                                               (EGLNativeWindowType)
                                                   native_window,
                                               attrib_list);
    if (!window->egl) {
        wegl_emit_error(plat, "eglCreateWindowSurface");
        goto fail;
    }

    return true;

fail:
    wegl_window_teardown(window);
    return false;
}

bool
wegl_window_teardown(struct wegl_window *window)
{
    struct wegl_display *dpy = wegl_display(window->wcore.display);
    struct wegl_platform *plat = wegl_platform(dpy->wcore.platform);
    bool result = true;

    if (window->egl) {
        bool ok = plat->eglDestroySurface(dpy->egl, window->egl);
        if (!ok) {
            wegl_emit_error(plat, "eglDestroySurface");
            result = false;
        }
    }

    result &= wcore_window_teardown(&window->wcore);
    return result;
}

bool
wegl_window_swap_buffers(struct wcore_window *wc_window)
{
    struct wegl_window *window = wegl_window(wc_window);
    struct wegl_display *dpy = wegl_display(window->wcore.display);
    struct wegl_platform *plat = wegl_platform(dpy->wcore.platform);

    bool ok = plat->eglSwapBuffers(dpy->egl, window->egl);
    if (!ok)
        wegl_emit_error(plat, "eglSwapBuffers");

    return ok;
}
