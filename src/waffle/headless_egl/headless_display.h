#pragma once

#include "wegl_display.h"

struct wcore_display*
headless_display_connect(struct wcore_platform *wc_plat,
                         const char *name);

bool
headless_display_destroy(struct wcore_display *wc_self);
