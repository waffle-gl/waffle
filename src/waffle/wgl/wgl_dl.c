// Copyright 2014 Emil Velikov
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
#include <windows.h>

#include "wcore_error.h"

#include "wgl_dl.h"
#include "wgl_platform.h"

static const char *wgl_gl_path = "OPENGL32";

static bool
wgl_dl_check_enum(int32_t waffle_dl)
{
    switch (waffle_dl) {
        case WAFFLE_DL_OPENGL:
        case WAFFLE_DL_OPENGL_ES1:
        case WAFFLE_DL_OPENGL_ES2:
        case WAFFLE_DL_OPENGL_ES3:
        // OPENGL32.DLL provides GL and GLES* symbols.
            return true;
        default:
            assert(false);
            return false;
   }
}

static bool
wgl_dl_open(struct wgl_platform *plat)
{
    plat->dl_gl = LoadLibraryA(wgl_gl_path);

    if (plat->dl_gl)
        return true;

    int error = GetLastError();

    if (error) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "LoadLibraryA(\"%s\") failed: %d",
                     wgl_gl_path, error);
    }

    return false;
}

bool
wgl_dl_can_open(struct wcore_platform *wc_plat,
                int32_t waffle_dl)
{
    struct wgl_platform *plat = wgl_platform(wc_plat);

    if (!wgl_dl_check_enum(waffle_dl))
        return false;

    if (plat->dl_gl)
        return true;

    WCORE_ERROR_DISABLED({
        wgl_dl_open(plat);
    });

    return plat->dl_gl != NULL;
}

void*
wgl_dl_sym(struct wcore_platform *wc_plat,
          int32_t waffle_dl,
          const char *name)
{
    struct wgl_platform *plat = wgl_platform(wc_plat);

    if (!wgl_dl_check_enum(waffle_dl))
        return NULL;

    if (!plat->dl_gl)
        wgl_dl_open(plat);

    if (!plat->dl_gl)
        return NULL;

    void *sym = GetProcAddress(plat->dl_gl, name);

    if (sym)
        return sym;

    int error = GetLastError();

    if (error) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "GetProcAddress(libname=\"%s\", symbol=\"%s\") failed: %d",
                     wgl_gl_path, name, error);
    }

    return NULL;
}

bool
wgl_dl_close(struct wcore_platform *wc_plat)
{
    struct wgl_platform *plat = wgl_platform(wc_plat);

    if (!plat->dl_gl)
        return true;

    if (FreeLibrary(plat->dl_gl))
        return true;

    int error = GetLastError();

    if (error) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "FreeLibrary(libname=\"%s\") failed: %d",
                     wgl_gl_path, error);
    }

    return false;
}
