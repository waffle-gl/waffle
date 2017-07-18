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

#define _POSIX_C_SOURCE 200112 // glib feature macro for unsetenv()

#include <dlfcn.h>

#include "wcore_error.h"
#include "wegl_platform.h"


#ifdef WAFFLE_HAS_ANDROID
static const char *libEGL_filename = "libEGL.so";
#else
static const char *libEGL_filename = "libEGL.so.1";
#endif

static void
setup_env(const struct wegl_platform *self)
{
    switch (self->egl_platform) {
        case EGL_PLATFORM_ANDROID_KHR:
            // Don't set EGL_PLATFORM because I don't know the impact doing so
            // on Android. Does anything other than Mesa use it?
            break;
        case EGL_PLATFORM_GBM_KHR:
            setenv("EGL_PLATFORM", "drm", true);
            break;
        case EGL_PLATFORM_WAYLAND_KHR:
            setenv("EGL_PLATFORM", "wayland", true);
            break;
        case EGL_PLATFORM_X11_KHR:
            setenv("EGL_PLATFORM", "x11", true);
            break;
        case EGL_PLATFORM_SURFACELESS_MESA:
            setenv("EGL_PLATFORM", "surfaceless", true);
            break;
        default:
            assert(!"bad egl_platform enum");
            break;
    }
}

bool
wegl_platform_teardown(struct wegl_platform *self)
{
    bool ok = true;
    int error = 0;

    if (!wegl_platform_can_use_eglGetPlatformDisplay(self) &&
        !wegl_platform_can_use_eglGetPlatformDisplayEXT(self) &&
        self->egl_platform != EGL_PLATFORM_ANDROID_KHR) {
        unsetenv("EGL_PLATFORM");
    }

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
wegl_platform_init(struct wegl_platform *self, EGLenum egl_platform)
{
    bool ok;

    ok = wcore_platform_init(&self->wcore);
    if (!ok)
        goto error;

    self->egl_platform = egl_platform;

    // Most Waffle platforms will call eglCreateWindowSurface.
    self->egl_surface_type_mask = EGL_WINDOW_BIT;

    self->eglHandle = dlopen(libEGL_filename, RTLD_LAZY | RTLD_LOCAL);
    if (!self->eglHandle) {
        wcore_errorf(WAFFLE_ERROR_FATAL,
                     "dlopen(\"%s\") failed: %s",
                     libEGL_filename, dlerror());
        ok = false;
        goto error;
    }

#define RETRIEVE_EGL_SYMBOL(function)                                  \
    self->function = dlsym(self->eglHandle, #function);                \
    if (!self->function) {                                             \
        wcore_errorf(WAFFLE_ERROR_FATAL,                             \
                     "dlsym(\"%s\", \"" #function "\") failed: %s",    \
                     libEGL_filename, dlerror());                      \
        ok = false;                                                    \
        goto error;                                                    \
    }

    // Use eglGetProcAddress to get EGL 1.5 symbols, not dlsym, because the
    // EGL 1.5 spec requires that implementors support eglGetProcAddress for
    // all symbols.
    //
    // From the EGL 1.5 spec:
    //
    //   eglGetProcAddress may be queried for all EGL and client API functions
    //   supported by the implementation (whether those functions are
    //   extensions or not, and whether they are supported by the current
    //   client API context or not).
    //
    //   For functions that are queryable with eglGetProcAddress,
    //   implementations may choose to also export those functions statically
    //   from the object libraries implementing those functions. However,
    //   portable clients cannot rely on this behavior.
    //
#define RETRIEVE_EGL_SYMBOL_OPTIONAL(function) \
    self->function = (void*) self->eglGetProcAddress(#function);

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
    RETRIEVE_EGL_SYMBOL(eglCreatePbufferSurface);
    RETRIEVE_EGL_SYMBOL(eglDestroySurface);
    RETRIEVE_EGL_SYMBOL(eglSwapBuffers);

    // EGL 1.5
    RETRIEVE_EGL_SYMBOL_OPTIONAL(eglGetPlatformDisplay);

    // EGL_EXT_platform_display
    RETRIEVE_EGL_SYMBOL_OPTIONAL(eglGetPlatformDisplayEXT);

    // EGL_EXT_image_dma_buf_import_modifiers
    RETRIEVE_EGL_SYMBOL_OPTIONAL(eglQueryDmaBufFormatsEXT);
    RETRIEVE_EGL_SYMBOL_OPTIONAL(eglQueryDmaBufModifiersEXT);

#undef RETRIEVE_EGL_SYMBOL
#undef RETRIEVE_EGL_SYMBOL_OPTIONAL

    self->client_extensions =
        self->eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);

    if (!wegl_platform_can_use_eglGetPlatformDisplay(self) &&
        !wegl_platform_can_use_eglGetPlatformDisplayEXT(self)) {
        setup_env(self);
    }

error:
    // On failure the caller of wegl_platform_init will trigger it's own
    // destruction which will execute wegl_platform_teardown.
    return ok;
}

bool
wegl_platform_can_use_eglGetPlatformDisplay(const struct wegl_platform *plat)
{
    const char *ext;

    if (!plat->eglGetPlatformDisplay)
        return false;

    switch (plat->egl_platform) {
        case EGL_PLATFORM_ANDROID_KHR:
            ext = "EGL_KHR_platform_android";
            break;
        case EGL_PLATFORM_GBM_KHR:
            ext = "EGL_KHR_platform_gbm";
            break;
        case EGL_PLATFORM_WAYLAND_KHR:
            ext = "EGL_KHR_platform_wayland";
            break;
        case EGL_PLATFORM_X11_KHR:
            ext = "EGL_KHR_platform_x11";
            break;
        case EGL_PLATFORM_SURFACELESS_MESA:
            ext = "EGL_MESA_platform_surfaceless";
            break;
        default:
            assert(!"bad egl_platform enum");
            return false;
    }

    return waffle_is_extension_in_string(plat->client_extensions, ext);
}

bool
wegl_platform_can_use_eglGetPlatformDisplayEXT(const struct wegl_platform *plat)
{
    const char *ext;

    if (!plat->eglGetPlatformDisplayEXT)
        return false;

    switch (plat->egl_platform) {
        case EGL_PLATFORM_ANDROID_KHR:
            // There exist no Android extension for eglGetPlatformDisplayEXT.
            return false;
        case EGL_PLATFORM_GBM_KHR:
            ext = "EGL_MESA_platform_gbm";
            break;
        case EGL_PLATFORM_WAYLAND_KHR:
            ext = "EGL_EXT_platform_wayland";
            break;
        case EGL_PLATFORM_X11_KHR:
            ext = "EGL_EXT_platform_x11";
            break;
        case EGL_PLATFORM_SURFACELESS_MESA:
            ext = "EGL_MESA_platform_surfaceless";
            break;
        default:
            assert(!"bad egl_platform enum");
            return false;
    }

    return waffle_is_extension_in_string(plat->client_extensions, ext);
}
