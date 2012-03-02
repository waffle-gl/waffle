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

#pragma once

#include <stdbool.h>

#include "waffle_visibility.h"

#ifdef __cplusplus
extern "C" {
#endif

struct waffle_context;
struct waffle_window;

WAFFLE_API bool
waffle_make_current(
        struct waffle_window *window,
        struct waffle_context *ctx);

WAFFLE_API void*
waffle_get_proc_address(const char *name);

WAFFLE_API void*
waffle_dlsym_gl(const char *name);

#ifdef __cplusplus
} // end extern "C"
#endif
