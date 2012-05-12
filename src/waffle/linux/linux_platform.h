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

/// @defgroup linux_platform linux_platform
/// @ingroup linux
/// @{

/// @file

#pragma once

#include <stdbool.h>
#include <stdint.h>

struct linux_platform;

struct linux_platform*
linux_platform_create(void);

bool
linux_platform_destroy(struct linux_platform *self);

/// @copydoc waffle_dl_can_open()
bool
linux_platform_dl_can_open(
        struct linux_platform *self,
        int32_t waffle_dl);

void*
linux_platform_dl_sym(
        struct linux_platform *self,
        int32_t waffle_dl,
        const char *name);

/// @}
