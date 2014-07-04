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

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c99_compat.h"

#include "wcore_error.h"
#include "wcore_tinfo.h"

enum {
    WCORE_ERROR_MESSAGE_BUFSIZE = 1024,
};

struct wcore_error_tinfo {
    bool is_enabled;
    enum waffle_error code;
    char message[WCORE_ERROR_MESSAGE_BUFSIZE];

    /// @brief The user-visible portion of the error state.
    struct waffle_error_info user_info;
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
wcore_error_reset(void)
{
    struct wcore_error_tinfo *t = wcore_tinfo_get()->error;

    if (!t->is_enabled)
        return;

    t->code = WAFFLE_NO_ERROR;
    t->message[0] = '\0';
}

void
wcore_error(enum waffle_error error)
{
    struct wcore_error_tinfo *t = wcore_tinfo_get()->error;

    if (!t->is_enabled)
        return;

    if (t->code != WAFFLE_NO_ERROR) {
        // Waffle is incapable of emitting a sequence of errors. The first
        // error emitted will likely be the most significant one the
        // sequence, so don't clobber it.
        return;
    }

    t->code = error;
    t->message[0] = '\0';
}

void
wcore_errorf(enum waffle_error error, const char *format, ...)
{
    struct wcore_error_tinfo *t = wcore_tinfo_get()->error;
    va_list ap;

    if (!t->is_enabled)
        return;

    if (t->code != WAFFLE_NO_ERROR) {
        // Waffle is incapable of emitting a sequence of errors. The first
        // error emitted will likely be the most significant one the
        // sequence, so don't clobber it.
        return;
    }

    t->code = error;
    va_start(ap, format);
    vsnprintf(t->message, WCORE_ERROR_MESSAGE_BUFSIZE - 1, format, ap);
    va_end(ap);
}

void
wcore_error_errno(const char *format, ...)
{
   int saved_errno = errno;

   struct wcore_error_tinfo *t = wcore_tinfo_get()->error;
   char *cur = t->message;
   char *end = t->message + WCORE_ERROR_MESSAGE_BUFSIZE;
   int printed;

   if (!t->is_enabled)
       return;

   t->code = WAFFLE_ERROR_UNKNOWN;

   if (format) {
       va_list ap;

       va_start(ap, format);
       printed = vsnprintf(cur, end - cur, format, ap);
       cur += printed;
       va_end(ap);

       if (printed < 0 || cur >= end)
           return;

       printed = snprintf(cur, end - cur, ": ");
       cur += printed;

       if (printed < 0 || cur >= end)
           return;
   }

   strerror_r(saved_errno, cur, end - cur);
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

    // If an error has already been emitted, then clobber it. Internal errors
    // get priority.
    t->code = WAFFLE_ERROR_INTERNAL;

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

    snprintf(cur, end - cur, " ; Please report bug at https://github.com/waffle-gl/waffle/issues");
}

enum waffle_error
wcore_error_get_code(void)
{
    return wcore_tinfo_get()->error->code;
}

const struct waffle_error_info*
wcore_error_get_info(void)
{
    struct wcore_error_tinfo *info = wcore_tinfo_get()->error;

    info->user_info.code = info->code;
    info->user_info.message = info->message;
    info->user_info.message_length = strlen(info->message);

    return &info->user_info;
}
