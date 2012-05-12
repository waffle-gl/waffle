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

/// @addtogroup x11
/// @{

/// @file

#include "x11.h"

#include <waffle/core/wcore_error.h>

static void
x11_error(const char *func_call)
{
    wcore_errorf(WAFFLE_UNKNOWN_ERROR, "%s", func_call);
}

bool
x11_display_connect(
        const char *name,
        Display **xlib_dpy,
        xcb_connection_t **xcb_conn)
{
    *xlib_dpy = XOpenDisplay(name);
    if (!*xlib_dpy) {
        x11_error("XOpenDisplay");
        return false;
    }

    *xcb_conn = XGetXCBConnection(*xlib_dpy);
    if (!xcb_conn) {
        XCloseDisplay(*xlib_dpy);
        return false;
    }

    return true;
}

bool
x11_display_disconnect(Display *dpy)
{
    int error = XCloseDisplay(dpy);
    if (error)
        x11_error("XCloseDisplay");
    return !error;
}

xcb_window_t
x11_window_create(
        xcb_connection_t *conn,
        xcb_visualid_t visual_id,
        int width,
        int height)
{
    const xcb_setup_t *setup = xcb_get_setup(conn);
    xcb_colormap_t colormap = 0;
    xcb_window_t window = 0;
    if (!setup){
        wcore_errorf(WAFFLE_UNKNOWN_ERROR, "xcb_get_setup() failed");
        goto error;
    }

    const xcb_screen_t *screen = xcb_setup_roots_iterator(setup).data;
    if (!screen) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR, "failed to get xcb screen");
        goto error;
    }

    colormap = xcb_generate_id(conn);
    window = xcb_generate_id(conn);
    if (colormap <= 0 || window <= 0) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR, "xcb_generate_id() failed");
        goto error;
    }

    xcb_void_cookie_t colormap_cookie = xcb_create_colormap_checked(
                                            conn,
                                            XCB_COLORMAP_ALLOC_NONE,
                                            colormap,
                                            screen->root,
                                            visual_id);

    const uint32_t event_mask = XCB_EVENT_MASK_BUTTON_PRESS
                               | XCB_EVENT_MASK_EXPOSURE
                               | XCB_EVENT_MASK_KEY_PRESS;
    const uint32_t attrib_mask = XCB_CW_EVENT_MASK | XCB_CW_COLORMAP;
    const uint32_t attrib_list[] = {event_mask, colormap, 0};

    xcb_void_cookie_t create_cookie = xcb_create_window_checked(
            conn,
            XCB_COPY_FROM_PARENT, // depth
            window,
            screen->root, // parent
            0, 0, // x, y
            width, height,
            0, // border width
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            visual_id,
            attrib_mask,
            attrib_list);

    xcb_void_cookie_t map_cookie = xcb_map_window_checked(conn, window);

    // Check errors.
    xcb_generic_error_t *error;
    error = xcb_request_check(conn, colormap_cookie);
    if (error) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "xcb_create_colormap() failed on visual_id=0x%x with "
                     "error=0x%x\n", visual_id, error->error_code);
        goto error;
    }
    error = xcb_request_check(conn, create_cookie);
    if (error) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "xcb_create_window_checked() failed: error=0x%x",
                     error->error_code);
        goto error;
    }
    error = xcb_request_check(conn, map_cookie);
    if (error) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "xcb_map_window_checked() failed: error=0x%x",
                     error->error_code);
        goto error;
    }

    goto end;

error:
    xcb_free_colormap(conn, colormap);

    if (window)
        xcb_destroy_window(conn, window);
    window = 0;

end:

    return window;
}

bool
x11_window_destroy(
        xcb_connection_t *conn,
        xcb_window_t window)
{
    bool ok = true;

    xcb_void_cookie_t cookie = xcb_destroy_window_checked(conn, window);
    xcb_generic_error_t *error = xcb_request_check(conn, cookie);

    if (error) {
        ok = false;
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "xcb_destroy_window_checked() failed: error=0x%x",
                     error->error_code);
    }

    return ok;
}

/// @}
