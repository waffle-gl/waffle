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

/*
 * C99 restrict keyword
 */
#ifndef restrict
#  if defined(__GNUC__)
#    define restrict __restrict__
#  elif defined(_MSC_VER)
     /* leave empty to prevent compiler wows - decl vs. def diff */
#    define restrict
#  else
#    define restrict
#  endif
#endif

/*
 * C99 inline keyword
 */
#ifndef inline
#  if defined(__GNUC__)
#    define inline __inline__
#  elif defined(_MSC_VER)
#    define inline __inline
#  elif defined(__ICL)
#    define inline __inline
#  else
#    define inline
#  endif
#endif

/*
 * C99 string manipulation functions - snprintf, strcasecmp
 *
 * http://stackoverflow.com/questions/2915672/snprintf-and-visual-studio-2010
 */
#if defined(_MSC_VER)
#include <stdarg.h> // for va_start, va_end
#include <stdio.h>  // for _vscprintf, _vscprintf_s
#include <string.h>

#define strcasecmp _stricmp
#define snprintf c99_snprintf
#define vsnprintf c99_vsnprintf

static inline int
c99_vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
    int count = -1;

    if (size != 0)
        count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
    if (count == -1)
        count = _vscprintf(format, ap);

    return count;
}

static inline int
c99_snprintf(char* str, size_t size, const char* format, ...)
{
    int count;
    va_list ap;

    va_start(ap, format);
    count = c99_vsnprintf(str, size, format, ap);
    va_end(ap);

    return count;
}

#else
#include <strings.h>

#endif
