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

/// @defgroup waffle_display waffle_display
/// @ingroup waffle_api
/// @{

/// @file

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "waffle_visibility.h"

#ifdef __cplusplus
extern "C" {
#endif

struct waffle_display;

WAFFLE_API struct waffle_display*
waffle_display_connect(const char *name);

WAFFLE_API bool
waffle_display_disconnect(struct waffle_display *self);

/// Check if the display supports creation of a @ref waffle_context with
/// the given @a context_api. Choices for @a context_api are
/// `WAFFLE_CONTEXT_OPENGL*`.
WAFFLE_API bool
waffle_display_supports_context_api(
        struct waffle_display *self,
        int32_t context_api);

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
