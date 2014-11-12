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

#include <windows.h>

#include "wcore_error.h"

#include "wgl_config.h"
#include "wgl_platform.h"
#include "wgl_window.h"

bool
wgl_window_destroy(struct wcore_window *wc_self)
{
    struct wgl_window *self = wgl_window(wc_self);

    assert(self);
    assert(self->hWnd);

    self->created = false;
    ShowWindow(self->hWnd, SW_HIDE);
    return true;
}

bool
wgl_window_priv_destroy(struct wcore_window *wc_self)
{
    struct wgl_window *self = wgl_window(wc_self);
    bool ok = true;

    if (!self)
        return true;

    if (self->hWnd) {
        if (self->hDC) {
            ok &= ReleaseDC(self->hWnd, self->hDC);
        }
        ok &= DestroyWindow(self->hWnd);
    }

    ok &= wcore_window_teardown(wc_self);
    free(self);
    return ok;
}

struct wcore_window*
wgl_window_create(struct wcore_platform *wc_plat,
                  struct wcore_config *wc_config,
                  int width,
                  int height)
{
    struct wgl_config *config = wgl_config(wc_config);
    bool ok;

    assert(config->window);

    // Currently we do not allow multiple windows per config.
    // Neither piglit nor the waffle examples do that yet, so just
    // return NULL in case that ever changes.
    assert(!config->window->created);
    if (config->window->created)
        return NULL;

    config->window->created = true;

    ok = wgl_window_resize(&config->window->wcore, width, height);
    if (!ok)
        return NULL;

    return &config->window->wcore;
}

struct wcore_window*
wgl_window_priv_create(struct wcore_platform *wc_plat,
                       struct wcore_config *wc_config,
                       int width,
                       int height)
{
    struct wgl_platform *plat = wgl_platform(wc_plat);
    struct wgl_window *self;
    bool ok;
    RECT rect;

    self = wcore_calloc(sizeof(*self));
    if (!self)
        return NULL;

    ok = wcore_window_init(&self->wcore, wc_config);
    if (!ok)
        goto error;

    rect.left = 0;
    rect.top = 0;
    rect.right = rect.left + width;
    rect.bottom = rect.top + height;

    ok = AdjustWindowRect(&rect, WS_POPUPWINDOW, FALSE);
    if (!ok)
        goto error;

    self->hWnd = CreateWindow(plat->class_name, NULL, WS_POPUPWINDOW,
                              0, 0,
                              rect.right - rect.left, rect.bottom - rect.top,
                              NULL, NULL, NULL, NULL);
    if (!self->hWnd)
        goto error;

    self->hDC = GetDC(self->hWnd);
    if (!self->hDC)
        goto error;

    return &self->wcore;

error:
    wgl_window_priv_destroy(&self->wcore);
    return NULL;
}

bool
wgl_window_show(struct wcore_window *wc_self)
{
    struct wgl_window *self = wgl_window(wc_self);

    assert(self);
    assert(self->hWnd);

    // If the window was previously hidden the function returns zero,
    // and non-zero otherwise.
    // XXX: Use SW_SHOW or SW_SHOWDEFAULT, SW_SHOWNORMAL ?
    ShowWindow(self->hWnd, SW_SHOW);
    return true;
}

bool
wgl_window_resize(struct wcore_window *wc_self,
                  int32_t width, int32_t height)
{
    struct wgl_window *self = wgl_window(wc_self);
    RECT rect;
    bool ok;

    assert(self);
    assert(self->hWnd);

    rect.left = 0;
    rect.top = 0;
    rect.right = rect.left + width;
    rect.bottom = rect.top + height;

    ok = AdjustWindowRect(&rect, WS_POPUPWINDOW, FALSE);
    if (!ok)
        return false;

    ok = SetWindowPos(self->hWnd, 0, 0, 0,
                      rect.right - rect.left,
                      rect.bottom - rect.top,
                      SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);

#ifdef DEBUG
    // Verify the client area size matches the required size.

    GetClientRect(self->hWnd, &rect);
    assert(rect.left == 0);
    assert(rect.top == 0);
    assert(rect.right - rect.left == width);
    assert(rect.bottom - rect.top == height);
#endif
    return ok;
}

bool
wgl_window_swap_buffers(struct wcore_window *wc_self)
{
    struct wgl_window *self = wgl_window(wc_self);

    assert(self);
    assert(self->hDC);

    return SwapBuffers(self->hDC);
}
