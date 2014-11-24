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
// DISCLAIMED. IN NO EVENT SHALL TH E COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <stdlib.h>
#include <dlfcn.h>

#include "wcore_error.h"

#include "linux_platform.h"

#include "glx_config.h"
#include "glx_context.h"
#include "glx_display.h"
#include "glx_platform.h"
#include "glx_window.h"
#include "glx_wrappers.h"

static const char *libGL_filename = "libGL.so.1";

static const struct wcore_platform_vtbl glx_platform_vtbl;

static bool
glx_platform_destroy(struct wcore_platform *wc_self)
{
    struct glx_platform *self = glx_platform(wc_self);
    bool ok = true;
    int error = 0;

    if (!self)
        return true;

    if (self->linux)
        ok &= linux_platform_destroy(self->linux);

    if (self->glxHandle) {
        error = dlclose(self->glxHandle);
        if (error) {
            ok &= false;
            wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                         "dlclose(\"%s\") failed: %s",
                         libGL_filename, dlerror());
        }
    }

    ok &= wcore_platform_teardown(wc_self);
    free(self);
    return ok;
}

struct wcore_platform*
glx_platform_create(void)
{
    struct glx_platform *self;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_platform_init(&self->wcore);
    if (!ok)
        goto error;

    self->glxHandle = dlopen(libGL_filename, RTLD_LAZY | RTLD_LOCAL);
    if (!self->glxHandle) {
        wcore_errorf(WAFFLE_ERROR_FATAL,
                     "dlopen(\"%s\") failed: %s",
                     libGL_filename, dlerror());
        goto error;
    }

#define RETRIEVE_GLX_SYMBOL(function)                                  \
    self->function = dlsym(self->glxHandle, #function);                \
    if (!self->function) {                                             \
        wcore_errorf(WAFFLE_ERROR_FATAL,                             \
                     "dlsym(\"%s\", \"" #function "\") failed: %s",    \
                     libGL_filename, dlerror());                      \
        goto error;                                                    \
    }

    RETRIEVE_GLX_SYMBOL(glXCreateNewContext);
    RETRIEVE_GLX_SYMBOL(glXDestroyContext);
    RETRIEVE_GLX_SYMBOL(glXMakeCurrent);

    RETRIEVE_GLX_SYMBOL(glXQueryExtensionsString);
    RETRIEVE_GLX_SYMBOL(glXGetProcAddress);

    RETRIEVE_GLX_SYMBOL(glXGetVisualFromFBConfig);
    RETRIEVE_GLX_SYMBOL(glXGetFBConfigAttrib);
    RETRIEVE_GLX_SYMBOL(glXChooseFBConfig);

    RETRIEVE_GLX_SYMBOL(glXSwapBuffers);
#undef RETRIEVE_GLX_SYMBOL

    self->linux = linux_platform_create();
    if (!self->linux)
        goto error;

    self->glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) self->glXGetProcAddress((const uint8_t*) "glXCreateContextAttribsARB");

    self->wcore.vtbl = &glx_platform_vtbl;
    return &self->wcore;

error:
    glx_platform_destroy(&self->wcore);
    return NULL;
}

static bool
glx_platform_make_current(struct wcore_platform *wc_self,
                          struct wcore_display *wc_dpy,
                          struct wcore_window *wc_window,
                          struct wcore_context *wc_ctx)
{
    struct glx_platform *self = glx_platform(wc_self);
    Display *dpy = glx_display(wc_dpy)->x11.xlib;
    GLXDrawable win = wc_window ? glx_window(wc_window)->x11.xcb : 0;
    GLXContext ctx = wc_ctx ? glx_context(wc_ctx)->glx : NULL;
    bool ok;

    ok = wrapped_glXMakeCurrent(self, dpy, win, ctx);
    if (!ok) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "glXMakeCurrent failed");
    }

    return ok;
}

static void*
glx_platform_get_proc_address(struct wcore_platform *wc_self,
                              const char *name)
{
    struct glx_platform *self = glx_platform(wc_self);
    return self->glXGetProcAddress((const GLubyte*) name);
}

static bool
glx_platform_dl_can_open(struct wcore_platform *wc_self,
                         int32_t waffle_dl)
{
    return linux_platform_dl_can_open(glx_platform(wc_self)->linux,
                                      waffle_dl);
}

static void*
glx_platform_dl_sym(struct wcore_platform *wc_self,
                    int32_t waffle_dl,
                    const char *name)
{
    return linux_platform_dl_sym(glx_platform(wc_self)->linux,
                                              waffle_dl,
                                              name);
}

static const struct wcore_platform_vtbl glx_platform_vtbl = {
    .destroy = glx_platform_destroy,

    .make_current = glx_platform_make_current,
    .get_proc_address = glx_platform_get_proc_address,
    .dl_can_open = glx_platform_dl_can_open,
    .dl_sym = glx_platform_dl_sym,

    .display = {
        .connect = glx_display_connect,
        .destroy = glx_display_destroy,
        .supports_context_api = glx_display_supports_context_api,
        .get_native = glx_display_get_native,
    },

    .config = {
        .choose = glx_config_choose,
        .destroy = glx_config_destroy,
        .get_native = glx_config_get_native,
    },

    .context = {
        .create = glx_context_create,
        .destroy = glx_context_destroy,
        .get_native = glx_context_get_native,
    },

    .window = {
        .create = glx_window_create,
        .destroy = glx_window_destroy,
        .show = glx_window_show,
        .resize = glx_window_resize,
        .swap_buffers = glx_window_swap_buffers,
        .get_native = glx_window_get_native,
    },
};
