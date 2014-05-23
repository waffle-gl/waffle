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

#include "api_priv.h"

#include "wcore_error.h"
#include "wcore_platform.h"

static bool
waffle_dl_check_enum(int32_t dl)
{
    switch (dl) {
        case WAFFLE_DL_OPENGL:
        case WAFFLE_DL_OPENGL_ES1:
        case WAFFLE_DL_OPENGL_ES2:
        case WAFFLE_DL_OPENGL_ES3:
            return true;
        default:
            wcore_errorf(WAFFLE_ERROR_BAD_PARAMETER, "dl has bad value %#x");
            return false;
    }
}

WAFFLE_API bool
waffle_dl_can_open(int32_t dl)
{
    if (!api_check_entry(NULL, 0))
         return false;

     if (!waffle_dl_check_enum(dl))
         return false;

     return api_platform->vtbl->dl_can_open(api_platform, dl);
}

WAFFLE_API void*
waffle_dl_sym(int32_t dl, const char *name)
{
    if (!api_check_entry(NULL, 0))
        return NULL;

    if (!waffle_dl_check_enum(dl))
        return NULL;

    return api_platform->vtbl->dl_sym(api_platform, dl, name);
}
