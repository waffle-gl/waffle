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

/// @addtogroup cgl_error
/// @{

/// @file

#include "cgl_error.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include <OpenGL/OpenGL.h>

#include "waffle/core/wcore_error.h"

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
    wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                 "%s failed with %s: %s",
                 func_name,
                 cgl_error_to_string(error_code),
                 CGLErrorString(error_code));
}

/// @}
