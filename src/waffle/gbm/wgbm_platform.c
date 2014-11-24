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

#define _POSIX_C_SOURCE 200112 // glib feature macro for unsetenv()

#include <stdlib.h>
#include <dlfcn.h>

#include "wcore_error.h"

#include "linux_platform.h"

#include "wegl_config.h"
#include "wegl_context.h"
#include "wegl_platform.h"
#include "wegl_util.h"

#include "wgbm_config.h"
#include "wgbm_display.h"
#include "wgbm_platform.h"
#include "wgbm_window.h"

static const char *libgbm_filename = "libgbm.so.1";

static const struct wcore_platform_vtbl wgbm_platform_vtbl;

static bool
wgbm_platform_destroy(struct wcore_platform *wc_self)
{
    struct wgbm_platform *self = wgbm_platform(wegl_platform(wc_self));
    bool ok = true;
    int error = 0;

    if (!self)
        return true;

    unsetenv("EGL_PLATFORM");

    if (self->linux)
        ok &= linux_platform_destroy(self->linux);

    if (self->gbmHandle) {
        error = dlclose(self->gbmHandle);
        if (error) {
            ok &= false;
            wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                         "dlclose(\"%s\") failed: %s",
                         libgbm_filename, dlerror());
        }
    }

    ok &= wegl_platform_teardown(&self->wegl);
    free(self);
    return ok;
}

struct wcore_platform*
wgbm_platform_create(void)
{
    struct wgbm_platform *self;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wegl_platform_init(&self->wegl);
    if (!ok)
        goto error;

    self->gbmHandle = dlopen(libgbm_filename, RTLD_LAZY | RTLD_LOCAL);
    if (!self->gbmHandle) {
        wcore_errorf(WAFFLE_ERROR_FATAL,
                     "dlopen(\"%s\") failed: %s",
                     libgbm_filename, dlerror());
        goto error;
    }

#define RETRIEVE_GBM_SYMBOL(function)                                  \
    self->function = dlsym(self->gbmHandle, #function);                \
    if (!self->function) {                                             \
        wcore_errorf(WAFFLE_ERROR_FATAL,                             \
                     "dlsym(\"%s\", \"" #function "\") failed: %s",    \
                     libgbm_filename, dlerror());                      \
        goto error;                                                    \
    }

    RETRIEVE_GBM_SYMBOL(gbm_create_device);
    RETRIEVE_GBM_SYMBOL(gbm_device_get_fd);
    RETRIEVE_GBM_SYMBOL(gbm_device_destroy);

    RETRIEVE_GBM_SYMBOL(gbm_surface_create);
    RETRIEVE_GBM_SYMBOL(gbm_surface_destroy);

    RETRIEVE_GBM_SYMBOL(gbm_surface_lock_front_buffer);
    RETRIEVE_GBM_SYMBOL(gbm_surface_release_buffer);
#undef RETRIEVE_GBM_SYMBOL

    self->linux = linux_platform_create();
    if (!self->linux)
        goto error;

    setenv("EGL_PLATFORM", "drm", true);

    self->wegl.wcore.vtbl = &wgbm_platform_vtbl;
    return &self->wegl.wcore;

error:
    wgbm_platform_destroy(&self->wegl.wcore);
    return NULL;
}

static bool
wgbm_dl_can_open(struct wcore_platform *wc_self,
                 int32_t waffle_dl)
{
    struct wgbm_platform *self = wgbm_platform(wegl_platform(wc_self));
    return linux_platform_dl_can_open(self->linux, waffle_dl);
}

static void*
wgbm_dl_sym(struct wcore_platform *wc_self,
            int32_t waffle_dl,
            const char *name)
{
    struct wgbm_platform *self = wgbm_platform(wegl_platform(wc_self));
    return linux_platform_dl_sym(self->linux, waffle_dl, name);
}

static union waffle_native_context*
wgbm_context_get_native(struct wcore_context *wc_ctx)
{
    struct wgbm_display *dpy = wgbm_display(wc_ctx->display);
    struct wegl_context *ctx = wegl_context(wc_ctx);
    union waffle_native_context *n_ctx;

    WCORE_CREATE_NATIVE_UNION(n_ctx, gbm);
    if (!n_ctx)
        return NULL;

    wgbm_display_fill_native(dpy, &n_ctx->gbm->display);
    n_ctx->gbm->egl_context = ctx->egl;

    return n_ctx;
}

static const struct wcore_platform_vtbl wgbm_platform_vtbl = {
    .destroy = wgbm_platform_destroy,

    .make_current = wegl_make_current,
    .get_proc_address = wegl_get_proc_address,
    .dl_can_open = wgbm_dl_can_open,
    .dl_sym = wgbm_dl_sym,

    .display = {
        .connect = wgbm_display_connect,
        .destroy = wgbm_display_destroy,
        .supports_context_api = wegl_display_supports_context_api,
        .get_native = wgbm_display_get_native,
    },

    .config = {
        .choose = wgbm_config_choose,
        .destroy = wegl_config_destroy,
        .get_native = wgbm_config_get_native,
    },

    .context = {
        .create = wegl_context_create,
        .destroy = wegl_context_destroy,
        .get_native = wgbm_context_get_native,
    },

    .window = {
        .create = wgbm_window_create,
        .destroy = wgbm_window_destroy,
        .show = wgbm_window_show,
        .swap_buffers = wgbm_window_swap_buffers,
        .get_native = wgbm_window_get_native,
    },
};
