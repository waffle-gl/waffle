// Copyright 2013 Intel Corporation
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

#include "api_priv.h"

#include "wcore_attrib_list.h"
#include "wcore_error.h"

WAFFLE_API int32_t
waffle_attrib_list_length(const int32_t attrib_list[])
{

    wcore_error_reset();
    return wcore_attrib_list_length(attrib_list);
}

WAFFLE_API bool
waffle_attrib_list_get(
        const int32_t *attrib_list,
        int32_t key,
        int32_t *value)
{
    wcore_error_reset();
    return wcore_attrib_list_get(attrib_list, key, value);
}

WAFFLE_API bool
waffle_attrib_list_get_with_default(
        const int32_t attrib_list[],
        int32_t key,
        int32_t *value,
        int32_t default_value)
{
    wcore_error_reset();
    return wcore_attrib_list_get_with_default(attrib_list, key, value,
                                               default_value);
}

WAFFLE_API bool
waffle_attrib_list_update(
        int32_t *attrib_list,
        int32_t key,
        int32_t value)
{
    wcore_error_reset();
    return wcore_attrib_list_update(attrib_list, key, value);
}
