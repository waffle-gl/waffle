// Copyright 2014 Emil Velikov
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

#pragma once

#include <stdbool.h>

#include "wcore_config_attrs.h"
#include "wcore_context.h"
#include "wcore_util.h"

struct wcore_config;
struct wcore_platform;

struct wgl_context {
    struct wcore_context wcore;
    HGLRC hglrc;
};

static inline struct wgl_context*
wgl_context(struct wcore_context *wcore)
{
    return (struct wgl_context *)wcore;
}

struct wcore_context*
wgl_context_create(struct wcore_platform *wc_plat,
                   struct wcore_config *wc_config,
                   struct wcore_context *wc_share_ctx);

bool
wgl_context_destroy(struct wcore_context *wc_self);

// XXX: Keep in sync with wgl_config_check_context_attrs()
static inline bool
wgl_context_needs_arb_create_context(const struct wcore_config_attrs *attrs)
{
    // Any of the following require the ARB extension, since the data is
    // passed as attributes.

    // Using any GLES* profile,
    if (attrs->context_api != WAFFLE_CONTEXT_OPENGL)
        return true;

    // explicitly requesting OpenGL version (>=3.2),
    if (wcore_config_attrs_version_ge(attrs, 32))
        return true;

    // ... or any of the context flags.
    if (attrs->context_forward_compatible)
        return true;

    if (attrs->context_debug)
        return true;

    if (attrs->context_robust)
        return true;

    return false;
}
