// Copyright 2014 Emil Velikov
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

#include <stdlib.h>
#include <windows.h>

#include "wcore_error.h"

#include "wgl_config.h"
#include "wgl_context.h"
#include "wgl_display.h"
#include "wgl_dl.h"
#include "wgl_platform.h"
#include "wgl_window.h"

static const struct wcore_platform_vtbl wgl_platform_vtbl;

const char* wfl_class_name = "waffle";

static bool
wgl_platform_destroy(struct wcore_platform *wc_self)
{
    struct wgl_platform *self = wgl_platform(wc_self);
    bool ok = true;

    if (!self)
        return true;

    if (self->dl_gl)
        ok &= wgl_dl_close(wc_self);

    if (self->class_name)
        ok &= UnregisterClass(self->class_name, GetModuleHandle(NULL));

    ok &= wcore_platform_teardown(wc_self);
    free(self);
    return ok;
}

static bool
wgl_platform_register_class(const char* class_name)
{
    WNDCLASS wc;
    bool ok;

    memset(&wc, 0, sizeof(wc));
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    // XXX: Use a non-default window_proc ?
    wc.lpfnWndProc = DefWindowProc;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);;
    wc.lpszClassName = class_name;

    ok = !!RegisterClass(&wc);

    if (!ok) {
        int error = GetLastError();

        if (error) {
            wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                         "RegisterClass() failed: %d",
                         error);
        }
    }

    return ok;
}

struct wcore_platform*
wgl_platform_create(void)
{
    struct wgl_platform *self;
    bool ok;

    self = wcore_calloc(sizeof(*self));
    if (!self)
        return NULL;

    ok = wcore_platform_init(&self->wcore);
    if (!ok)
        goto error;

    ok = wgl_platform_register_class(wfl_class_name);
    if (!ok)
        goto error;
    self->class_name = wfl_class_name;

    self->wcore.vtbl = &wgl_platform_vtbl;
    return &self->wcore;

error:
    wgl_platform_destroy(&self->wcore);
    return NULL;
}

static bool
wgl_make_current(struct wcore_platform *wc_self,
                 struct wcore_display *wc_dpy,
                 struct wcore_window *wc_window,
                 struct wcore_context *wc_ctx)
{
    HDC hDC = wc_window ? wgl_window(wc_window)->hDC : NULL;
    HGLRC hglrc = wc_ctx ? wgl_context(wc_ctx)->hglrc : NULL;

    return wglMakeCurrent(hDC, hglrc);
}

static void*
wgl_get_proc_address(struct wcore_platform *wc_self, const char *name)
{
    return wglGetProcAddress(name);
}

static const struct wcore_platform_vtbl wgl_platform_vtbl = {
    .destroy = wgl_platform_destroy,

    .make_current = wgl_make_current,
    .get_proc_address = wgl_get_proc_address,
    .dl_can_open = wgl_dl_can_open,
    .dl_sym = wgl_dl_sym,

    .display = {
        .connect = wgl_display_connect,
        .destroy = wgl_display_destroy,
        .supports_context_api = wgl_display_supports_context_api,
        .get_native = NULL,
    },

    .config = {
        .choose = wgl_config_choose,
        .destroy = wgl_config_destroy,
        .get_native = NULL,
    },

    .context = {
        .create = wgl_context_create,
        .destroy = wgl_context_destroy,
        .get_native = NULL,
    },

    .window = {
        .create = wgl_window_create,
        .destroy = wgl_window_destroy,
        .show = wgl_window_show,
        .resize = wgl_window_resize,
        .swap_buffers = wgl_window_swap_buffers,
        .get_native = NULL,
    },
};
