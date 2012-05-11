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

#include "wcore_tinfo.h"

enum {
    WCORE_ERROR_MESSAGE_BUFSIZE = 1024,
};

struct wcore_error_tinfo {
    bool is_enabled;
    int32_t code;
    char message[WCORE_ERROR_MESSAGE_BUFSIZE];
};

struct wcore_error_tinfo*
wcore_error_tinfo_create(void)
{
    struct wcore_error_tinfo *self = malloc(sizeof(*self));
    if (!self)
        return NULL;

    self->is_enabled = true;
    self->code = WAFFLE_NO_ERROR;
    self->message[0] = 0;

    return self;
}

bool
wcore_error_tinfo_destroy(struct wcore_error_tinfo *self)
{
    free(self);
    return true;
}

void
_wcore_error_enable(void)
{
    wcore_tinfo_get()->error->is_enabled = true;
}

void
_wcore_error_disable(void)
{
    wcore_tinfo_get()->error->is_enabled = false;
}

void
wcore_error(int error)
{
    struct wcore_error_tinfo *t = wcore_tinfo_get()->error;

    if (!t->is_enabled)
        return;

    t->code = error;
    t->message[0] = '\0';
}

void
wcore_errorf(int error, const char *format, ...)
{
    struct wcore_error_tinfo *t = wcore_tinfo_get()->error;
    va_list ap;

    if (!t->is_enabled)
        return;

    t->code = error;
    va_start(ap, format);
    vsnprintf(t->message, WCORE_ERROR_MESSAGE_BUFSIZE - 1, format, ap);
    va_end(ap);
}

void
_wcore_error_internal(const char *file, int line, const char *format, ...)
{
    struct wcore_error_tinfo *t = wcore_tinfo_get()->error;

    char *cur = t->message;
    char *end = t->message + WCORE_ERROR_MESSAGE_BUFSIZE;
    int printed;

    if (!t->is_enabled)
        return;

    t->code = WAFFLE_INTERNAL_ERROR;

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
    return wcore_tinfo_get()->error->code;
}

const char*
wcore_error_get_message(void)
{
    return wcore_tinfo_get()->error->message;
}