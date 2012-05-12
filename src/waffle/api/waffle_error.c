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

/// @addtogroup waffle_error
/// @{

/// @file

#include <string.h>
#include <waffle/core/wcore_error.h>


static int
min(int x, int y)
{
    if (x < y)
        return x;
    else
        return y;
}

int32_t
waffle_get_error(void)
{
    return wcore_error_get_code();
}

int32_t
waffle_get_error_m(
        char message_buffer[],
        size_t buffer_size,
        size_t *message_length)
{
    const char *m = wcore_error_get_message();
    size_t length = strlen(m);

    if (message_length)
        *message_length = length;

    if (buffer_size > 0)
        memcpy(message_buffer, m, min(buffer_size, length + 1));

    return wcore_error_get_code();
}

const char*
waffle_error_to_string(int32_t e)
{
    switch (e) {
#define CASE(x) case x: return #x
        CASE(WAFFLE_NO_ERROR);
        CASE(WAFFLE_FATAL_ERROR);
        CASE(WAFFLE_UNKNOWN_ERROR);
        CASE(WAFFLE_INTERNAL_ERROR);
        CASE(WAFFLE_OUT_OF_MEMORY);
        CASE(WAFFLE_NOT_INITIALIZED);
        CASE(WAFFLE_ALREADY_INITIALIZED);
        CASE(WAFFLE_OLD_OBJECT);
        CASE(WAFFLE_BAD_ATTRIBUTE);
        CASE(WAFFLE_INCOMPATIBLE_ATTRIBUTES);
        CASE(WAFFLE_BAD_PARAMETER);
        CASE(WAFFLE_UNSUPPORTED_ON_PLATFORM);
        CASE(WAFFLE_NOT_IMPLEMENTED);
        default: return 0;
#undef CASE
    }
}

/// @}
