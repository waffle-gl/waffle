// Copyright 2014 Intel Corporation
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
#include <stdio.h>

#include "nacl_platform.h"

static const struct wcore_platform_vtbl nacl_platform_vtbl;

static bool
nacl_platform_destroy(struct wcore_platform *wc_self)
{
    struct nacl_platform *self = nacl_platform(wc_self);
    bool ok = true;

    if (!self)
        return true;

    ok &= wcore_platform_teardown(wc_self);

    nacl_teardown(self->nacl);

    if (self->gl_dl)
        if (dlclose(self->gl_dl) != 0)
            wcore_errorf(WAFFLE_ERROR_UNKNOWN, "dlclose failed: %s",
                         dlerror());

    free(self);
    return ok;
}

static bool
nacl_platform_dl_can_open(struct wcore_platform *wc_self,
                          int32_t waffle_dl)
{
    struct nacl_platform *self = nacl_platform(wc_self);

    switch (waffle_dl) {
        case WAFFLE_DL_OPENGL_ES2:
            if (!self->gl_dl)
                self->gl_dl = dlopen(NACL_GLES2_LIBRARY, RTLD_LAZY);
            break;
        // API not supported
        default:
            return false;
    }

    if (!self->gl_dl)
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "dlopen failed: %s", dlerror());

    return self->gl_dl ? true : false;
}

// Construct a string that maps GL function to NaCl function
// by concating given prefix and function name tail from 'src'.
static char *
nacl_prefix(const char *src, const char *prefix)
{
    if (strncmp(src, "gl", 2) != 0)
        return NULL;

    uint32_t len = strlen(src) + strlen(prefix);

    char *dst = wcore_calloc(len);
    if (!dst)
        return NULL;

    snprintf(dst, len, "%s%s", prefix, src + 2);

    return dst;
}

static void*
nacl_platform_dl_sym(struct wcore_platform *wc_self,
                     int32_t waffle_dl,
                     const char *name)
{
    struct nacl_platform *self = nacl_platform(wc_self);
    char *nacl_name = NULL;
    void *func = NULL;

    if (!self->gl_dl)
        if (!nacl_platform_dl_can_open(wc_self, waffle_dl))
            return false;

    nacl_name = nacl_prefix(name, "GLES2");

    if (!nacl_name)
        return NULL;

    func = dlsym(self->gl_dl, nacl_name);

    if (!func) {
        const char *error = dlerror();
        if (error) {
            wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                         "dlsym(libname=\"%s\", symbol=\"%s\") failed: %s",
                         NACL_GLES2_LIBRARY, nacl_name, error);
        }
    }

    free(nacl_name);

    return func;
}

static bool
nacl_platform_make_current(struct wcore_platform *wc_self,
                           struct wcore_display *wc_dpy,
                           struct wcore_window *wc_window,
                           struct wcore_context *wc_ctx)
{
    bool release = (!wc_window && !wc_ctx);
    return nacl_makecurrent(nacl_platform(wc_self)->nacl,
                            release);
}

struct wcore_platform*
nacl_platform_create(void)
{
    struct nacl_platform *self;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_platform_init(&self->wcore);
    if (!ok)
        goto error;

    self->nacl = nacl_init();
    if (!self->nacl)
        goto error;

    self->wcore.vtbl = &nacl_platform_vtbl;
    return &self->wcore;

error:
    nacl_platform_destroy(&self->wcore);
    return NULL;
}

static const struct wcore_platform_vtbl nacl_platform_vtbl = {
    .destroy = nacl_platform_destroy,

    .make_current = nacl_platform_make_current,
    .dl_can_open = nacl_platform_dl_can_open,
    .dl_sym = nacl_platform_dl_sym,

    .display = {
        .connect = nacl_display_connect,
        .destroy = nacl_display_destroy,
        .supports_context_api = nacl_display_supports_context_api,
        .get_native = NULL,
    },

    .config = {
        .choose = nacl_config_choose,
        .destroy = nacl_config_destroy,
        .get_native = NULL,
    },

    .context = {
        .create = nacl_context_create,
        .destroy = nacl_context_destroy,
        .get_native = NULL,
    },

    .window = {
        .create = nacl_window_create,
        .destroy = nacl_window_destroy,
        .show = nacl_window_show,
        .swap_buffers = nacl_window_swap_buffers,
        .resize = nacl_window_resize,
        .get_native = NULL,
    },
};
