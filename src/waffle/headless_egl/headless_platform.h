#pragma once

#include "wegl_platform.h"
#include "wcore_util.h"

struct headless_platform {
    struct wegl_platform wegl;
    struct linux_platform *linux;
};

DEFINE_CONTAINER_CAST_FUNC(headless_platform,
                           struct headless_platform,
                           struct wegl_platform,
                           wegl)


struct wcore_platform*
headless_platform_create(void);

bool
headless_platform_destroy(struct wcore_platform *wc_self);

bool
headless_dl_can_open(struct wcore_platform *wc_self,
                 int32_t waffle_dl);

void*
headless_dl_sym(struct wcore_platform *wc_self,
            int32_t waffle_dl,
            const char *name);
