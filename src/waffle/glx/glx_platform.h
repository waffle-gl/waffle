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

#pragma once

#include <GL/glx.h>
#undef linux

#include "wcore_platform.h"
#include "wcore_util.h"

struct linux_platform;

struct glx_platform {
    struct wcore_platform wcore;
    struct linux_platform *linux;

    // glX function pointers
    void *glxHandle;

    GLXContext (*glXCreateNewContext)(Display *dpy, GLXFBConfig config,
                                      int renderType, GLXContext shareList,
                                      Bool direct);
    void (*glXDestroyContext)(Display *dpy, GLXContext ctx);
    Bool (*glXMakeCurrent)(Display *dpy, GLXDrawable drawable, GLXContext ctx);

    const char *(*glXQueryExtensionsString)(Display *dpy, int screen);
    void *(*glXGetProcAddress)(const GLubyte *procname);

    XVisualInfo *(*glXGetVisualFromFBConfig)(Display *dpy, GLXFBConfig config);
    int (*glXGetFBConfigAttrib)(Display *dpy, GLXFBConfig config,
                                int attribute, int *value);
    GLXFBConfig *(*glXChooseFBConfig)(Display *dpy, int screen,
                                      const int *attribList, int *nitems);

    void (*glXSwapBuffers)(Display *dpy, GLXDrawable drawable);


    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB;
};

DEFINE_CONTAINER_CAST_FUNC(glx_platform,
                           struct glx_platform,
                           struct wcore_platform,
                           wcore)

struct wcore_platform*
glx_platform_create(void);
