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

/// @addtogroup waffle_gl_misc
/// @{

/// @file

#include <waffle/waffle_gl_misc.h>

#include <stddef.h>
#include <string.h>

#include <waffle/native.h>
#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_error.h>
#include <waffle/core/wcore_platform.h>

#include "api_priv.h"

bool
waffle_is_extension_in_string(
        const char *restrict extension_string,
        const char *restrict extension_name)
{
    size_t name_length;
    const char *restrict search_start;

    wcore_error_reset();

    if (extension_string == NULL || extension_name == NULL)
        return false;

    name_length = strlen(extension_name);
    search_start = extension_string;

    if (name_length == 0)
        return false;

    while (true) {
        const char *restrict s;
        const char *restrict next_char;

        s = strstr(search_start, extension_name);
        if (s == NULL)
            return false;

        next_char = s + name_length;
        if (*next_char == ' ' || *next_char == '\0')
            return true;

        // strstr found an extension whose name begins with, but is not
        // equal to, extension_name. Continue the search
        search_start = next_char;
    }
}

bool
waffle_make_current(
        struct waffle_display *dpy,
        struct waffle_window *window,
        struct waffle_context *ctx)
{
    int obj_list_length = 1;

    const struct api_object *obj_list[] = {
        waffle_display_cast_to_api_object(dpy),
        0,
        0,
    };

    if (window)
        obj_list[obj_list_length++] = waffle_window_cast_to_api_object(window);
    if (ctx)
        obj_list[obj_list_length++] = waffle_context_cast_to_api_object(ctx);

    if (!api_check_entry(obj_list, obj_list_length))
        return false;

    return api_current_platform->dispatch->
                make_current(dpy->native,
                             window ? window->native :NULL,
                             ctx ? ctx->native : NULL);
}

void*
waffle_get_proc_address(const char *name)
{
    if (!api_check_entry(NULL, 0))
        return NULL;

    return api_current_platform->dispatch->
            get_proc_address(api_current_platform->native, name);
}

/// @}
