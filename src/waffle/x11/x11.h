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

bool
x11_window_show(
        xcb_connection_t *conn,
        xcb_window_t window);

/// @}
