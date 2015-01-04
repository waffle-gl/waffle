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
#include "c99_compat.h"

struct wcore_config;
struct wcore_config_attrs;
struct wcore_context;
struct wcore_display;
struct wcore_platform;
struct wcore_window;

struct wcore_platform_vtbl {
    bool
    (*destroy)(struct wcore_platform *self);

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

    struct wcore_display_vtbl {
        struct wcore_display*
        (*connect)(struct wcore_platform *platform,
                   const char *name);

        bool
        (*destroy)(struct wcore_display *self);

        bool
        (*supports_context_api)(
                struct wcore_display *display,
                int32_t context_api);

        /// May be null.
        union waffle_native_display*
        (*get_native)(struct wcore_display *display);
    } display;

    struct wcore_config_vtbl {
        struct wcore_config*
        (*choose)(struct wcore_platform *platform,
                  struct wcore_display *display,
                  const struct wcore_config_attrs *attrs);

        bool
        (*destroy)(struct wcore_config *config);

        /// May be null.
        union waffle_native_config*
        (*get_native)(struct wcore_config *config);
    } config;

    struct wcore_context_vtbl {
        struct wcore_context*
        (*create)(struct wcore_platform *platform,
                  struct wcore_config *config,
                  struct wcore_context *share_ctx);

        bool
        (*destroy)(struct wcore_context *ctx);

        /// May be null.
        union waffle_native_context*
        (*get_native)(struct wcore_context *ctx);
    } context;

    struct wcore_window_vtbl {
        struct wcore_window*
        (*create)(struct wcore_platform *platform,
                  struct wcore_config *config,
                  int32_t width,
                  int32_t height);
        bool
        (*destroy)(struct wcore_window *window);

        bool
        (*show)(struct wcore_window *window);

        bool
        (*swap_buffers)(struct wcore_window *window);

        bool
        (*resize)(struct wcore_window *window,
                  int32_t height,
                  int32_t width);

        /// May be null.
        union waffle_native_window*
        (*get_native)(struct wcore_window *window);
    } window;
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
