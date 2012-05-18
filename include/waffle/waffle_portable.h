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

/// @defgroup waffle_portable waffle_portable
/// @ingroup waffle_api
///
/// @brief Cross-platform portability.
///
/// Visual Studio doesn't support C99. Ugh.
/// @{

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/// @def WAFFLE_API
/// @brief Declare that a symbol is in Waffle's public API.
////
/// @see "GCC Wiki - Visibility". (http://gcc.gnu.org/wiki/Visibility).
/// @see "How to Write Shared Libraries. Ulrich Drepper.
///       (http://www.akkadia.org/drepper/dsohowto.pdf).
///
/// @todo Implement WAFFLE_API for Apple.
/// @todo Implement WAFFLE_API for Windows.
///
#if defined(__GNUC__) && __GNUC__ >= 4
#   define WAFFLE_API __attribute__ ((visibility("default")))
#else
#   define WAFFLE_API
#endif

/// @def WAFFLE_APIENTRY
///
/// Used only on Windows.
#define WAFFLE_APIENTRY

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
