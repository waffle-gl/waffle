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
#include <strings.h>
#include <windows.h>

#include "wcore_error.h"

#include "wgl_display.h"
#include "wgl_dl.h"
#include "wgl_platform.h"

bool
wgl_display_destroy(struct wcore_display *wc_self)
{
    struct wgl_display *self = wgl_display(wc_self);
    bool ok = true;

    if (!self)
        return true;

    if (self->hWnd) {
        if (self->hglrc) {
            ok &= wglDeleteContext(self->hglrc);
        }

        if (self->hDC)
            ok &= ReleaseDC(self->hWnd, self->hDC);

        ok &= DestroyWindow(self->hWnd);
    }

    ok &= wcore_display_teardown(wc_self);
    free(self);
    return ok;
}

static bool
wgl_display_create_window(struct wgl_platform *plat, struct wgl_display *dpy)
{
    dpy->hWnd = CreateWindow(plat->class_name, NULL,
                             WS_POPUPWINDOW|WS_DISABLED,
                             0, 0, 0, 0, NULL, NULL, NULL, NULL);
    if (!dpy->hWnd)
        return false;

    dpy->hDC = GetDC(dpy->hWnd);
    if (!dpy->hDC)
        return false;

    return true;
}

static bool
wgl_display_choose_config(struct wgl_display *dpy)
{
    // XXX: Is there a move common/appropriate pixelformat ?
    PIXELFORMATDESCRIPTOR pfd = {0};
    bool ok;

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;

    pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;

    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.iLayerType = PFD_MAIN_PLANE;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 16;

    dpy->pixel_format = ChoosePixelFormat(dpy->hDC, &pfd);
    if (!dpy->pixel_format)
        return false;

    ok = SetPixelFormat(dpy->hDC, dpy->pixel_format, &pfd);
    if (!ok)
        return false;

    return true;
}

static bool
wgl_display_hardware_render(struct wgl_display *dpy)
{
#ifndef GL_RENDERER
#define GL_RENDERER 0x1F01
#endif
    typedef unsigned int GLenum;
    typedef unsigned char GLubyte;
    typedef const GLubyte * (__stdcall *PFNGLGETSTRINGPROC)(GLenum name);

    PFNGLGETSTRINGPROC glGetString_func;
    const GLubyte *gl_renderer;
    bool ok;

    glGetString_func = wgl_dl_sym(dpy->wcore.platform, WAFFLE_DL_OPENGL, "glGetString");
    if (!glGetString_func)
        return false;

    ok = wglMakeCurrent(dpy->hDC, dpy->hglrc);
    if (!ok)
        return false;

    gl_renderer = glGetString_func(GL_RENDERER);
    ok = wglMakeCurrent(NULL, NULL);
    if (!ok)
        return false;

    // Bail out if we cannot retrieve the renderer string or if we're using GDI
    if (!gl_renderer || strcasecmp((const char *)gl_renderer, "GDI Generic") == 0)
        return false;

    return true;
}

struct wcore_display*
wgl_display_connect(struct wcore_platform *wc_plat,
                    const char *name)
{
    struct wgl_display *self;
    bool ok;

    self = wcore_calloc(sizeof(*self));
    if (!self)
        return NULL;

    ok = wcore_display_init(&self->wcore, wc_plat);
    if (!ok)
        goto error;

    ok = wgl_display_create_window(wgl_platform(wc_plat), self);
    if (!ok)
        goto error;

    ok = wgl_display_choose_config(self);
    if (!ok)
        goto error;

    self->hglrc = wglCreateContext(self->hDC);
    if (!self->hglrc)
        goto error;

    ok = wgl_display_hardware_render(self);
    if (!ok)
        goto error;

    return &self->wcore;

error:
    wgl_display_destroy(&self->wcore);
    return NULL;
}

bool
wgl_display_supports_context_api(struct wcore_display *wc_self,
                                 int32_t context_api)
{
    switch (context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            return true;
        case WAFFLE_CONTEXT_OPENGL_ES1:
            return false;
        case WAFFLE_CONTEXT_OPENGL_ES2:
            return false;
        case WAFFLE_CONTEXT_OPENGL_ES3:
            return false;
        default:
            wcore_error_internal("waffle_context_api has bad value %#x",
                                 context_api);
            return false;
    }
}
