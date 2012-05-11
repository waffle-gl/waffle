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

#include "wcore_error.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

enum {
    WCORE_ERROR_MESSAGE_BUFSIZE = 1024,
};

static bool wcore_error_is_enabled = true;
static int wcore_error_code = WAFFLE_NO_ERROR;
static char wcore_error_message[WCORE_ERROR_MESSAGE_BUFSIZE];

void
_wcore_error_enable(void)
{
    wcore_error_is_enabled = true;
}

void
_wcore_error_disable(void)
{
    wcore_error_is_enabled = false;
}

void
wcore_error(int error)
{
    if (!wcore_error_is_enabled)
        return;

    wcore_error_code = error;
    wcore_error_message[0] = '\0';
}

void
wcore_errorf(int error, const char *format, ...)
{
    va_list ap;

    if (!wcore_error_is_enabled)
        return;

    wcore_error_code = error;
    va_start(ap, format);
    vsnprintf(wcore_error_message, WCORE_ERROR_MESSAGE_BUFSIZE - 1, format, ap);
    va_end(ap);
}

void
_wcore_error_internal(const char *file, int line, const char *format, ...)
{
    char *cur = wcore_error_message;
    char *end = wcore_error_message + sizeof(wcore_error_message);
    int printed;

    if (!wcore_error_is_enabled)
        return;

    wcore_error_code = WAFFLE_INTERNAL_ERROR;

    printed = snprintf(cur, end - cur,
                       "waffle: internal error: %s:%d: ", file, line);
    cur += printed;

    if (printed < 0  || cur >= end)
        return;

    if (format) {
        va_list ap;

        va_start(ap, format);
        printed = vsnprintf(cur, end - cur, format, ap);
        cur += printed;
        va_end(ap);

        if (printed < 0 || cur >= end)
            return;
    }

    snprintf(cur, end - cur, " (report this bug to chad@chad-versace.us)");
}

int
wcore_error_get_code(void)
{
    return wcore_error_code;
}

const char*
wcore_error_get_message(void)
{
    return wcore_error_message;
}