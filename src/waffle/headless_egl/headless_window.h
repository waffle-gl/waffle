#pragma once

#include "wcore_window.h"
#include "wegl_window.h"

bool
headless_window_show(struct wcore_window *wc_self);

struct wcore_window*
headless_window_create(struct wcore_platform *wc_plat,
                   struct wcore_config *wc_config,
                   int32_t width,
                   int32_t height,
                   const intptr_t attrib_list[]);

bool
headless_window_destroy(struct wcore_window *wc_self);

bool
headless_window_init(struct wegl_window *window,
                     struct wcore_config *wc_config,
                     intptr_t native_window);

bool
headless_window_teardown(struct wegl_window *window);

bool
headless_window_swap_buffers(struct wcore_window *wc_window);
