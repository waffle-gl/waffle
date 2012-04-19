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

#include "wcore_platform.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <waffle/waffle_enum.h>
#include <waffle/glx/glx_platform.h>
#include <waffle/wayland/wayland_platform.h>
#include <waffle/x11_egl/xegl_platform.h>

#include "wcore_error.h"

static uint64_t id_counter = 0;

struct wcore_platform*
wcore_platform_create(int platform, int gl_api)
{
    struct wcore_platform *self;

    self = calloc(1, sizeof(*self));
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->id = ++id_counter;
    if (self->id == 0) {
        fprintf(stderr, "waffle: error: internal counter wrapped around to 0");
        abort();
    }

    self->gl_api = gl_api;

    switch (platform) {
#ifdef WAFFLE_HAS_GLX
        case WAFFLE_PLATFORM_GLX:
            self->native = glx_platform_create(gl_api, &self->dispatch);
            if (!self->native)
                goto error;
            break;
#endif
#ifdef WAFFLE_HAS_WAYLAND
        case WAFFLE_PLATFORM_WAYLAND:
            self->native = wayland_platform_create(gl_api, &self->dispatch);
            if (!self->native)
                goto error;
            break;
#endif
#ifdef WAFFLE_HAS_X11_EGL
        case WAFFLE_PLATFORM_X11_EGL:
            self->native = xegl_platform_create(gl_api, &self->dispatch);
            if (!self->native)
                goto error;
            break;
#endif
        default:
            wcore_error_internal("bad value for platform (0x%x)", platform);
            goto error;
    }

    self->native_tag = platform;

    return self;

error:
    WCORE_ERROR_DISABLED({
        wcore_platform_destroy(self);
    });
    return NULL;
}

bool
wcore_platform_destroy(struct wcore_platform *self)
{
    bool ok = true;

    if (!self)
        return true;

    switch (self->native_tag) {
#ifdef WAFFLE_HAS_GLX
        case WAFFLE_PLATFORM_GLX:
            ok &= glx_platform_destroy(self->native);
            break;
#endif
#ifdef WAFFLE_HAS_WAYLAND
        case WAFFLE_PLATFORM_WAYLAND:
            ok &= wayland_platform_destroy(self->native);
            break;
#endif
#ifdef WAFFLE_HAS_X11_EGL
        case WAFFLE_PLATFORM_X11_EGL:
            ok &= xegl_platform_destroy(self->native);
            break;
#endif
        default:
            ok = false;
            wcore_error_internal("bad value for wcore_platform._native_tag "
                                 "(0x%x)", self->native_tag);
            break;
    }

    return ok;
}
