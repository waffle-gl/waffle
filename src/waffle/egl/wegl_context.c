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

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "wcore_error.h"

#include "wegl_config.h"
#include "wegl_context.h"
#include "wegl_imports.h"
#include "wegl_platform.h"
#include "wegl_util.h"

static bool
bind_api(struct wegl_platform *plat, int32_t waffle_context_api)
{
    bool ok = true;

    switch (waffle_context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            ok &= plat->eglBindAPI(EGL_OPENGL_API);
            break;
        case WAFFLE_CONTEXT_OPENGL_ES1:
        case WAFFLE_CONTEXT_OPENGL_ES2:
        case WAFFLE_CONTEXT_OPENGL_ES3:
            ok &= plat->eglBindAPI(EGL_OPENGL_ES_API);
            break;
        default:
            wcore_error_internal("waffle_context_api has bad value #x%x",
                                 waffle_context_api);
            return false;
    }

    if (!ok)
        wegl_emit_error(plat, "eglBindAPI");

    return ok;
}

static EGLContext
create_real_context(struct wegl_config *config,
                    EGLContext share_ctx)

{
    struct wegl_display *dpy = wegl_display(config->wcore.display);
    struct wegl_platform *plat = wegl_platform(dpy->wcore.platform);
    struct wcore_config_attrs *attrs = &config->wcore.attrs;
    bool ok = true;
    int32_t waffle_context_api = attrs->context_api;
    EGLint attrib_list[64];
    EGLint context_flags = 0;
    int i = 0;

    if (attrs->context_debug) {
        context_flags |= EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR;
    }

    switch (waffle_context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            if (dpy->KHR_create_context) {
                attrib_list[i++] = EGL_CONTEXT_MAJOR_VERSION_KHR;
                attrib_list[i++] = attrs->context_major_version;
                attrib_list[i++] = EGL_CONTEXT_MINOR_VERSION_KHR;
                attrib_list[i++] = attrs->context_minor_version;
            }
            else {
                assert(attrs->context_major_version == 1);
                assert(attrs->context_minor_version == 0);
            }

            if (attrs->context_forward_compatible) {
                assert(dpy->KHR_create_context);
                context_flags |= EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR;
            }

            if (wcore_config_attrs_version_ge(attrs, 32))  {
                assert(dpy->KHR_create_context);
                switch (attrs->context_profile) {
                    case WAFFLE_CONTEXT_CORE_PROFILE:
                        attrib_list[i++] = EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR;
                        attrib_list[i++] = EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR;
                        break;
                    case WAFFLE_CONTEXT_COMPATIBILITY_PROFILE:
                        attrib_list[i++] = EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR;
                        attrib_list[i++] = EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR;
                        break;
                    default:
                        wcore_error_internal("attrs->context_profile has bad value %#x",
                                             attrs->context_profile);
                        return EGL_NO_CONTEXT;
                }
            }
            break;

        case WAFFLE_CONTEXT_OPENGL_ES1:
        case WAFFLE_CONTEXT_OPENGL_ES2:
        case WAFFLE_CONTEXT_OPENGL_ES3:
            attrib_list[i++] = EGL_CONTEXT_MAJOR_VERSION_KHR;
            attrib_list[i++] = attrs->context_major_version;

            if (dpy->KHR_create_context) {
                attrib_list[i++] = EGL_CONTEXT_MINOR_VERSION_KHR;
                attrib_list[i++] = attrs->context_minor_version;
            }
            else {
                assert(attrs->context_minor_version == 0);
            }

            break;

        default:
            wcore_error_internal("waffle_context_api has bad value %#x",
                                 waffle_context_api);
            return EGL_NO_CONTEXT;
    }

    if (context_flags != 0) {
        attrib_list[i++] = EGL_CONTEXT_FLAGS_KHR;
        attrib_list[i++] = context_flags;
    }

    attrib_list[i++] = EGL_NONE;

    ok = bind_api(plat, waffle_context_api);
    if (!ok)
        return false;

    EGLContext ctx = plat->eglCreateContext(dpy->egl, config->egl,
                                            share_ctx, attrib_list);
    if (!ctx)
        wegl_emit_error(plat, "eglCreateContext");

    return ctx;
}

struct wcore_context*
wegl_context_create(struct wcore_platform *wc_plat,
                    struct wcore_config *wc_config,
                    struct wcore_context *wc_share_ctx)
{
    struct wegl_context *ctx;
    struct wegl_config *config = wegl_config(wc_config);
    struct wegl_context *share_ctx = wegl_context(wc_share_ctx);
    bool ok;

    ctx = wcore_calloc(sizeof(*ctx));
    if (!ctx)
        return NULL;

    ok = wcore_context_init(&ctx->wcore, &config->wcore);
    if (!ok)
        goto fail;

    ctx->egl = create_real_context(config,
                                   share_ctx
                                       ? share_ctx->egl
                                       : NULL);
    if (!ctx->egl)
        goto fail;

    return &ctx->wcore;

fail:
    wegl_context_destroy(&ctx->wcore);
    return NULL;
}

bool
wegl_context_destroy(struct wcore_context *wc_ctx)
{
    struct wegl_display *dpy = wegl_display(wc_ctx->display);
    struct wegl_platform *plat = wegl_platform(dpy->wcore.platform);
    struct wegl_context *ctx;
    bool result = true;

    if (!wc_ctx)
        return result;

    ctx = wegl_context(wc_ctx);

    if (ctx->egl) {
        bool ok = plat->eglDestroyContext(wegl_display(wc_ctx->display)->egl,
                                          ctx->egl);
        if (!ok) {
            wegl_emit_error(plat, "eglDestroyContext");
            result = false;
        }
    }

    result &= wcore_context_teardown(wc_ctx);
    free(ctx);
    return result;
}
