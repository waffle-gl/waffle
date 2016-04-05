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

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include <dlfcn.h>

#include "wcore_error.h"
#include "wcore_util.h"

#include "linux_dl.h"

struct linux_dl {
    /// @brief For example, "libGLESv2.so.2".
    const char *name;

    /// @brief The library obtained with dlopen().
    ///
    /// The library is initialized if and only if `dl != NULL`.
    void *dl;
};

static const char*
linux_dl_get_name(int32_t waffle_dl)
{
    switch (waffle_dl) {
        case WAFFLE_DL_OPENGL:
#ifdef WAFFLE_HAS_ANDROID
            return NULL;
#else
            return "libGL.so.1";
#endif
        case WAFFLE_DL_OPENGL_ES1:
#ifdef WAFFLE_HAS_ANDROID
            return "libGLESv1_CM.so";
#else
            return "libGLESv1_CM.so.1";
#endif
        case WAFFLE_DL_OPENGL_ES2:
        case WAFFLE_DL_OPENGL_ES3:
            // As of 2014-04-20, Mesa statically provides the ES2 and ES3
            // symbols symbols in libGLESv2.so.2 and provides no ES3-only library
            // 
            // Mesa did not change the library's soname when it added the ES3
            // symbols. The soname was and is libGLESv2.so.2 before and after
            // ES3.
#ifdef WAFFLE_HAS_ANDROID
            return "libGLESv2.so";
#else
            return "libGLESv2.so.2";
#endif
        default:
            assert(false);
            return NULL;
    }
}

struct linux_dl*
linux_dl_open(int32_t waffle_dl)
{
    struct linux_dl *self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    self->name = linux_dl_get_name(waffle_dl);
    if (!self->name)
        goto error;

    self->dl = dlopen(self->name, RTLD_LAZY);
    if (!self->dl) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "dlopen(\"%s\") failed: %s", self->name, dlerror());
        goto error;
    }

    return self;

error:
    free(self);
    return NULL;
}

bool
linux_dl_close(struct linux_dl *self)
{
    int error = 0;

    if (!self)
        return true;

    if (self->dl) {
        error = dlclose(self->dl);
        if (error) {
            wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                         "dlclose(libname=\"%s\") failed: %s",
                         self->name, dlerror());
        }
    }

    free(self);
    return error == 0;
}

void*
linux_dl_sym(struct linux_dl *self, const char *symbol)
{
    // Clear any previous error.
    dlerror();

    void *sym = dlsym(self->dl, symbol);

    const char *error = dlerror();
    if (error) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "dlsym(libname=\"%s\", \"%s\") failed: %s",
                     self->name, symbol, error);
        return NULL;
    }

    return sym;
}
