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

#include <stdbool.h>
#include <stdint.h>

#include "wcore_display.h"
#include "wcore_util.h"

#include "wegl_display.h"

#include "waffle_wayland.h"

struct wcore_platform;
struct wl_display;
struct wl_compositor;
struct wl_shell;
struct xdg_wm_base;

struct wayland_display {
    struct wl_display *wl_display;
    struct wl_registry *wl_registry;
    struct wl_compositor *wl_compositor;
    struct xdg_wm_base *xdg_shell;
    struct wl_shell *wl_shell;

    struct wegl_display wegl;
};

static inline struct wayland_display*
wayland_display(struct wcore_display *wc_self)
{
    if (wc_self) {
        struct wegl_display *wegl_self = container_of(wc_self, struct wegl_display, wcore);
        return container_of(wegl_self, struct wayland_display, wegl);
    }
    else {
        return NULL;
    }
}

struct wcore_display*
wayland_display_connect(struct wcore_platform *wc_plat,
                        const char *name);

bool
wayland_display_destroy(struct wcore_display *wc_self);

void
wayland_display_fill_native(struct wayland_display *self,
                            struct waffle_wayland_display *n_dpy);

union waffle_native_display*
wayland_display_get_native(struct wcore_display *wc_self);

/// @brief Synchronize with server.
///
/// Wayland has flushing requirements that differ from X, largely due to
/// Wayland being agressively asyncrhonous. For example, according to
/// wl_display(3), in event loops "the client should always call
/// wl_display_dispatch_pending() and then wl_display_flush() prior to going
/// back to sleep". Otherwise, all pending requests may not be dispatched to
/// the server.
///
/// Waffle can't expect the client to be aware of Wayland's flushing
/// requirements and to make the needed native Wayland calls before blocking.
/// Instead, the only sensible solution (for now, at least) is to make the
/// public entry points synchronous.
bool
wayland_display_sync(struct wayland_display *dpy);
