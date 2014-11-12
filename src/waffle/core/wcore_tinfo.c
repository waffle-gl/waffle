// Copyright 2012-2013 Intel Corporation
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

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "threads.h"

#include "wcore_error.h"
#include "wcore_tinfo.h"

static once_flag wcore_tinfo_once = ONCE_FLAG_INIT;
static tss_t wcore_tinfo_key;

#ifdef WAFFLE_HAS_TLS
/// @brief Thread-local storage for all of Waffle.
///
/// For documentation on the tls_model, see the GCC manual [1] and
/// Drepper [2].
///
/// [1] GCC 4.7.0 Manual, 6.59 Thread-Local Storage.
///     http://gcc.gnu.org/onlinedocs/gcc-4.7.0/gcc/Thread_002dLocal.html
///
/// [2] Ulrich Drepper. "Elf Handling For Thread Local Storage".
///     http://people.redhat.com/drepper/tls.pdf
static __thread struct wcore_tinfo wcore_tinfo
#ifdef WAFFLE_HAS_TLS_MODEL_INITIAL_EXEC
    __attribute__((tls_model("initial-exec")))
#endif
    ;
#endif // WAFFLE_HAS_TLS

#if defined(__GNUC__)
#define NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define NORETURN __declspec(noreturn)
#else
#define NORETURN
#endif

static void NORETURN
wcore_tinfo_abort_init(void)
{
    printf("waffle: fatal-error: failed to initialize thread local info\n");
    abort();
}

static void
wcore_tinfo_key_dtor(void *args)
{
    struct wcore_tinfo *tinfo = args;
    if (!tinfo)
        return;

    wcore_error_tinfo_destroy(tinfo->error);

#ifndef WAFFLE_HAS_TLS
    free(tinfo);
#endif
}

static void
wcore_tinfo_key_create(void)
{
    int err;

    err = tss_create(&wcore_tinfo_key, wcore_tinfo_key_dtor);
    if (err)
        wcore_tinfo_abort_init();
}

static void
wcore_tinfo_init(struct wcore_tinfo *tinfo)
{
    int err;

    if (tinfo->is_init)
        return;

    tinfo->error = wcore_error_tinfo_create();
    if (!tinfo->error)
        wcore_tinfo_abort_init();

    tinfo->is_init = true;

#ifdef WAFFLE_HAS_TLS
    // Register tinfo with the key's destructor to prevent memory leaks at
    // thread exit. The destructor must be registered once per process, but
    // each instance of tinfo must be registered individually. The key's data
    // is never retrieved because use the key only to register tinfo for
    // destruction.

    // With C11 threads call_once "can never fail"...
    // http://open-std.org/twiki/pub/WG14/DefectReports/n1654.htm
    call_once(&wcore_tinfo_once, wcore_tinfo_key_create);
#endif

    err = tss_set(wcore_tinfo_key, tinfo);
    if (err)
        wcore_tinfo_abort_init();
}

struct wcore_tinfo*
wcore_tinfo_get(void)
{
#ifdef WAFFLE_HAS_TLS
    wcore_tinfo_init(&wcore_tinfo);
    return &wcore_tinfo;
#else
    struct wcore_tinfo *tinfo;

    // With C11 threads call_once "can never fail"...
    // http://open-std.org/twiki/pub/WG14/DefectReports/n1654.htm
    call_once(&wcore_tinfo_once, wcore_tinfo_key_create);

    tinfo = tss_get(wcore_tinfo_key);
    if (tinfo)
        return tinfo;

    tinfo = calloc(1, sizeof(*tinfo));
    if (!tinfo)
        wcore_tinfo_abort_init();

    wcore_tinfo_init(tinfo);
    return tinfo;
#endif
}
