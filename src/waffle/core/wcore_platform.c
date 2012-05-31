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

/// @addtogroup wcore_platform
/// @{

/// @file

#include "wcore_platform.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <waffle/waffle_enum.h>
#include <waffle/cgl/cgl_platform.h>
#include <waffle/glx/glx_platform.h>
#include <waffle/wayland/wayland_platform.h>
#include <waffle/x11_egl/xegl_platform.h>

#include "wcore_error.h"

static uint64_t id_counter = 0;

struct wcore_platform*
wcore_platform_create(int platform)
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

    switch (platform) {
#ifdef WAFFLE_HAS_CGL
        case WAFFLE_PLATFORM_CGL:
            self->native = cgl_platform_create(&self->dispatch);
            if (!self->native)
                goto error;
            break;
#endif
#ifdef WAFFLE_HAS_GLX
        case WAFFLE_PLATFORM_GLX:
            self->native = glx_platform_create(&self->dispatch);
            if (!self->native)
                goto error;
            break;
#endif
#ifdef WAFFLE_HAS_WAYLAND
        case WAFFLE_PLATFORM_WAYLAND:
            self->native = wayland_platform_create(&self->dispatch);
            if (!self->native)
                goto error;
            break;
#endif
#ifdef WAFFLE_HAS_X11_EGL
        case WAFFLE_PLATFORM_X11_EGL:
            self->native = xegl_platform_create(&self->dispatch);
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
    wcore_platform_destroy(self);
    return NULL;
}

bool
wcore_platform_destroy(struct wcore_platform *self)
{
    bool ok = true;

    if (!self)
        return true;

    switch (self->native_tag) {
#ifdef WAFFLE_HAS_CGL
        case WAFFLE_PLATFORM_CGL:
            ok &= cgl_platform_destroy(self->native);
            break;
#endif
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

/// @}
