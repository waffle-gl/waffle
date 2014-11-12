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

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "wcore_display.h"
#include "wcore_util.h"

struct wcore_platform;

// XXX: Move the typedefs ?
typedef HGLRC (__stdcall *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC,
                                                             HGLRC hShareContext,
                                                             const int *attribList);

typedef BOOL (__stdcall *PFNWGLCHOOSEPIXELFORMATARBPROC )(HDC hdc,
                                                          const int * piAttribIList,
                                                          const float * pfAttribFList,
                                                          unsigned int nMaxFormats,
                                                          int * piFormats,
                                                          unsigned int * nNumFormats);

struct wgl_display {
    struct wcore_display wcore;

    HWND hWnd;
    HDC hDC;
    int pixel_format;
    HGLRC hglrc;

    bool ARB_create_context;
    bool ARB_create_context_profile;
    bool EXT_create_context_es_profile;
    bool EXT_create_context_es2_profile;
    bool ARB_pixel_format;

    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
};

static inline struct wgl_display*
wgl_display(struct wcore_display *wcore)
{
	return (struct wgl_display*)wcore;
}

struct wcore_display*
wgl_display_connect(struct wcore_platform *wc_plat,
                    const char *name);

bool
wgl_display_destroy(struct wcore_display *wc_self);

bool
wgl_display_supports_context_api(struct wcore_display *wc_self,
                                 int32_t context_api);
