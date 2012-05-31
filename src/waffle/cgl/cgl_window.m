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

/// @addtogroup cgl_window
/// @{

/// @file

#include "cgl_window.h"

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>

#include <waffle/native.h>
#include <waffle/core/wcore_error.h>

static WaffleGLView*
cgl_window_create_gl_view(int width, int height)
{
    WaffleGLView *view = [[[WaffleGLView alloc]
                           initWithFrame:NSMakeRect(0, 0, width, height)]
                          autorelease];

    if (!view)
        wcore_errorf(WAFFLE_UNKNOWN_ERROR, "failed to create NSView");

    return view;
}

static NSWindow*
cgl_window_create_ns_window(NSView *view)
{
    // WARNING(chad): I understand neither Objective-C nor Cocoa. This is
    // WARNING(chad): likely all wrong.
    int styleMask = NSTitledWindowMask | NSClosableWindowMask;

    NSWindow *window = [[[NSWindow alloc]
                         initWithContentRect:[view frame]
                                      styleMask:styleMask
                                        backing:NSBackingStoreBuffered
                                          defer:NO]
                        autorelease];

    if (!window) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR, "failed to create NSWindow");
        return NULL;
    }

    [window setContentView:view];
    [window makeFirstResponder:view];
    [window center];
    [window makeKeyAndOrderFront:nil];

    return window;
}

union native_window*
cgl_window_create(
        union native_config *config,
        int width,
        int height)
{
    union native_window *self;
    NATIVE_ALLOC(self, cgl);
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    self->cgl->gl_view = cgl_window_create_gl_view(width, height);
    if (!self->cgl->gl_view)
        goto error;

    self->cgl->ns_window = cgl_window_create_ns_window(self->cgl->gl_view);
    if (!self->cgl->ns_window)
        goto error;

    return self;

error:
    cgl_window_destroy(self);
    return NULL;
}

bool
cgl_window_destroy(union native_window *self)
{
    if (!self)
        return true;

    if (self->cgl->gl_view)
        [self->cgl->gl_view release];

    if (self->cgl->ns_window)
        [self->cgl->ns_window release];

    free(self);
    return true;
}

bool
cgl_window_show(union native_window *self)
{
    return true;
}

bool
cgl_window_swap_buffers(union native_window *self)
{
    [self->cgl->gl_view swapBuffers];
    return true;
}

/// @}
