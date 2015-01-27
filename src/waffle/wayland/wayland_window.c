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

#define WL_EGL_PLATFORM 1

#include <stdlib.h>
#include <string.h>

#include <wayland-egl.h>
#undef container_of

#include "waffle_wayland.h"

#include "wcore_attrib_list.h"
#include "wcore_error.h"

#include "wegl_config.h"

#include "wayland_display.h"
#include "wayland_window.h"

bool
wayland_window_destroy(struct wcore_window *wc_self)
{
    struct wayland_window *self = wayland_window(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    ok &= wegl_window_teardown(&self->wegl);

    if (self->wl_window)
        wl_egl_window_destroy(self->wl_window);

    if (self->wl_shell_surface)
        wl_shell_surface_destroy(self->wl_shell_surface);

    if (self->wl_surface)
        wl_surface_destroy(self->wl_surface);

    free(self);
    return ok;
}

static void
shell_surface_listener_ping(void *data,
                            struct wl_shell_surface *shell_surface,
                            uint32_t serial)
{
    wl_shell_surface_pong(shell_surface, serial);
}

static void
shell_surface_listener_configure(void *data,
                                 struct wl_shell_surface *shell_surface,
                                 uint32_t edges,
                                 int32_t width,
                                 int32_t height)
{
}

static void
shell_surface_listener_popup_done(void *data,
                                  struct wl_shell_surface *shell_surface)
{
}

static const struct wl_shell_surface_listener shell_surface_listener = {
    .ping = shell_surface_listener_ping,
    .configure = shell_surface_listener_configure,
    .popup_done = shell_surface_listener_popup_done
};

struct wcore_window*
wayland_window_create(struct wcore_platform *wc_plat,
                      struct wcore_config *wc_config,
                      int32_t width,
                      int32_t height,
                      const intptr_t attrib_list[])
{
    struct wayland_window *self;
    struct wayland_display *dpy = wayland_display(wc_config->display);
    bool ok = true;

    if (wcore_attrib_list_length(attrib_list) > 0) {
        wcore_error_bad_attribute(attrib_list[0]);
        return NULL;
    }

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    if (!dpy->wl_compositor) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "wayland compositor not found");
        goto error;
    }
    if (!dpy->wl_shell) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "wayland shell not found");
        goto error;
    }

    self->wl_surface = wl_compositor_create_surface(dpy->wl_compositor);
    if (!self->wl_surface) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "wl_compositor_create_surface failed");
        goto error;
    }

    self->wl_shell_surface = wl_shell_get_shell_surface(dpy->wl_shell,
                                                        self->wl_surface);
    if (!self->wl_shell_surface) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "wl_shell_get_shell_surface failed");
        goto error;
    }

    wl_shell_surface_add_listener(self->wl_shell_surface,
                                  &shell_surface_listener,
                                  NULL);

    self->wl_window = wl_egl_window_create(self->wl_surface, width, height);
    if (!self->wl_window) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "wl_egl_window_create failed");
        goto error;
    }

    ok = wegl_window_init(&self->wegl, wc_config, (intptr_t) self->wl_window);
    if (!ok)
        goto error;

    ok = wayland_display_sync(dpy);
    if (!ok)
       goto error;

    return &self->wegl.wcore;

error:
    wayland_window_destroy(&self->wegl.wcore);
    return NULL;
}


bool
wayland_window_show(struct wcore_window *wc_self)
{
    struct wayland_window *self = wayland_window(wc_self);
    struct wayland_display *dpy = wayland_display(wc_self->display);
    bool ok = true;

    wl_shell_surface_set_toplevel(self->wl_shell_surface);

    ok = wayland_display_sync(dpy);
    if (!ok)
       return false;

    // FIXME: How to detect errors in Wayland?
    return true;
}

bool
wayland_window_swap_buffers(struct wcore_window *wc_self)
{
   struct wayland_display *dpy = wayland_display(wc_self->display);
   bool ok;

   ok = wegl_window_swap_buffers(wc_self);
   if (!ok)
      return false;

   ok = wayland_display_sync(dpy);
   if (!ok)
      return false;

   return true;
}

bool
wayland_window_resize(struct wcore_window *wc_self,
                      int32_t width, int32_t height)
{
   struct wayland_window *self = wayland_window(wc_self);
   struct wayland_display *dpy = wayland_display(self->wegl.wcore.display);

   wl_egl_window_resize(wayland_window(wc_self)->wl_window,
                        width, height, 0, 0);

   if (!wayland_display_sync(dpy))
      return false;

   // FIXME: How to detect if the resize failed?
   return true;
}

union waffle_native_window*
wayland_window_get_native(struct wcore_window *wc_self)
{
    struct wayland_window *self = wayland_window(wc_self);
    struct wayland_display *dpy = wayland_display(wc_self->display);
    union waffle_native_window *n_window;

    WCORE_CREATE_NATIVE_UNION(n_window, wayland);
    if (!n_window)
        return NULL;

    wayland_display_fill_native(dpy, &n_window->wayland->display);
    n_window->wayland->wl_surface = self->wl_surface;
    n_window->wayland->wl_shell_surface = self->wl_shell_surface;
    n_window->wayland->wl_window = self->wl_window;
    n_window->wayland->egl_surface = self->wegl.egl;

    return n_window;
}
