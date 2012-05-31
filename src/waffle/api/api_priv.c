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

/// @addtogroup api_priv
/// @{

/// @file

#include <stdio.h>
#include <stdlib.h>

#include "api_priv.h"

#include <waffle/core/wcore_error.h>
#include <waffle/core/wcore_platform.h>

struct wcore_platform *api_current_platform = 0;

bool
api_check_entry(const struct api_object *obj_list[], int length)
{
    int i;

    wcore_error_reset();

    if (!api_current_platform) {
        wcore_error(WAFFLE_NOT_INITIALIZED);
        return false;
    }

    for (i = 0; i < length; ++i) {
        if (obj_list[i] == NULL) {
            wcore_errorf(WAFFLE_BAD_PARAMETER, "null pointer");
            return false;
        }

        if (obj_list[i]->platform_id != api_current_platform->id) {
            wcore_error(WAFFLE_OLD_OBJECT);
            return false;
        }

        if (obj_list[i]->display_id != obj_list[0]->display_id) {
            wcore_error(WAFFLE_BAD_DISPLAY_MATCH);
        }
    }

    return true;
}

size_t
api_new_object_id(void)
{
    static size_t counter = 1;

    if (counter == 0) {
        fprintf(stderr, "waffle: error: internal counter wrapped to 0\n");
        abort();
    }

    return counter++;
}

/// @}
