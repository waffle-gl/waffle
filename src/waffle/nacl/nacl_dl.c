// Copyright 2012-2015 Intel Corporation
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
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#include "wcore_error.h"

#include "nacl_container.h"
#include "nacl_dl.h"
#include "nacl_platform.h"


static bool
nacl_dl_check_enum(int32_t waffle_dl)
{
    switch (waffle_dl) {
        case WAFFLE_DL_OPENGL:
            wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                         "NACL does not support OpenGL");
            return false;
        case WAFFLE_DL_OPENGL_ES1:
            wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                         "NACL does not support OpenGL ES1");
            return false;
        case WAFFLE_DL_OPENGL_ES2:
            return true;
        case WAFFLE_DL_OPENGL_ES3:
            wcore_errorf(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM,
                         "NACL does not support OpenGL ES3");
            return false;
        default:
            assert(false);
            return false;
   }
}

static bool
nacl_dl_open(struct nacl_platform *plat)
{
    plat->dl_gl = dlopen(NACL_GLES2_LIBRARY, RTLD_LAZY);

    if (!plat->dl_gl) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "dlopen(\"%s\") failed: %s", NACL_GLES2_LIBRARY, dlerror());
        return false;
    }

    return true;
}

bool
nacl_dl_can_open(struct wcore_platform *wc_plat,
                 int32_t waffle_dl)
{
    struct nacl_platform *plat = nacl_platform(wc_plat);
    bool ok;

    WCORE_ERROR_DISABLED({
        ok = nacl_dl_check_enum(waffle_dl);
    });

    if (!ok)
        return false;

    if (plat->dl_gl != NULL)
        return true;

    WCORE_ERROR_DISABLED({
        nacl_dl_open(plat);
    });

    return plat->dl_gl != NULL;
}

// Construct a string that maps GL function to NaCl function
// by concating given prefix and function name tail from 'src'.
static char *
nacl_dl_prefix(const char *src, const char *prefix)
{
    if (strncmp(src, "gl", 2) != 0) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "NACL symbol name does not start with \"gl\"");
        return NULL;
    }

    uint32_t len = strlen(src) + strlen(prefix);

    char *dst = wcore_calloc(len);
    if (!dst)
        return NULL;

    int n = snprintf(dst, len, "%s%s", prefix, src + 2);
    if (n < 0 || n >= len) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "NACL cannot create symbol prefix");
        free(dst);
        return NULL;
    }

    return dst;
}

void*
nacl_dl_sym(struct wcore_platform *wc_plat,
            int32_t waffle_dl,
            const char *name)
{
    struct nacl_platform *plat = nacl_platform(wc_plat);

    if (!nacl_dl_check_enum(waffle_dl))
        return NULL;

    if (plat->dl_gl == NULL)
        nacl_dl_open(plat);

    if (plat->dl_gl == NULL)
        return NULL;

    char *nacl_name = nacl_dl_prefix(name, "GLES2");
    if (!nacl_name)
        return NULL;

    // Clear any previous error.
    dlerror();

    void *sym = dlsym(plat->dl_gl, name);

    if (sym) {
        free(nacl_name);
        return sym;
    }

    // dlsym returned NULL. Check if an error occured.
    const char *error = dlerror();
    if (error) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "dlsym(libname=\"%s\", symbol=\"%s\") failed: %s",
                     NACL_GLES2_LIBRARY, nacl_name, error);
    }
    free(nacl_name);

    return NULL;
}

bool
nacl_dl_close(struct wcore_platform *wc_plat)
{
    struct nacl_platform *plat = nacl_platform(wc_plat);

    int error_code = 0;
    const char *error_msg = NULL;

    if (!plat->dl_gl)
        return true;

    error_code = dlclose(plat->dl_gl);

    if (!error_code)
        return true;

    error_msg = dlerror();

    if (error_msg) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "dlclose(libname=\"%s\") failed: %s",
                     NACL_GLES2_LIBRARY, error_msg);
    }
    else {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "dlclose(libname=\"%s\") failed",
                     NACL_GLES2_LIBRARY);
    }

    return false;
}
