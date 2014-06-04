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

static bool
wgl_dl_check_enum(int32_t waffle_dl)
{
    switch (waffle_dl) {
        case WAFFLE_DL_OPENGL:
            return false;
        case WAFFLE_DL_OPENGL_ES1:
            return false;
        case WAFFLE_DL_OPENGL_ES2:
            return false;
        case WAFFLE_DL_OPENGL_ES3:
            return false;
        default:
            assert(false);
            return false;
   }
}

static bool
wgl_dl_open(struct wgl_platform *plat)
{
    return false;
}

bool
wgl_dl_can_open(struct wcore_platform *wc_plat,
                int32_t waffle_dl)
{
    return false;
}

void*
wgl_dl_sym(struct wcore_platform *wc_plat,
          int32_t waffle_dl,
          const char *name)
{
    return NULL;
}

bool
wgl_dl_close(struct wcore_platform *wc_plat)
{
    return false;
}
