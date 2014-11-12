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

#include <stdlib.h>

#include "wcore_error.h"
#include "wcore_util.h"

void*
wcore_malloc(size_t size)
{
    void *p = malloc(size);
    if (p == NULL)
        wcore_error(WAFFLE_ERROR_BAD_ALLOC);
    return p;
}

void*
wcore_calloc(size_t size)
{
    void *p = calloc(1, size);
    if (p == NULL)
        wcore_error(WAFFLE_ERROR_BAD_ALLOC);
    return p;
}

const char*
wcore_enum_to_string(int32_t e)
{
    switch (e) {
        #define CASE(x) case x: return #x

        CASE(WAFFLE_DONT_CARE);
        CASE(WAFFLE_NONE);
        CASE(WAFFLE_PLATFORM);
        CASE(WAFFLE_PLATFORM_ANDROID);
        CASE(WAFFLE_PLATFORM_CGL);
        CASE(WAFFLE_PLATFORM_GLX);
        CASE(WAFFLE_PLATFORM_WAYLAND);
        CASE(WAFFLE_PLATFORM_X11_EGL);
        CASE(WAFFLE_PLATFORM_GBM);
        CASE(WAFFLE_PLATFORM_WGL);
        CASE(WAFFLE_CONTEXT_API);
        CASE(WAFFLE_CONTEXT_OPENGL);
        CASE(WAFFLE_CONTEXT_OPENGL_ES1);
        CASE(WAFFLE_CONTEXT_OPENGL_ES2);
        CASE(WAFFLE_CONTEXT_OPENGL_ES3);
        CASE(WAFFLE_CONTEXT_MAJOR_VERSION);
        CASE(WAFFLE_CONTEXT_MINOR_VERSION);
        CASE(WAFFLE_CONTEXT_PROFILE);
        CASE(WAFFLE_CONTEXT_CORE_PROFILE);
        CASE(WAFFLE_CONTEXT_COMPATIBILITY_PROFILE);
        CASE(WAFFLE_CONTEXT_FORWARD_COMPATIBLE);
        CASE(WAFFLE_CONTEXT_DEBUG);
        CASE(WAFFLE_RED_SIZE);
        CASE(WAFFLE_GREEN_SIZE);
        CASE(WAFFLE_BLUE_SIZE);
        CASE(WAFFLE_ALPHA_SIZE);
        CASE(WAFFLE_DEPTH_SIZE);
        CASE(WAFFLE_STENCIL_SIZE);
        CASE(WAFFLE_SAMPLE_BUFFERS);
        CASE(WAFFLE_SAMPLES);
        CASE(WAFFLE_DOUBLE_BUFFERED);
        CASE(WAFFLE_ACCUM_BUFFER);
        CASE(WAFFLE_DL_OPENGL);
        CASE(WAFFLE_DL_OPENGL_ES1);
        CASE(WAFFLE_DL_OPENGL_ES2);
        CASE(WAFFLE_DL_OPENGL_ES3);

        default: return NULL;

        #undef CASE
    }
}
