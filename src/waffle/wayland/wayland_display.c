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

/// @addtogroup wayland_display
/// @{

/// @file

#include "wayland_display.h"

#include <stdlib.h>
#include <string.h>

#include <waffle/native.h>
#include <waffle/core/wcore_error.h>

#include "wayland_priv_egl.h"
#include "wayland_priv_types.h"

static void
wayland_display_listener(
        struct wl_display *display,
        uint32_t id,
        const char *interface,
        uint32_t version,
        void *data)
{
    union native_display *self = data;

    if (!strncmp(interface, "wl_compositor", 14)) {
        self->wl->wl_compositor = wl_display_bind(display, id,
                                                  &wl_compositor_interface);
    }
    else if (!strncmp(interface, "wl_shell", 9)) {
        self->wl->wl_shell = wl_display_bind(display, id,
                                             &wl_shell_interface);
    }
}

union native_display*
wayland_display_connect(
        union native_platform *platform,
        const char *name)
{
    union native_display *self;
    NATIVE_ALLOC(self, wl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->wl->platform = platform;

    self->wl->wl_display = wl_display_connect(name);
    if (!self->wl->wl_display) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR, "wl_display_connect failed");
        goto error;
    }

    wl_display_add_global_listener(self->wl->wl_display,
                                   wayland_display_listener,
                                   self);

    self->wl->egl_display = wayland_egl_initialize(self->wl->wl_display);
    if (!self->wl->egl_display)
        goto error;

    return self;

error:
    wayland_display_disconnect(self);
    return NULL;
}

bool
wayland_display_disconnect(union native_display *self)
{
    bool ok = true;

    if (!self)
        return true;

    if (self->wl->egl_display)
        ok &= egl_terminate(self->wl->egl_display);

    if (self->wl->wl_display)
        wl_display_disconnect(self->wl->wl_display);

    free(self);
    return ok;
}

bool
wayland_display_supports_context_api(
        union native_display *self,
        int32_t context_api)
{
    union native_platform *platform = self->wl->platform;
    return egl_supports_context_api(platform->wl->linux_, context_api);
}


/// @}
