// Copyright 2012 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// @defgroup cgl_window cgl_window
/// @ingroup cgl
/// @{

/// @file

#pragma once

#include <stdbool.h>

#include "WaffleGLView.h"

union native_config;
union native_display;
union native_window;

@class NSWindow;

struct cgl_window {
    NSWindow *ns_window;
    WaffleGLView *gl_view;
};

union native_window*
cgl_window_create(
        union native_config *config,
        int width,
        int height);

bool
cgl_window_destroy(union native_window *self);

bool
cgl_window_show(union native_window *self);

bool
cgl_window_swap_buffers(union native_window *self);

/// @}
