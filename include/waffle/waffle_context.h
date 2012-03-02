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

struct waffle_config;
struct waffle_context;

WAFFLE_API struct waffle_context*
waffle_context_create(
        struct waffle_config *config,
        struct waffle_context *shared_ctx);

WAFFLE_API bool
waffle_context_destroy(struct waffle_context *self);

#ifdef __cplusplus
} // end extern "C"
#endif
