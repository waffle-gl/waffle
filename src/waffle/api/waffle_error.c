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

#include <string.h>

#include "api_priv.h"

#include "wcore_error.h"

WAFFLE_API enum waffle_error
waffle_error_get_code(void)
{
    return wcore_error_get_code();
}

WAFFLE_API const struct waffle_error_info*
waffle_error_get_info(void)
{
    return wcore_error_get_info();
}

WAFFLE_API const char*
waffle_error_to_string(enum waffle_error e)
{
    switch (e) {
#define CASE(x) case x: return #x
        CASE(WAFFLE_NO_ERROR);
        CASE(WAFFLE_ERROR_FATAL);
        CASE(WAFFLE_ERROR_UNKNOWN);
        CASE(WAFFLE_ERROR_INTERNAL);
        CASE(WAFFLE_ERROR_BAD_ALLOC);
        CASE(WAFFLE_ERROR_NOT_INITIALIZED);
        CASE(WAFFLE_ERROR_ALREADY_INITIALIZED);
        CASE(WAFFLE_ERROR_BAD_ATTRIBUTE);
        CASE(WAFFLE_ERROR_BAD_PARAMETER);
        CASE(WAFFLE_ERROR_BAD_DISPLAY_MATCH);
        CASE(WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM);
        CASE(WAFFLE_ERROR_BUILT_WITHOUT_SUPPORT);
        default: return 0;
#undef CASE
    }
}
