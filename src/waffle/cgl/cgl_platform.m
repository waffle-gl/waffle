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

#include <stdlib.h>

#include "wcore_error.h"

#include "cgl_config.h"
#include "cgl_context.h"
#include "cgl_display.h"
#include "cgl_dl.h"
#include "cgl_platform.h"
#include "cgl_window.h"

static const struct wcore_platform_vtbl cgl_platform_vtbl;

static bool
cgl_platform_destroy(struct wcore_platform *wc_self)
{
    struct cgl_platform *self = cgl_platform(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    if (self->dl_gl)
        ok &= cgl_dl_close(&self->wcore);

    ok &= wcore_platform_teardown(wc_self);
    free(self);
    return ok;
}

static bool
cgl_platform_set_system_version(struct cgl_platform *self)
{
    NSString *plistPath = @"/System/Library/CoreServices/SystemVersion.plist";
    NSDictionary *dict = [NSDictionary dictionaryWithContentsOfFile:plistPath];
    if (!dict) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
            "failed to open 'SystemVersion.plist'; "
            "failed to get system version");
        return false;
    }

    NSString *str = [dict objectForKey:@"ProductVersion"];
    if (!str) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
            "failed to get key 'ProductVersion' from 'SystemVersion.plist'; "
            "failed to get system version");
        return false;
    }

    NSArray *array = [str componentsSeparatedByString:@"."];
    int len = [array count];
    if (len < 2) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
            "SystemVersion.plit:ProductVersion=\"%s\" has too few comonents; "
            "failed to get system version",
            [str UTF8String]);
        return false;
    }

    self->system_version_major = [(NSString*)[array objectAtIndex:0] integerValue];
    self->system_version_minor = [(NSString*)[array objectAtIndex:1] integerValue];
    self->system_version_full = (self->system_version_major << 8)
                              | self->system_version_minor;
    return true;
}

struct wcore_platform*
cgl_platform_create(void)
{
    struct cgl_platform *self;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (!self)
        return NULL;

    ok = wcore_platform_init(&self->wcore);
    if (!ok)
        goto error;

    ok = cgl_platform_set_system_version(self);
    if (!ok)
        goto error;

    if (self->system_version_full <= 0x104) {
        wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                     "Mac OS < 10.4 is unsupported");
        goto error;
    }

    self->wcore.vtbl = &cgl_platform_vtbl;
    return &self->wcore;

error:
    cgl_platform_destroy(&self->wcore);
    return NULL;
}

static bool
cgl_make_current(struct wcore_platform *wc_self,
                          struct wcore_display *wc_dpy,
                          struct wcore_window *wc_window,
                          struct wcore_context *wc_ctx)
{
    struct cgl_window *window = cgl_window(wc_window);
    struct cgl_context *ctx = cgl_context(wc_ctx);

    NSOpenGLContext *cocoa_ctx = ctx ? ctx->ns : NULL;
    WaffleGLView* cocoa_view = window ? window->gl_view : NULL;

    if (cocoa_view)
        cocoa_view.glContext = cocoa_ctx;

    if (cocoa_ctx) {
        [cocoa_ctx makeCurrentContext];
        [cocoa_ctx setView:cocoa_view];
    }

    return true;
}

static void*
cgl_get_proc_address(struct wcore_platform *wc_self, const char *name)
{
    // There is no CGLGetProcAddress. However, Waffle follows the principle of
    // least surprise here. The only supported API on CGL is OpenGL, so assume
    // the user called waffle_get_proc_address() to obtain an OpenGL function
    // pointer.
    return cgl_dl_sym(wc_self, WAFFLE_DL_OPENGL, name);
}

static const struct wcore_platform_vtbl cgl_platform_vtbl = {
    .destroy = cgl_platform_destroy,

    .make_current = cgl_make_current,
    .get_proc_address = cgl_get_proc_address,
    .dl_can_open = cgl_dl_can_open,
    .dl_sym = cgl_dl_sym,

    .display = {
        .connect = cgl_display_connect,
        .destroy = cgl_display_destroy,
        .supports_context_api = cgl_display_supports_context_api,
        .get_native = NULL,
    },

    .config = {
        .choose = cgl_config_choose,
        .destroy = cgl_config_destroy,
        .get_native = NULL,
    },

    .context = {
        .create = cgl_context_create,
        .destroy = cgl_context_destroy,
        .get_native = NULL,
    },

    .window = {
        .create = cgl_window_create,
        .destroy = cgl_window_destroy,
        .show = cgl_window_show,
        .resize = cgl_window_resize,
        .swap_buffers = cgl_window_swap_buffers,
        .get_native = NULL,
    },
};
