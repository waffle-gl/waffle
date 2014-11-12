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

/// @brief Check the values of `attrs->context_*`.
static bool
wgl_config_check_context_attrs(struct wgl_display *dpy,
                               const struct wcore_config_attrs *attrs)
{
    if (attrs->context_forward_compatible) {
        assert(attrs->context_api == WAFFLE_CONTEXT_OPENGL);
        assert(wcore_config_attrs_version_ge(attrs, 30));
    }

    if (attrs->context_debug && !dpy->ARB_create_context) {
        wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                     "WGL_ARB_create_context is required in order to "
                     "request a debug context");
        return false;
    }

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            if (!wcore_config_attrs_version_eq(attrs, 10) && !dpy->ARB_create_context) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "WGL_ARB_create_context is required in order to "
                             "request an OpenGL version not equal to the default "
                             "value 1.0");
                return false;
            }
            else if (wcore_config_attrs_version_ge(attrs, 32) && !dpy->ARB_create_context_profile) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "WGL_ARB_create_context_profile is required "
                             "to create a context with version >= 3.2");
                return false;
            }
            else if (wcore_config_attrs_version_ge(attrs, 32)) {
                assert(attrs->context_profile == WAFFLE_CONTEXT_CORE_PROFILE ||
                       attrs->context_profile == WAFFLE_CONTEXT_COMPATIBILITY_PROFILE);
            }

            if (attrs->context_forward_compatible && !dpy->ARB_create_context) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "WGL_ARB_create_context is required in order to "
                             "request a forward-compatible context");
                return false;
            }

            return true;

        case WAFFLE_CONTEXT_OPENGL_ES1:
            assert(wcore_config_attrs_version_eq(attrs, 10) ||
                   wcore_config_attrs_version_eq(attrs, 11));
            assert(!attrs->context_forward_compatible);

            if (!dpy->EXT_create_context_es_profile) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "WGL_EXT_create_context_es_profile is required "
                             "to create an OpenGL ES1 context");
                return false;
            }

            return true;

        case WAFFLE_CONTEXT_OPENGL_ES2:
            assert(attrs->context_major_version == 2);
            assert(!attrs->context_forward_compatible);

            if (!dpy->EXT_create_context_es2_profile) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "WGL_EXT_create_context_es2_profile is required "
                             "to create an OpenGL ES2 context");
                return false;
            }

            return true;

        case WAFFLE_CONTEXT_OPENGL_ES3:
            assert(attrs->context_major_version == 3);

            if (!dpy->EXT_create_context_es_profile) {
                wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                             "WGL_EXT_create_context_es_profile is required "
                             "to create an OpenGL ES3 context");
                return false;
            }

            return true;

        default:
            wcore_error_internal("context_api has bad value %#x",
                                 attrs->context_api);
            return false;
    }
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

    // Use wglChoosePixelFormatARB if available.
    if (dpy->ARB_pixel_format) {
        float fAttribs[1] = { 0 };
        int iAttribs[] = {
            WGL_COLOR_BITS_ARB,         attrs->rgba_size,
            WGL_RED_BITS_ARB,           attrs->red_size,
            WGL_GREEN_BITS_ARB,         attrs->green_size,
            WGL_BLUE_BITS_ARB,          attrs->blue_size,
            WGL_ALPHA_BITS_ARB,         attrs->alpha_size,

            WGL_DEPTH_BITS_ARB,         attrs->depth_size,
            WGL_STENCIL_BITS_ARB,       attrs->stencil_size,

            WGL_SAMPLE_BUFFERS_ARB,     attrs->sample_buffers,
            WGL_STEREO_ARB,             attrs->samples,

            WGL_DOUBLE_BUFFER_ARB,      attrs->double_buffered,

            WGL_ACCUM_RED_BITS_ARB,     attrs->accum_buffer,
            WGL_ACCUM_GREEN_BITS_ARB,   attrs->accum_buffer,
            WGL_ACCUM_BLUE_BITS_ARB,    attrs->accum_buffer,
            WGL_ACCUM_ALPHA_BITS_ARB,   attrs->accum_buffer,

            WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
            WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,

            0,
        };
        unsigned int num_formats;
        bool ok;

        // But first we need a current context to use it...
        ok = wglMakeCurrent(dpy->hDC, dpy->hglrc);
        if (!ok)
            return false;

        ok = dpy->wglChoosePixelFormatARB(dpy->hDC, iAttribs, fAttribs, 1,
                                          &config->pixel_format, &num_formats);

        wglMakeCurrent(NULL, NULL);

        if (!ok || !num_formats) {
            wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                         "wglChoosePixelFormatARB failed");
            return false;
        }

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

    ok = wgl_config_check_context_attrs(dpy, attrs);
    if (!ok)
        return NULL;

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
