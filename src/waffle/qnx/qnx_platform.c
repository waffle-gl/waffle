#include "wegl_context.h"
#include "wegl_util.h"
#include "linux_platform.h"
#include "linux_platform_libs.h"
#include "wegl_config.h"
#include "wegl_platform.h"
#include "qnx_display.h"
#include "qnx_platform.h"
#include "qnx_window.h"

static const struct wcore_platform_vtbl qnx_platform_vtbl;

static bool
qnx_platform_destroy(struct wcore_platform *wc_self)
{
    struct qnx_platform *self = qnx_platform(wegl_platform(wc_self));
    bool ok = true;

    if (!self)
        return true;

    if (self->linux)
        ok &= linux_platform_destroy(self->linux);

    ok &= wcore_platform_teardown(wc_self);

    free(self);
    return ok;
}

struct wcore_platform*
qnx_platform_create(void)
{
    struct linux_platform_libs lib_names = { .libgl = NULL,
                                             .libgles1 = "libGLESv1_CM.so",
                                             .libgles2 = "libGLESv2_viv.so",
                                             .libgles3 = "libGLESv2_viv.so"
                                           };
    struct qnx_platform *self;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    ok = wegl_platform_init(&self->wegl, EGL_NONE);
    if (!ok)
        goto error;

    self->linux = linux_platform_create2(&lib_names);
    if (self->linux == NULL)
        goto error;

    self->wegl.wcore.vtbl = &qnx_platform_vtbl;
    return &self->wegl.wcore;

error:
    qnx_platform_destroy(&self->wegl.wcore);
    return NULL;
}

static bool
qnx_platform_dl_can_open(struct wcore_platform *wc_self,
                         int32_t waffle_dl)
{
    return linux_platform_dl_can_open(qnx_platform(wegl_platform(wc_self))->linux,
                                      waffle_dl);
}

static void*
qnx_platform_dl_sym(struct wcore_platform *wc_self,
                    int32_t waffle_dl,
                    const char *name)
{
    return linux_platform_dl_sym(qnx_platform(wegl_platform(wc_self))->linux,
                                 waffle_dl, name);
}

static bool
qnx_display_supports_context_api(struct wcore_display *wc_self,
                                 int32_t context_api)
{
    switch (context_api) {
        case WAFFLE_CONTEXT_OPENGL_ES2:
        case WAFFLE_CONTEXT_OPENGL_ES3:
            return true;
        case WAFFLE_CONTEXT_OPENGL:
        case WAFFLE_CONTEXT_OPENGL_ES1:
            return false;
        default:
            assert(false);
            return false;
    }
}

static const struct wcore_platform_vtbl qnx_platform_vtbl = {
    .destroy = qnx_platform_destroy,

    .make_current = wegl_make_current,
    .get_proc_address = wegl_get_proc_address,
    .dl_can_open = qnx_platform_dl_can_open,
    .dl_sym = qnx_platform_dl_sym,

    .display = {
        .connect = qnx_display_connect,
        .destroy = qnx_display_disconnect,
        .supports_context_api = qnx_display_supports_context_api,
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
        .create = qnx_window_create,
        .destroy = qnx_window_destroy,
        .show = qnx_window_show,
        .swap_buffers = wegl_surface_swap_buffers,
        .resize = qnx_window_resize,
        .get_native = NULL,
    },
};
