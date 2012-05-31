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
