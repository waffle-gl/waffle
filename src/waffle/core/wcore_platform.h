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

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

struct wcore_config;
struct wcore_config_attrs;
struct wcore_context;
struct wcore_display;
struct wcore_platform;
struct wcore_window;

struct wcore_platform_vtbl {
    bool
    (*destroy)(struct wcore_platform *self);

    struct wcore_display*
    (*connect_to_display)(
            struct wcore_platform *self,
            const char *name);

    struct wcore_config*
    (*choose_config)(
            struct wcore_platform *self,
            struct wcore_display *dpy,
            const struct wcore_config_attrs *attrs);

    struct wcore_context*
    (*create_context)(
            struct wcore_platform *self,
            struct wcore_config *config,
            struct wcore_context *share_ctx);

    struct wcore_window*
    (*create_window)(
            struct wcore_platform *self,
            struct wcore_config *config,
            int width,
            int height);

    bool
    (*make_current)(
            struct wcore_platform *self,
            struct wcore_display *dpy,
            struct wcore_window *window,
            struct wcore_context *ctx);

    void*
    (*get_proc_address)(
            struct wcore_platform *self,
            const char *proc);

    bool
    (*dl_can_open)(
            struct wcore_platform *self,
            int32_t waffle_dl);

    void*
    (*dl_sym)(
            struct wcore_platform *self,
            int32_t waffle_dl,
            const char *symbol);
};

struct wcore_platform {
    const struct wcore_platform_vtbl *vtbl;
};

static inline bool
wcore_platform_init(struct wcore_platform *self)
{
    assert(self);
    return true;
}

static inline bool
wcore_platform_teardown(struct wcore_platform *self)
{
    assert(self);
    return true;
}
