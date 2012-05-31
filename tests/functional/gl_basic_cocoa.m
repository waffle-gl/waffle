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

#include "gl_basic_cocoa.h"

#include <stdio.h>
#include <Cocoa/Cocoa.h>

static NSAutoreleasePool* pool;

void
gl_basic_cocoa_init(void)
{
    // From the NSApplication Class Reference:
    //     [...] if you do need to use Cocoa classes within the main()
    //     function itself (other than to load nib files or to instantiate
    //     NSApplication), you should create an autorelease pool before using
    //     the classes and then release the pool when you’re done.
    pool = [[NSAutoreleasePool alloc] init];

    // From the NSApplication Class Reference:
    //     The sharedApplication class method initializes the display
    //     environment and connects your program to the window server and the
    //     display server.
    //
    // It also creates the singleton NSApp if it does not yet exist.
    [NSApplication sharedApplication];
}

void
gl_basic_cocoa_finish(void)
{
    // stdout must be flushed before invoking the garbage collector.
    // Otherwise, the content of stdout does not appear over ssh. Weird.
    fflush(stdout);
    [pool drain];
}
