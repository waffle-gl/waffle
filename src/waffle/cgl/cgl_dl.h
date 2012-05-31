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

/// @defgroup cgl_dl cgl_dl
/// @ingroup cgl
/// @{

/// @file

#pragma once

#include <stdbool.h>
#include <stdint.h>

struct cgl_platform;
union native_platform;

bool
cgl_dl_can_open(
        union native_platform *native,
        int32_t waffle_dl);
void*
cgl_dl_sym(
        union native_platform *native,
        int32_t waffle_dl,
        const char *name);

/// Used by cgl_platform_destroy().
bool
cgl_dl_close(struct cgl_platform *platform);

/// @}
