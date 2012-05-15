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

/// @addtogroup waffle_dl
/// @{

/// @file

#include <waffle/waffle_dl.h>

#include <waffle/native.h>
#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_error.h>
#include <waffle/core/wcore_platform.h>

#include "api_priv.h"

static bool
waffle_dl_check_enum(int32_t dl)
{
    switch (dl) {
        case WAFFLE_DL_OPENGL:
        case WAFFLE_DL_OPENGL_ES1:
        case WAFFLE_DL_OPENGL_ES2:
            return true;
        default:
            wcore_errorf(WAFFLE_BAD_PARAMETER, "dl has bad value %#x");
            return false;
    }
}

bool
waffle_dl_can_open(int32_t dl)
{
    if (!api_check_entry(NULL, 0))
         return NULL;

     if (!waffle_dl_check_enum(dl))
         return false;

     return api_current_platform->dispatch->
             dl_can_open(api_current_platform->native, dl);
}

void*
waffle_dl_sym(int32_t dl, const char *name)
{
    if (!api_check_entry(NULL, 0))
        return NULL;

    if (!waffle_dl_check_enum(dl))
        return false;

    return api_current_platform->dispatch->
            dl_sym(api_current_platform->native, dl, name);
}

/// @}
