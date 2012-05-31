// Copyright 2012 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// @defgroup native native
/// @{

/// @file

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct wcore_config_attrs;

#define NATIVE_UNION(name) \
    struct cgl_##name; \
    struct droid_##name; \
    struct glx_##name; \
    struct wayland_##name; \
    struct xegl_##name; \
    \
    union native_##name { \
        struct cgl_##name *cgl; \
        struct droid_##name *droid; \
        struct glx_##name *glx; \
        struct wayland_##name *wl; \
        struct xegl_##name *xegl; \
    }

NATIVE_UNION(platform);
NATIVE_UNION(config);
NATIVE_UNION(context);
NATIVE_UNION(display);
NATIVE_UNION(window);

/// @brief Allocate a `union native_?`.
///
/// @param x must have type `union native_?`.
/// @param platform is one of droid, glx, mac, wl, xegl.
#define NATIVE_ALLOC(x, platform) \
    do { \
        x = calloc(1, sizeof(*x) + sizeof(*x->platform)); \
        x->platform = (void*) (x + 1); \
    } while (0)


struct native_dispatch {
    union native_display*
    (*display_connect)(
            union native_platform *platform,
            const char *name);

    bool
    (*display_disconnect)(union native_display *self);

    bool
    (*display_supports_context_api)(
            union native_display *self,
            int32_t context_api);

    union native_config*
    (*config_choose)(
            union native_display *dpy,
            const struct wcore_config_attrs *attrs);

    bool
    (*config_destroy)(union native_config *self);

    union native_context*
    (*context_create)(
            union native_config *config,
            union native_context *shared_context);

    bool
    (*context_destroy)(union native_context *self);

    bool
    (*dl_can_open)(
            union native_platform *platform,
            int32_t waffle_dl);

    void*
    (*dl_sym)(
            union native_platform *platform,
            int32_t waffle_dl,
            const char *name);

    union native_window*
    (*window_create)(
            union native_config *config,
            int width,
            int height);

    bool
    (*window_destroy)(union native_window *self);

    bool
    (*window_show)(union native_window *self);

    bool
    (*window_swap_buffers)(union native_window *self);

    bool
    (*make_current)(
            union native_display *dpy,
            union native_window *window,
            union native_context *ctx);

    void*
    (*get_proc_address)(
            union native_platform *platform,
            const char *name);
};

/// @}
