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

/// @defgroup waffle_display waffle_display
/// @ingroup waffle_api
/// @{

/// @file

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "waffle_portable.h"

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
