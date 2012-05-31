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

#if __STDC_VERSION__ < 199901L
#   define restrict
#endif

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
