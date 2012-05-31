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

/// @addtogroup cgl_error
/// @{

/// @file

#include "cgl_error.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include <OpenGL/OpenGL.h>

#include <waffle/core/wcore_error.h>

const char*
cgl_error_to_string(int error_code)
{
    switch (error_code) {
        #define CASE(x) case x: return #x
        CASE(kCGLNoError);
        CASE(kCGLBadAttribute);
        CASE(kCGLBadProperty);
        CASE(kCGLBadPixelFormat);
        CASE(kCGLBadRendererInfo);
        CASE(kCGLBadContext);
        CASE(kCGLBadDrawable);
        CASE(kCGLBadDisplay);
        CASE(kCGLBadState);
        CASE(kCGLBadValue);
        CASE(kCGLBadMatch);
        CASE(kCGLBadEnumeration);
        CASE(kCGLBadOffScreen);
        CASE(kCGLBadFullScreen);
        CASE(kCGLBadWindow);
        CASE(kCGLBadAddress);
        CASE(kCGLBadCodeModule);
        CASE(kCGLBadAlloc);
        CASE(kCGLBadConnection);
        #undef CASE

        default:
            assert(false);
            return NULL;
    }
}

void
cgl_error_failed_func(const char *func_name, int error_code)
{
    wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                 "%s failed with %s: %s",
                 func_name,
                 cgl_error_to_string(error_code),
                 CGLErrorString(error_code));
}

/// @}
