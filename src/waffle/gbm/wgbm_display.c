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

#define __GBM__ 1
#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>

#include <gbm.h>
#include <libudev.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>

#include "wcore_error.h"

#include "wgbm_display.h"
#include "wgbm_platform.h"

bool
wgbm_display_destroy(struct wcore_display *wc_self)
{
    struct wgbm_display *self = wgbm_display(wc_self);
    bool ok = true;
    int fd;

    if (!self)
        return ok;


    ok &= wegl_display_teardown(&self->wegl);

    if (self->gbm_device) {
        fd = gbm_device_get_fd(self->gbm_device);
        gbm_device_destroy(self->gbm_device);
        close(fd);
    }

    free(self);
    return ok;
}

static int
wgbm_get_default_fd_for_pattern(const char *pattern)
{
    struct udev *ud;
    struct udev_enumerate *en;
    struct udev_list_entry *devices, *entry;
    const char *path, *filename;
    struct udev_device *device;
    int fd;

    ud = udev_new();
    en = udev_enumerate_new(ud);
    udev_enumerate_add_match_subsystem(en, "drm");
    udev_enumerate_add_match_sysname(en, pattern);
    udev_enumerate_scan_devices(en);
    devices = udev_enumerate_get_list_entry(en);

    udev_list_entry_foreach(entry, devices) {
        path = udev_list_entry_get_name(entry);
        device = udev_device_new_from_syspath(ud, path);
        filename = udev_device_get_devnode(device);
        fd = open(filename, O_RDWR | O_CLOEXEC);
        udev_device_unref(device);
        if (fd >= 0) {
            udev_enumerate_unref(en);
            udev_unref(ud);
            return fd;
        }
    }

    udev_enumerate_unref(en);
    udev_unref(ud);
    return -1;
}

static int
wgbm_get_default_fd(void)
{
    int fd;

    // Try opening render node first
    fd = wgbm_get_default_fd_for_pattern("renderD[0-9]*");
    if (fd >= 0)
        return fd;

    // If render nodes are not supported, then fall back to the card
    fd = wgbm_get_default_fd_for_pattern("card[0-9]*");
    return fd;
}

struct wcore_display*
wgbm_display_connect(struct wcore_platform *wc_plat,
                     const char *name)
{
    struct wgbm_display *self;
    bool ok = true;
    int fd;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    if (name != NULL) {
        fd = open(name, O_RDWR | O_CLOEXEC);
    } else {
        fd = wgbm_get_default_fd();
    }

    if (fd < 0) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "open drm file for gbm failed");
        goto error;
    }

    dlopen("libglapi.so.0", RTLD_LAZY | RTLD_GLOBAL);
    self->gbm_device = gbm_create_device(fd);
    if (!self->gbm_device) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "gbm_create_device failed");
        goto error;
    }

    ok = wegl_display_init(&self->wegl, wc_plat, (intptr_t) self->gbm_device);
    if (!ok)
        goto error;

    return &self->wegl.wcore;

error:
    wgbm_display_destroy(&self->wegl.wcore);
    return NULL;
}

void
wgbm_display_fill_native(struct wgbm_display *self,
                         struct waffle_gbm_display *n_dpy)
{
    n_dpy->gbm_device = self->gbm_device;
    n_dpy->egl_display = self->wegl.egl;
}

union waffle_native_display*
wgbm_display_get_native(struct wcore_display *wc_self)
{
    struct wgbm_display *self = wgbm_display(wc_self);
    union waffle_native_display *n_dpy;

    WCORE_CREATE_NATIVE_UNION(n_dpy, gbm);
    if (n_dpy == NULL)
        return NULL;

    wgbm_display_fill_native(self, n_dpy->gbm);

    return n_dpy;
}
