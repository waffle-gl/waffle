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

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "wcore_config_attrs.h"
#include "wcore_error.h"

#include "wgl_config.h"
#include "wgl_display.h"
#include "wgl_error.h"
#include "wgl_platform.h"
#include "wgl_window.h"

bool
wgl_config_destroy(struct wcore_config *wc_self)
{
    struct wgl_config *self = wgl_config(wc_self);
    bool ok = true;

    if (!self)
        return true;

    if (self->window)
        ok &= wgl_window_priv_destroy(&self->window->wcore);

    ok &= wcore_config_teardown(wc_self);
    free(self);
    return ok;
}

static void
wgl_config_set_pixeldescriptor(struct wgl_config *config,
                               const struct wcore_config_attrs *attrs)
{
    PIXELFORMATDESCRIPTOR *pfd = &config->pfd;

    pfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd->nVersion = 1;

    pfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
    if (attrs->double_buffered)
        pfd->dwFlags |= PFD_DOUBLEBUFFER;

    pfd->iPixelType = PFD_TYPE_RGBA;

    pfd->cColorBits        = attrs->rgba_size;
    pfd->cRedBits          = attrs->red_size;
    pfd->cGreenBits        = attrs->green_size;
    pfd->cBlueBits         = attrs->blue_size;
    pfd->cAlphaBits        = attrs->alpha_size;

    pfd->cDepthBits        = attrs->depth_size;
    pfd->cStencilBits      = attrs->stencil_size;

    // XXX: Double check these
    pfd->cAccumRedBits     = attrs->accum_buffer;
    pfd->cAccumGreenBits   = attrs->accum_buffer;
    pfd->cAccumBlueBits    = attrs->accum_buffer;
    pfd->cAccumAlphaBits   = attrs->accum_buffer;
    pfd->cAccumBits        = pfd->cAccumRedBits +
                             pfd->cAccumGreenBits +
                             pfd->cAccumBlueBits +
                             pfd->cAccumAlphaBits;

    pfd->iLayerType = PFD_MAIN_PLANE;
}

static bool
wgl_config_choose_native(struct wgl_config *config,
                         struct wgl_display *dpy,
                         const struct wcore_config_attrs *attrs)
{
    if (0 /* dpy->ARB_pixel_format */) {
        // XXX: FINISHME
    }
    else {
        config->pixel_format = ChoosePixelFormat(dpy->hDC, &config->pfd);
        if (!config->pixel_format) {
            wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                         "ChoosePixelFormat failed to find a matching format");
            return false;
        }
    }

    return true;
}


struct wcore_config*
wgl_config_choose(struct wcore_platform *wc_plat,
                  struct wcore_display *wc_dpy,
                  const struct wcore_config_attrs *attrs)
{
    struct wgl_config *self;
    struct wgl_display *dpy = wgl_display(wc_dpy);
    struct wcore_window *wc_window;
    bool ok;

    self = wcore_calloc(sizeof(*self));
    if (!self)
        return NULL;

    ok = wcore_config_init(&self->wcore, wc_dpy, attrs);
    if (!ok)
        goto error;

    wgl_config_set_pixeldescriptor(self, attrs);

    ok = wgl_config_choose_native(self, dpy, attrs);
    if (!ok)
        goto error;

    // Hurray, we've got the pixel format.

    wc_window = wgl_window_priv_create(wc_plat, &self->wcore, 10, 10);
    if (!wc_window)
        goto error;

    self->window = wgl_window(wc_window);

    // Now let's pray that the root window's hDC is compatible with the
    // new window hDC.
    ok = SetPixelFormat(self->window->hDC, self->pixel_format, &self->pfd);
    if (!ok)
        goto error;

    return &self->wcore;

error:
    wgl_config_destroy(&self->wcore);
    return NULL;
}
