// Copyright © 2012, 2019 Intel Corporation
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

#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>

#include <xf86drm.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "wcore_error.h"

#include "wgbm_display.h"
#include "wgbm_platform.h"

bool
wgbm_display_destroy(struct wcore_display *wc_self)
{
    struct wgbm_display *self = wgbm_display(wc_self);
    struct wcore_platform *wc_plat = wc_self->platform;
    struct wgbm_platform *plat = wgbm_platform(wegl_platform(wc_plat));
    bool ok = true;
    int fd;

    if (!self)
        return ok;


    ok &= wegl_display_teardown(&self->wegl);

    if (self->gbm_device) {
        fd = plat->gbm_device_get_fd(self->gbm_device);
        plat->gbm_device_destroy(self->gbm_device);
        close(fd);
    }

    free(self);
    return ok;
}

static int
wgbm_get_fd_for_node_type(drmDevicePtr *devs, int num_devs, int node_type)
{
    for (int i = 0; i < num_devs; i++) {
        if (!(devs[i]->available_nodes & 1 << node_type))
            continue;

        int fd = open(devs[i]->nodes[node_type], O_RDWR | O_CLOEXEC);
        if (fd < 0)
            continue;

        return fd;
    }

    return -1;
}

static int
wgbm_get_default_fd(struct wgbm_platform *plat)
{
    // Checking the first 64 devices is enough for now.
    drmDevicePtr devs[64];

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
    int ret = plat->drm.GetDevices2(0, devs, ARRAY_SIZE(devs));
    if (ret < 0)
        return ret;

    // Try opening render node first, then fall back to the primary
    int fd = wgbm_get_fd_for_node_type(devs, ret, DRM_NODE_RENDER);
    if (fd < 0)
        fd = wgbm_get_fd_for_node_type(devs, ret, DRM_NODE_PRIMARY);

    plat->drm.FreeDevices(devs, ret);
    return fd;
}

struct wcore_display*
wgbm_display_connect(struct wcore_platform *wc_plat,
                     const char *name)
{
    struct wgbm_display *self;
    struct wgbm_platform *plat = wgbm_platform(wegl_platform(wc_plat));
    bool ok = true;
    int fd;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    if (name == NULL) {
        name = getenv("WAFFLE_GBM_DEVICE");
    }

    if (name != NULL) {
        fd = open(name, O_RDWR | O_CLOEXEC);
        if (fd < 0) {
            wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                    "failed to open gbm device \"%s\"", name);
            goto error;
        }
    } else {
        fd = wgbm_get_default_fd(plat);
        if (fd < 0) {
            wcore_errorf(WAFFLE_ERROR_UNKNOWN, "open drm file for gbm failed");
            goto error;
        }
    }

    self->gbm_device = plat->gbm_create_device(fd);
    if (!self->gbm_device) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "gbm_create_device failed");
        goto error;
    }

    ok = wegl_display_init(&self->wegl, wc_plat, self->gbm_device);
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
