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

#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "api_object.h"

#include "wcore_config.h"
#include "wcore_util.h"

struct wcore_context;
struct wcore_display;
union waffle_native_context;

struct wcore_context {
    struct api_object api;
    struct wcore_display *display;
};

static inline struct waffle_context*
waffle_context(struct wcore_context *ctx) {
    return (struct waffle_context*) ctx;
}

static inline struct wcore_context*
wcore_context(struct waffle_context *ctx) {
    return (struct wcore_context*) ctx;
}

static inline bool
wcore_context_init(struct wcore_context *self,
                   struct wcore_config *config)
{
    assert(self);
    assert(config);

    self->api.display_id = config->display->api.display_id;
    self->display = config->display;

    return true;
}

static inline bool
wcore_context_teardown(struct wcore_context *self)
{
    assert(self);
    return true;
}
