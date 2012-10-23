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

/// @addtogroup wcore_tinfo
/// @{

/// @file

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "wcore_error.h"
#include "wcore_tinfo.h"

/// @brief Key for @ref wcore_tinfo_tl_singleton.
static pthread_key_t wcore_tinfo_key;

#ifdef WAFFLE_HAS_TLS

/// @brief Thread-local singleton for thread info.
///
/// There exists a distinct pointer for each thread.
///
/// For documentation on the tls_model, see the GCC manual [1] and
/// Drepper [2].
///
/// [1] GCC 4.7.0 Manual, 6.59 Thread-Local Storage.
///     http://gcc.gnu.org/onlinedocs/gcc-4.7.0/gcc/Thread_002dLocal.html
///
/// [2] Ulrich Drepper. "Elf Handling For Thread Local Storage".
///     http://people.redhat.com/drepper/tls.pdf

static __thread struct wcore_tinfo *wcore_tinfo_tl_singleton
#ifdef WAFFLE_HAS_TLS_MODEL_INITIAL_EXEC
    __attribute__((tls_model("initial-exec")))
#endif
    ;

#endif

static void
wcore_tinfo_destroy(struct wcore_tinfo *self)
{
    if (!self)
        return;

    if (self->error)
        wcore_error_tinfo_destroy(self->error);

    free(self);
}

static struct wcore_tinfo*
wcore_tinfo_create(void)
{
    struct wcore_tinfo *self = calloc(1, sizeof(*self));
    if (!self)
        goto error;

    self->error = wcore_error_tinfo_create();
    if (!self->error)
        goto error;

    return self;

error:
    wcore_tinfo_destroy(self);
    return NULL;
}

/// @brief Initialize @ref wcore_tinfo_key.
///
/// Once intialized, the key is never de-initialized.
static void
wcore_tinfo_key_init(void)
{
    static bool is_init = false;

    // The mutex protects is_init.
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    if (is_init)
        return;

    pthread_mutex_lock(&mutex);
    {
        int error;

        // Another thread may have initialized the key while this thread
        // blocked on the mutex.
        if (is_init)
            goto unlock;

        error = pthread_key_create(&wcore_tinfo_key,
                                   (void (*)(void*)) wcore_tinfo_destroy);
        if (error) {
            printf("waffle: fatal-error: pthread_key_create failed at "
                    "%s:%d\n", __FILE__, __LINE__);
            abort();
        }

        is_init = true;
        unlock:;
    }
    pthread_mutex_unlock(&mutex);
}

struct wcore_tinfo*
wcore_tinfo_get(void)
{
    struct wcore_tinfo *t;

    wcore_tinfo_key_init();

    #ifdef WAFFLE_HAS_TLS
        t = wcore_tinfo_tl_singleton;
    #else
        t = pthread_getspecific(wcore_tinfo_key);
    #endif

    if (t == NULL) {
        // If a key is assigned a non-null pointer, then the key's
        // destructor will be called on that pointer when the thread exits.
        //
        // The key should be assigned to the pointer before the thread-local
        // storage is assigned. If not, a memory leak will occur if a
        // disaster occurs between assignment to thread-local storage and
        // assignment to the key.
        t = wcore_tinfo_create();
        if (!t) {
            printf("waffle: fatal-error: failed to allocate thread info\n");
            abort();
        }
        pthread_setspecific(wcore_tinfo_key, t);

        #ifdef WAFFLE_HAS_TLS
            wcore_tinfo_tl_singleton = t;
        #endif
    }

    return t;
}

/// @}
