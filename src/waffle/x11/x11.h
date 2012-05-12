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

/// @defgroup x11 x11
///
/// @brief Wrappers for X11 functions.
///
/// Waffle supports two X11 platforms (GLX and X11/EGL). This header declares
/// all common code.
/// @{

/// @file

#pragma once

#include <stdbool.h>
#include <X11/Xlib-xcb.h>

bool
x11_display_connect(
        const char *name,
        Display **xlib_dpy,
        xcb_connection_t **xcb_conn);

bool
x11_display_disconnect(Display *dpy);

xcb_window_t
x11_window_create(
        xcb_connection_t *conn,
        xcb_visualid_t visual_id,
        int width,
        int height);

bool
x11_window_destroy(
        xcb_connection_t *conn,
        xcb_window_t window);

/// @}
