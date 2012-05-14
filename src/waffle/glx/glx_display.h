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

/// @defgroup glx_display glx_display
/// @ingroup glx
/// @{

/// @file

#pragma once

#include <stdbool.h>
#include <stdint.h>

union native_display;
union native_platform;

union native_display*
glx_display_connect(
        union native_platform *platform,
        const char *name);

bool
glx_display_disconnect(union native_display *self);

bool
glx_display_supports_context_api(
        union native_display *self,
        int32_t context_api);

/// @}
