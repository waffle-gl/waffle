#include <stdlib.h>
#include <dlfcn.h>

#include "wcore_error.h"

#include "linux_platform.h"

#include "wegl_config.h"
#include "wegl_context.h"
#include "wegl_platform.h"
#include "wegl_util.h"

#include "headless_display.h"
#include "headless_platform.h"
#include "headless_window.h"

static const struct wcore_platform_vtbl headless_platform_vtbl;

struct wcore_platform*
headless_platform_create(void)
{
    struct headless_platform *self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    bool ok = true;

    ok = wegl_platform_init(&self->wegl);
    if (!ok)
        goto error;

    self->linux = linux_platform_create();
    if (!self->linux)
        goto error;

    self->wegl.wcore.vtbl = &headless_platform_vtbl;

    return &self->wegl.wcore;

error:
    free(self);
    return NULL;
}

bool
headless_platform_destroy(struct wcore_platform *wc_self)
{
    struct headless_platform *self = headless_platform(wegl_platform(wc_self));

    if (!self)
        return true;

    if (self->linux)
         linux_platform_destroy(self->linux);

    wegl_platform_teardown(&self->wegl);

    free(self);
    return true;
}

bool
headless_dl_can_open(struct wcore_platform *wc_self,
                 int32_t waffle_dl)
{
    struct headless_platform *self = headless_platform(wegl_platform(wc_self));
    return linux_platform_dl_can_open(self->linux, waffle_dl);
}

void*
headless_dl_sym(struct wcore_platform *wc_self,
            int32_t waffle_dl,
            const char *name)
{
    struct headless_platform *self = headless_platform(wegl_platform(wc_self));
    return linux_platform_dl_sym(self->linux, waffle_dl, name);
}

static const struct wcore_platform_vtbl headless_platform_vtbl = {
    .destroy = headless_platform_destroy,

    .make_current = wegl_make_current,
    .get_proc_address = wegl_get_proc_address,
    .dl_can_open = headless_dl_can_open,
    .dl_sym = headless_dl_sym,

    .display = {
        .connect = headless_display_connect,
        .destroy = headless_display_destroy,
        .supports_context_api = wegl_display_supports_context_api,
        .get_native = NULL,
    },

    .config = {
        .choose = wegl_config_choose,
        .destroy = wegl_config_destroy,
        .get_native = NULL,
    },

    .context = {
        .create = wegl_context_create,
        .destroy = wegl_context_destroy,
        .get_native = NULL,
    },

    .window = {
        .create = headless_window_create,
        .destroy = headless_window_destroy,
        .show = headless_window_show,
        .swap_buffers = headless_window_swap_buffers,
        .get_native = NULL,
    },
};
