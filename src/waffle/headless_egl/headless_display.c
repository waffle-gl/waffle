#include "headless_display.h"
#include "headless_platform.h"

struct wcore_display*
headless_display_connect(struct wcore_platform *wc_plat,
                     const char *name)
{
    struct wegl_display *self;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wegl_display_init(self, wc_plat, (intptr_t) EGL_DEFAULT_DISPLAY);
    if (!ok)
        goto error;

    return &self->wcore;

error:
    free(self);
    return NULL;
}

bool
headless_display_destroy(struct wcore_display *wc_self)
{
    struct wegl_display *self = wegl_display(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    ok &= wegl_display_teardown(self);

    free(self);
    return ok;
}
