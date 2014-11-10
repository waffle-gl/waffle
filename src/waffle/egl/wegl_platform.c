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

#include <dlfcn.h>

#include "wcore_error.h"
#include "wegl_platform.h"


#ifdef WAFFLE_HAS_ANDROID
static const char *libEGL_filename = "libEGL.so";
#else
static const char *libEGL_filename = "libEGL.so.1";
#endif

bool
wegl_platform_teardown(struct wegl_platform *self)
{
    bool ok = true;
    int error = 0;

    if (self->eglHandle) {
        error = dlclose(self->eglHandle);
        if (error) {
            ok = false;
            wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                         "dlclose(\"%s\") failed: %s",
                         libEGL_filename, dlerror());
        }
    }

    ok &= wcore_platform_teardown(&self->wcore);
    return ok;
}
bool
wegl_platform_init(struct wegl_platform *self)
{
    bool ok;

    ok = wcore_platform_init(&self->wcore);
    if (!ok)
        goto error;

    self->eglHandle = dlopen(libEGL_filename, RTLD_LAZY | RTLD_LOCAL);
    if (!self->eglHandle) {
        wcore_errorf(WAFFLE_ERROR_FATAL,
                     "dlopen(\"%s\") failed: %s",
                     libEGL_filename, dlerror());
        goto error;
    }

#define RETRIEVE_EGL_SYMBOL(function)                                  \
    self->function = dlsym(self->eglHandle, #function);                \
    if (!self->function) {                                             \
        wcore_errorf(WAFFLE_ERROR_FATAL,                             \
                     "dlsym(\"%s\", \"" #function "\") failed: %s",    \
                     libEGL_filename, dlerror());                      \
        goto error;                                                    \
    }

    RETRIEVE_EGL_SYMBOL(eglMakeCurrent);
    RETRIEVE_EGL_SYMBOL(eglGetProcAddress);

    // display
    RETRIEVE_EGL_SYMBOL(eglGetDisplay);
    RETRIEVE_EGL_SYMBOL(eglInitialize);
    RETRIEVE_EGL_SYMBOL(eglQueryString);
    RETRIEVE_EGL_SYMBOL(eglGetError);
    RETRIEVE_EGL_SYMBOL(eglTerminate);

    // config
    RETRIEVE_EGL_SYMBOL(eglChooseConfig);

    // context
    RETRIEVE_EGL_SYMBOL(eglBindAPI);
    RETRIEVE_EGL_SYMBOL(eglCreateContext);
    RETRIEVE_EGL_SYMBOL(eglDestroyContext);

    // window
    RETRIEVE_EGL_SYMBOL(eglGetConfigAttrib);
    RETRIEVE_EGL_SYMBOL(eglCreateWindowSurface);
    RETRIEVE_EGL_SYMBOL(eglDestroySurface);
    RETRIEVE_EGL_SYMBOL(eglSwapBuffers);

#undef RETRIEVE_EGL_SYMBOL

error:
    // On failure the caller of wegl_platform_init will trigger it's own
    // destruction which will execute wegl_platform_teardown.
    return ok;
}
