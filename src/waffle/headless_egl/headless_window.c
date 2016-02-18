#include "wegl_config.h"
#include "wegl_display.h"
#include "wegl_imports.h"
#include "wegl_platform.h"
#include "wegl_util.h"
#include "wegl_window.h"

#include "headless_window.h"

bool
headless_window_destroy(struct wcore_window *wc_self)
{
    struct wegl_window *self = wegl_window(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    ok &= wegl_window_teardown(self);
    free(self);
    return ok;
}

struct wcore_window*
headless_window_create(struct wcore_platform *wc_plat,
                   struct wcore_config *wc_config,
                   int32_t width,
                   int32_t height,
                   const intptr_t attrib_list[])
{
    struct wegl_window *self;
    struct wegl_config *config = wegl_config(wc_config);
    struct wegl_display *dpy = wegl_display(wc_config->display);
    struct wegl_platform *plat = wegl_platform(dpy->wcore.platform);
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wcore_window_init(&self->wcore, wc_config);
    if (!ok)
        goto error;

    EGLint our_attrib_list[] = {
        EGL_WIDTH, width,
        EGL_HEIGHT, height,
        EGL_NONE,
    };

    self->egl = plat->eglCreatePbufferSurface(dpy->egl, config->egl, our_attrib_list);
    if (!self->egl) {
        wegl_emit_error(plat, "eglCreateWindowSurface");
        goto error;
    }

    return &self->wcore;

error:
    return NULL;
}

bool
headless_window_show(struct wcore_window *wc_self)
{
    return true;
}

bool
headless_window_init(struct wegl_window *window,
                     struct wcore_config *wc_config,
                     intptr_t native_window)
{
    bool ok;

    ok = wcore_window_init(&window->wcore, wc_config);
    if (!ok)
        goto fail;

    return true;

fail:
    wegl_window_teardown(window);
    return false;
}

bool
headless_window_teardown(struct wegl_window *window)
{
    bool result = true;

    result &= wcore_window_teardown(&window->wcore);
    return result;
}

bool
headless_window_swap_buffers(struct wcore_window *wc_window)
{
    (void)wc_window;
    return true;
}
