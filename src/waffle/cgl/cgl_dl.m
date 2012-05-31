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
