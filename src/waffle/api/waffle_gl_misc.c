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
