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

/// @defgroup glx_priv_types glx_priv_types
/// @ingroup glx
/// @{

/// @file

#pragma once

#include <GL/glx.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>

struct linux_platform;

union native_display;
union native_platform;

struct glx_platform {
    /// @brief One of WAFFLE_{GL,GLES1,GLES2}.
    int gl_api;

    struct linux_platform *linux_;
};

struct glx_display {
    union native_platform *platform;
    Display *xlib_display;
    xcb_connection_t *xcb_connection;
    int screen;
};

struct glx_config {
    union native_display *display;
    GLXFBConfig glx_fbconfig;
    int32_t glx_fbconfig_id;
    xcb_visualid_t xcb_visual_id;
};

struct glx_context {
    union native_display *display;
    GLXContext glx_context;
};

struct glx_window {
    union native_display *display;
    xcb_window_t xcb_window;
    GLXWindow glx_window;
};

/// @}
