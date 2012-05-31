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

/// @addtogroup cgl_dl
/// @{

/// @file

#include "cgl_dl.h"

#include <assert.h>
#include <dlfcn.h>

#include <waffle/waffle_enum.h>
#include <waffle/native.h>
#include <waffle/core/wcore_error.h>

#include "cgl_platform.h"

static const char *cgl_dl_gl_path =
    "/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL";

static bool
cgl_dl_check_enum(int32_t waffle_dl)
{
    switch (waffle_dl) {
        case WAFFLE_DL_OPENGL:
            return true;
        case WAFFLE_DL_OPENGL_ES1:
            wcore_errorf(WAFFLE_UNSUPPORTED_ON_PLATFORM,
                         "CGL does not support OpenGL ES1");
            return false;
        case WAFFLE_DL_OPENGL_ES2:
            wcore_errorf(WAFFLE_UNSUPPORTED_ON_PLATFORM,
                         "CGL does not support OpenGL ES2");
            return false;
        default:
            assert(false);
            return false;
   }
}

static bool
cgl_dl_open(union native_platform *platform)
{
    platform->cgl->dl_gl = dlopen(cgl_dl_gl_path, RTLD_LAZY);
    if (!platform->cgl->dl_gl) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "dlopen(\"%s\") failed", cgl_dl_gl_path);
        return false;
    }

    return true;
}

bool
cgl_dl_can_open(
        union native_platform *platform,
        int32_t waffle_dl)
{
    if (!cgl_dl_check_enum(waffle_dl))
        return false;

    if (platform->cgl->dl_gl != NULL)
        return true;

    WCORE_ERROR_DISABLED({
        cgl_dl_open(platform);
    });

    return platform->cgl->dl_gl != NULL;
}

bool
cgl_dl_close(struct cgl_platform *platform)
{
    int error_code = 0;
    const char *error_msg = NULL;

    if (!platform->dl_gl)
        return true;

    error_code = dlclose(platform->dl_gl);

    if (error_code)
        error_msg = dlerror();

    if (error_code && error_msg) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "dlclose(libname=\"%s\") failed: %s",
                     error_msg);
    }
    else if (error_code && !error_msg) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "dlclose(libname=\"%s\") failed");
    }

    return !error_code;
}

void*
cgl_dl_sym(
        union native_platform *platform,
        int32_t waffle_dl,
        const char *symbol)
{
    if (!cgl_dl_check_enum(waffle_dl))
        return NULL;

    if (platform->cgl->dl_gl == NULL)
        cgl_dl_open(platform);

    if (platform->cgl->dl_gl == NULL)
        return NULL;

    // Clear any previous error.
    dlerror();

    void *sym = dlsym(platform->cgl->dl_gl, symbol);

    if (sym)
        return sym;

    // dlsym returned NULL. Check if an error occured.
    const char *error = dlerror();
    if (error) {
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "dlsym(libname=\"%s\", symbol=\"%s\") failed: %s",
                     cgl_dl_gl_path, symbol, error);
    }

    return NULL;
}

/// @}
