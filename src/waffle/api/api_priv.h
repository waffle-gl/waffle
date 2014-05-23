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

#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "waffle.h"

// WAFFLE_API - Declare that a symbol is in Waffle's public API.
//
// See "GCC Wiki - Visibility". (http://gcc.gnu.org/wiki/Visibility).
// See "How to Write Shared Libraries. Ulrich Drepper.
//       (http://www.akkadia.org/drepper/dsohowto.pdf).
//
// TODO: Implement WAFFLE_API for Apple.
//
#if defined(_WIN32)
#   define WAFFLE_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#   define WAFFLE_API __attribute__ ((visibility("default")))
#else
#   define WAFFLE_API
#endif

struct api_object;
struct wcore_platform;

/// @brief Managed by waffle_init().
///
/// This is null if and only if waffle has not been initialized with
/// waffle_init().
extern struct wcore_platform *api_platform;

/// @brief Used to validate most API entry points.
///
/// The objects that the user passed into the API entry point are listed in
/// @a obj_list. If its @a length is 0, then the objects are not validated.
///
/// Emit an error and return false if any of the following:
///     - waffle is not initialized
///     - an object pointer is null
///     - two objects belong to different displays
bool
api_check_entry(const struct api_object *obj_list[], int length);
