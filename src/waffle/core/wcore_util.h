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

#include <stddef.h>
#include <string.h>
#include "c99_compat.h"

#ifdef __cplusplus
extern "C" {
#endif

#define container_of(ptr, type, member)                                 \
    (type*)((uint8_t *)ptr - offsetof(type, member))

/// @brief Safe downcast using container_of().
///
/// If given a null pointer, return null.
#define DEFINE_CONTAINER_CAST_FUNC(func_name,                           \
                                   container_type,                      \
                                   member_type,                         \
                                   member)                              \
                                                                        \
    static inline container_type*                                       \
    func_name(member_type *member##_self)                               \
    {                                                                   \
        if (member##_self)                                              \
            return container_of(member##_self, container_type, member); \
        else                                                            \
            return 0;                                                   \
    }

/// @brief Addition that detects arithmetic overflow.
///
/// If the addition would result in overflow, then return false and do not
/// update @a res.
bool
wcore_add_size(size_t *res, size_t x, size_t y);

/// @brief In-place variant of wcore_add_size().
static inline bool
wcore_iadd_size(size_t *x, size_t y) {
    return wcore_add_size(x, *x, y);
}

/// @brief Multiplication that detects arithmetic overflow.
///
/// If the multiplication would result in overflow, then return false and do
/// not update @a res.
bool
wcore_mul_size(size_t *res, size_t x, size_t y);

/// @brief In-place variant of wcore_mul_size().
static inline bool
wcore_imul_size(size_t *x, size_t y) {
    return wcore_mul_size(x, *x, y);
}

/// @brief Wrapper around malloc() that emits error if allocation fails.
void*
wcore_malloc(size_t size);

/// @brief Wrapper around calloc() that emits error if allocation fails.
///
/// This has the signature of malloc(), not calloc(), because calloc is too
/// easy to use incorrectly.
void*
wcore_calloc(size_t size);

/// @brief Wrapper around realloc() that emits error if allocation fails.
void*
wcore_realloc(void *ptr, size_t size);

/// @brief Wrapper around strdup() that emits error if allocation fails.
char*
wcore_strdup(const char *str);

/// @brief Create one of `union waffle_native_*`.
///
/// The example below allocates n_dpy and n_dpy->glx, then sets both
/// variables.
///
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/// union waffle_native_display *n_dpy;
/// WCORE_CREATE_NATIVE_UNION(n_dpy, glx);
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///
#define WCORE_CREATE_NATIVE_UNION(union_var, union_member)              \
        do {                                                            \
            union_var = wcore_malloc(sizeof(*union_var) +               \
                                     sizeof(*union_var->union_member)); \
            if (union_var)                                              \
                union_var->union_member = (void*) (union_var + 1);      \
        } while (0)

const char*
wcore_enum_to_string(int32_t e);

#ifdef __cplusplus
}
#endif
