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

/// @file wcore_tinfo.c
/// @ingroup wcore_tinfo

#include "wcore_tinfo.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "wcore_error.h"

/// @addtogroup wcore_tinfo
/// @{

// ---------------------------------------------------------------------------
/// @name Private
/// @{

/// @brief Key for @ref wcore_tinfo_tl_singleton.
static pthread_key_t wcore_tinfo_key;

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

static __thread
    struct wcore_tinfo *wcore_tinfo_tl_singleton
    __attribute__((tls_model("initial-exec")));

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

/// @} name Private
// ---------------------------------------------------------------------------

struct wcore_tinfo*
wcore_tinfo_get(void)
{
    wcore_tinfo_key_init();

    if (wcore_tinfo_tl_singleton == NULL) {
        // If a key is assigned a non-null pointer, then the key's
        // destructor will be called on that pointer when the thread exits.
        //
        // The key should be assigned to the pointer before the thread-local
        // storage is assigned. If not, a memory leak will occur if a
        // disaster occurs between assignment to thread-local storage and
        // assignment to the key.
        struct wcore_tinfo *t = wcore_tinfo_create();
        if (!t) {
            printf("waffle: fatal-error: failed to allocate thread info\n");
            abort();
        }
        pthread_setspecific(wcore_tinfo_key, t);
        wcore_tinfo_tl_singleton = t;
    }

    return wcore_tinfo_tl_singleton;
}

/// @} addtogroup wcore_tinfo
