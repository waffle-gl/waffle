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

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>

#include "wcore_error.h"

#include "cgl_config.h"
#include "cgl_window.h"

bool
cgl_window_destroy(struct wcore_window *wc_self)
{
    struct cgl_window *self = cgl_window(wc_self);
    bool ok = true;

    if (!self)
        return ok;

    if (self->gl_view)
        [self->gl_view release];

    if (self->ns_window)
        [self->ns_window release];

    ok &= wcore_window_teardown(&self->wcore);
    free(self);
    return ok;
}


static WaffleGLView*
cgl_window_create_gl_view(int width, int height)
{
    WaffleGLView *view = [[WaffleGLView alloc]
                          initWithFrame:NSMakeRect(0, 0, width, height)];

    if (!view)
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "failed to create NSView");

    return view;
}

static NSWindow*
cgl_window_create_ns_window(NSView *view)
{
    // WARNING(chad): I understand neither Objective-C nor Cocoa. This is
    // WARNING(chad): likely all wrong.
    int styleMask = NSTitledWindowMask | NSClosableWindowMask;

    NSWindow *window = [[NSWindow alloc]
                        initWithContentRect:[view frame]
                                  styleMask:styleMask
                                    backing:NSBackingStoreBuffered
                                      defer:NO];

    if (!window) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN, "failed to create NSWindow");
        return NULL;
    }

    [window setContentView:view];
    [window makeFirstResponder:view];
    [window center];
    [window makeKeyAndOrderFront:nil];
    [window orderFrontRegardless];

    return window;
}

struct wcore_window*
cgl_window_create(struct wcore_platform *wc_plat,
                  struct wcore_config *wc_config,
                  int width,
                  int height)
{
    struct cgl_window *self;
    bool ok = true;

    self = wcore_calloc(sizeof(*self));
    if (!self)
        return NULL;

    ok = wcore_window_init(&self->wcore, wc_config);
    if (!ok)
        goto error;

    self->gl_view = cgl_window_create_gl_view(width, height);
    if (!self->gl_view)
        goto error;

    self->ns_window = cgl_window_create_ns_window(self->gl_view);
    if (!self->ns_window)
        goto error;

    return &self->wcore;

error:
    cgl_window_destroy(&self->wcore);
    return NULL;
}

bool
cgl_window_show(struct wcore_window *wc_self)
{
    return true;
}

bool
cgl_window_resize(struct wcore_window *wc_self,
                  int32_t width, int32_t height)
{
    struct cgl_window *self = cgl_window(wc_self);
    [self->ns_window setContentSize:(NSSize){width, height}];
    return true;
}

bool
cgl_window_swap_buffers(struct wcore_window *wc_self)
{
    struct cgl_window *self = cgl_window(wc_self);
    [self->gl_view swapBuffers];
    return true;

}

union waffle_native_window*
cgl_window_get_native(struct wcore_window *wc_self)
{
    wcore_error(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
    return NULL;
}
