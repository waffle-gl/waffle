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

/// @addtogroup xegl_dl
/// @{

/// @file

#include "xegl_dl.h"

#include <waffle/native.h>
#include <waffle/linux/linux_dl.h>
#include <waffle/linux/linux_platform.h>

#include "xegl_priv_types.h"

bool
xegl_dl_can_open(
        union native_platform *native,
        int32_t waffle_dl)
{
    return linux_platform_dl_can_open(native->xegl->linux_, waffle_dl);
}

void*
xegl_dl_sym(
        union native_platform *native,
        int32_t waffle_dl,
        const char *name)
{
    return linux_platform_dl_sym(native->xegl->linux_, waffle_dl, name);
}

/// @}
