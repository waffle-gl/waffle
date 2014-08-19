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

#include <assert.h>

#include "wcore_error.h"

#include "x11_display.h"
#include "x11_window.h"

static uint8_t
x11_winddow_get_depth(xcb_connection_t *conn,
                      const xcb_screen_t *screen,
                      xcb_visualid_t id)
{
    for (xcb_depth_iterator_t depth =
            xcb_screen_allowed_depths_iterator(screen);
         depth.rem;
         xcb_depth_next(&depth))
    {
        for (xcb_visualtype_iterator_t visual =
                 xcb_depth_visuals_iterator (depth.data);
             visual.rem;
             xcb_visualtype_next(&visual))
        {
            if (visual.data->visual_id == id)
                return depth.data->depth;
        }
    }

    return 0;
}

static xcb_screen_t *
get_xcb_screen(const xcb_setup_t *setup, int screen)
{
    xcb_screen_iterator_t iter;

    iter = xcb_setup_roots_iterator(setup);
    for (; iter.rem; --screen, xcb_screen_next(&iter))
        if (screen == 0)
            return iter.data;

    return NULL;
}

bool
x11_window_init(struct x11_window *self,
                struct x11_display *dpy,
                xcb_visualid_t visual_id,
                int width,
                int height)
{
    xcb_colormap_t colormap = 0;
    xcb_window_t window = 0;

    assert(self);
    assert(dpy);

    bool ok = true;
    xcb_connection_t *conn = dpy->xcb;

    const xcb_setup_t *setup = xcb_get_setup(conn);
    if (!setup){
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "xcb_get_setup() failed");
        goto error;
    }

    const xcb_screen_t *screen = get_xcb_screen(setup, dpy->screen);
    if (!screen) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "failed to get xcb screen");
        goto error;
    }

    colormap = xcb_generate_id(conn);
    window = xcb_generate_id(conn);
    if (colormap <= 0 || window <= 0) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "xcb_generate_id() failed");
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

    // Please keep attrib_mask sorted the same as attrib_list.
    const uint32_t attrib_mask = XCB_CW_BORDER_PIXEL
                               | XCB_CW_EVENT_MASK
                               | XCB_CW_COLORMAP;

    // XCB requires that attrib_list be sorted in the same order as
    // `enum xcb_cw_t`.
    const uint32_t attrib_list[] = {
            /* border_pixel */ 0,
            event_mask,
            colormap,
    };

    xcb_void_cookie_t create_cookie = xcb_create_window_checked(
            conn,
            x11_winddow_get_depth(conn, screen, visual_id),
            window,
            screen->root, // parent
            0, 0, // x, y
            width, height,
            0, // border width
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            visual_id,
            attrib_mask,
            attrib_list);

    // Check errors.
    xcb_generic_error_t *error;
    error = xcb_request_check(conn, colormap_cookie);
    if (error) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "xcb_create_colormap() failed on visual_id=0x%x with "
                     "error=0x%x\n", visual_id, error->error_code);
        goto error;
    }
    error = xcb_request_check(conn, create_cookie);
    if (error) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "xcb_create_window_checked() failed: error=0x%x",
                     error->error_code);
        goto error;
    }

    self->display = dpy;
    self->xcb = window;
    goto end;

error:
    ok = false;

    if (colormap)
        xcb_free_colormap(conn, colormap);

    if (window)
        xcb_destroy_window(conn, window);

end:
    return ok;
}

bool
x11_window_teardown(struct x11_window *self)
{
    xcb_void_cookie_t cookie;
    xcb_generic_error_t *error;

    assert(self);

    if (!self->xcb)
        return true;

    cookie = xcb_destroy_window_checked(self->display->xcb, self->xcb);
    error = xcb_request_check(self->display->xcb, cookie);

    if (error) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "xcb_destroy_window_checked() failed: error=0x%x",
                     error->error_code);
    }

    return !error;
}

bool
x11_window_show(struct x11_window *self)
{
    xcb_void_cookie_t cookie;
    xcb_generic_error_t *error;

    assert(self);

    cookie = xcb_map_window_checked(self->display->xcb, self->xcb);
    error = xcb_request_check(self->display->xcb, cookie);

    if (error) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "xcb_map_window_checked() failed: error=0x%x",
                     error->error_code);
    }

    return !error;
}

bool
x11_window_resize(struct x11_window *self, int32_t width, int32_t height)
{
    xcb_void_cookie_t cookie;
    xcb_generic_error_t *error;

    cookie = xcb_configure_window(
        self->display->xcb, self->xcb,
        XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
        (uint32_t[]){width, height});

    error = xcb_request_check(self->display->xcb, cookie);
    if (error) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "xcb_configure_window() failed to resize width, "
                     "height: error=0x%x\n", error->error_code);
        return false;
    }

    return true;
}
