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

#include "waffle/api/api_object.h"

#include "wcore_display.h"
#include "wcore_util.h"

struct wcore_config;
union waffle_native_config;

struct wcore_config_vtbl {
    bool
    (*destroy)(struct wcore_config *self);

    union waffle_native_config*
    (*get_native)(struct wcore_config *self);
};

struct wcore_config {
    const struct wcore_config_vtbl *vtbl;

    struct waffle_config {} wfl;
    struct api_object api;

    struct wcore_display *display;
};

DEFINE_CONTAINER_CAST_FUNC(wcore_config,
                           struct wcore_config,
                           struct waffle_config,
                           wfl)

static inline bool
wcore_config_init(struct wcore_config *self,
                  struct wcore_display *display)
{
    assert(self);
    assert(display);

    self->api.display_id = display->api.display_id;
    self->display = display;

    return true;
}

static inline bool
wcore_config_teardown(struct wcore_config *self)
{
    assert(self);
    return true;
}
