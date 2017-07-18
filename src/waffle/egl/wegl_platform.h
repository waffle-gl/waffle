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

#include <EGL/egl.h>

#include "wcore_platform.h"
#include "wcore_util.h"

#include "wegl_imports.h"

struct wegl_platform {
    struct wcore_platform wcore;

    // An EGL_PLATFORM_* enum, such as EGL_PLATFORM_GBM_KHR.
    EGLenum egl_platform;

    /// @brief Value of EGLConfig attribute EGL_SURFACE_TYPE
    ///
    /// When calling eglChooseConfig, Waffle sets the EGL_SURFACE_TYPE attribute
    /// to this value.  Since most Waffle EGL platforms call
    /// eglCreatePlatformWindowSurface() from waffle_window_create(),
    /// wegl_platform_init() initializes this to EGL_WINDOW_BIT.
    ///
    /// This field exists because not all EGL platforms support EGL_WINDOW_BIT;
    /// namely, Mesa's "surfaceless" platform.
    EGLint egl_surface_type_mask;

    // EGL function pointers
    void *eglHandle;

    // See https://www.khronos.org/registry/egl/extensions/EXT/EGL_EXT_client_extensions.txt
    const char *client_extensions;

    EGLBoolean (*eglMakeCurrent)(EGLDisplay dpy, EGLSurface draw,
                                 EGLSurface read, EGLContext ctx);
    __eglMustCastToProperFunctionPointerType
       (*eglGetProcAddress)(const char *procname);

    // EGL 1.5
    EGLDisplay (*eglGetPlatformDisplay)(EGLenum platform, void *native_display,
                                        const EGLAttrib *attrib_list);

    // display
    EGLDisplay (*eglGetDisplay)(EGLNativeDisplayType display_id);
    EGLBoolean (*eglInitialize)(EGLDisplay dpy, EGLint *major, EGLint *minor);
    const char * (*eglQueryString)(EGLDisplay dpy, EGLint name);
    EGLint (*eglGetError)(void);
    EGLBoolean (*eglTerminate)(EGLDisplay dpy);

    // config
    EGLBoolean (*eglChooseConfig)(EGLDisplay dpy, const EGLint *attrib_list,
                                  EGLConfig *configs, EGLint config_size,
                                  EGLint *num_config);

    // context
    EGLBoolean (*eglBindAPI)(EGLenum api);
    EGLContext (*eglCreateContext)(EGLDisplay dpy, EGLConfig config,
                                   EGLContext share_context,
                                   const EGLint *attrib_list);
    EGLBoolean (*eglDestroyContext)(EGLDisplay dpy, EGLContext ctx);

    // window
    EGLBoolean (*eglGetConfigAttrib)(EGLDisplay dpy, EGLConfig config,
                                     EGLint attribute, EGLint *value);
    EGLSurface (*eglCreateWindowSurface)(EGLDisplay dpy, EGLConfig config,
                                         EGLNativeWindowType win,
                                         const EGLint *attrib_list);
    EGLSurface (*eglCreatePbufferSurface)(EGLDisplay dpy, EGLConfig config,
                                          const EGLint *attrib_list);
    EGLBoolean (*eglDestroySurface)(EGLDisplay dpy, EGLSurface surface);
    EGLBoolean (*eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

    // EGL_EXT_platform_display
    EGLDisplay (*eglGetPlatformDisplayEXT)(EGLenum platform, void *native_display,
                                           const EGLint *attrib_list);

    // EGL_EXT_image_dma_buf_import_modifiers
    EGLBoolean (*eglQueryDmaBufFormatsEXT)(EGLDisplay dpy,
                                           EGLint max_formats,
                                           EGLint *formats,
                                           EGLint *num_formats);
    EGLBoolean (*eglQueryDmaBufModifiersEXT)(EGLDisplay dpy, EGLint format,
                                             EGLint max_modifiers,
                                             EGLuint64KHR *modifiers,
                                             EGLBoolean *external_only,
                                             EGLint *num_modifiers);
};

DEFINE_CONTAINER_CAST_FUNC(wegl_platform,
                           struct wegl_platform,
                           struct wcore_platform,
                           wcore)

bool
wegl_platform_teardown(struct wegl_platform *self);

bool
wegl_platform_init(struct wegl_platform *self, EGLenum egl_platform);


// Can eglGetPlatformDisplay can be used for this platform?
//
// True if libEGL exposes the eglGetPlatformDisplay function; and if EGL
// supports the needed platform extension.
bool
wegl_platform_can_use_eglGetPlatformDisplay(const struct wegl_platform *plat);

// Can eglGetPlatformDisplayEXT can be used for this platform?
//
// True if libEGL exposes the eglGetPlatformDisplayEXT function; and if EGL
// supports the needed platform extension.
bool
wegl_platform_can_use_eglGetPlatformDisplayEXT(const struct wegl_platform *plat);
