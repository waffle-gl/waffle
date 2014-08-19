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

#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c99_compat.h"
#include "threads.h"

#include <cmocka.h>

#include "wcore_error.h"

static void
test_wcore_error_code_unknown_error(void **state) {
    wcore_error_reset();
    wcore_error(WAFFLE_ERROR_UNKNOWN);
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_UNKNOWN);
    assert_string_equal(wcore_error_get_info()->message, "");

}

static void
test_wcore_error_code_bad_attribute(void **state) {
    wcore_error_reset();
    wcore_error(WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_string_equal(wcore_error_get_info()->message, "");
}

static void
test_wcore_error_with_message(void **state) {
    wcore_error_reset();
    wcore_errorf(WAFFLE_ERROR_BAD_PARAMETER, "bad %s (0x%x)", "gl_api", 0x17);
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_BAD_PARAMETER);
    assert_string_equal(wcore_error_get_info()->message, "bad gl_api (0x17)");
}

static void
test_wcore_error_internal_error(void **state) {
    char error_location[1024];
    snprintf(error_location, 1024, "%s:%d:", __FILE__, __LINE__ + 3);

    wcore_error_reset();
    wcore_error_internal("%s zoroaster %d", "hello", 5);
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_INTERNAL);
    assert_true(strstr(wcore_error_get_info()->message, "hello zoroaster 5"));
    assert_true(strstr(wcore_error_get_info()->message, error_location));
}

static void
test_wcore_error_first_call_without_message_wins(void **state) {
    wcore_error_reset();
    wcore_errorf(WAFFLE_ERROR_UNKNOWN, "cookies");
    wcore_error(WAFFLE_ERROR_BAD_ATTRIBUTE);
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_UNKNOWN);
    assert_string_equal(wcore_error_get_info()->message, "cookies");
}

static void
test_wcore_error_first_call_with_message_wins(void **state) {
    wcore_error_reset();
    wcore_errorf(WAFFLE_ERROR_UNKNOWN, "cookies");
    wcore_errorf(WAFFLE_NO_ERROR, "all is well");
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_UNKNOWN);
    assert_string_equal(wcore_error_get_info()->message, "cookies");
}

static void
test_wcore_error_disable_then_error(void **state) {
    wcore_error_reset();
    wcore_error(WAFFLE_ERROR_NOT_INITIALIZED);
    WCORE_ERROR_DISABLED(
        wcore_error(WAFFLE_ERROR_BAD_ATTRIBUTE);
    );
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_NOT_INITIALIZED);
}

static void
test_wcore_error_disable_then_errorf(void **state) {
    wcore_error_reset();
    wcore_error(WAFFLE_ERROR_NOT_INITIALIZED);
    WCORE_ERROR_DISABLED(
        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE, "i'm not here");
    );
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_NOT_INITIALIZED);
}

static void
test_wcore_error_disable_then_error_internal(void **state) {
    wcore_error_reset();
    wcore_error(WAFFLE_ERROR_NOT_INITIALIZED);
    WCORE_ERROR_DISABLED({
        // Compilation fails with gcc when wcore_error_internal() appears
        // here. So directly the macro's implementing function.
        _wcore_error_internal(__FILE__, __LINE__, "this isn't happening");
    });
    assert_int_equal(wcore_error_get_code(), WAFFLE_ERROR_NOT_INITIALIZED);
}

/// Number of threads in test wcore_error.thread_local.
enum {
    NUM_THREADS = 3,
};

/// Given to thrd_create() in test wcore_error.thread_local.
///
/// A sub-thread, after calling wcore_error(), locks the mutex, increments
/// `num_threads_waiting`, and waits on `cond`. When all sub-threads are
/// waiting (that is, `num_threads_waiting == TOTAL_THREADS`), then the main
/// thread broadcasts on `cond`.
struct thread_arg {
    /// Has value in range `[0, TOTAL_THREADS)`.
    int thread_id;

    /// Protects `num_threads_waiting` and `cond`.
    mtx_t *mutex;

    /// Satisfied when `num_threads_waiting == TOTAL_THREADS`.
    cnd_t *cond;

    /// Number of threads waiting on `cond`.
    volatile int *num_threads_waiting;
};

/// The start routine given to threads in test wcore_error.thread_local.
static bool
thread_start(struct thread_arg *a)
{
    static const enum waffle_error error_codes[NUM_THREADS] = {
        WAFFLE_ERROR_BAD_ATTRIBUTE,
        WAFFLE_ERROR_UNKNOWN,
        WAFFLE_ERROR_ALREADY_INITIALIZED,
    };

    bool ok = true;
    enum waffle_error error_code = error_codes[a->thread_id];

    // Each thread begins in an error-free state.
    ok &= wcore_error_get_code() == WAFFLE_NO_ERROR;

    // Each thread sets its error code.
    wcore_error(error_code);

    // Wait for all threads to set their error codes, thus giving
    // the threads opportunity to clobber each other's codes.
    mtx_lock(a->mutex); {
        *a->num_threads_waiting += 1;
        cnd_wait(a->cond, a->mutex);
        mtx_unlock(a->mutex);
    }

    // Verify that the threads did not clobber each other's
    // error codes.
    ok &= wcore_error_get_code() == error_code;

    return ok;
}

// Test that threads do not clobber each other's error codes.
static void
test_wcore_error_thread_local(void **state) {
    mtx_t mutex;
    cnd_t cond;
    volatile int num_threads_waiting = 0;

    thrd_t threads[NUM_THREADS];
    struct thread_arg thread_args[NUM_THREADS];
    int exit_codes[NUM_THREADS];

    mtx_init(&mutex, mtx_plain);
    cnd_init(&cond);

    for (int i = 0; i < NUM_THREADS; ++i) {
        struct thread_arg *a = &thread_args[i];
        a->thread_id = i;
        a->mutex = &mutex;
        a->cond = &cond;
        a->num_threads_waiting = &num_threads_waiting;

        thrd_create(&threads[i],
                      (thrd_start_t) thread_start,
                      (void*) a);
    }

    // Wait for all threads to set their error codes, thus giving
    // the threads opportunity to clobber each other's codes.
    while (num_threads_waiting < NUM_THREADS)
        ;;
    mtx_lock(&mutex); {
        cnd_broadcast(&cond);
        mtx_unlock(&mutex);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        thrd_join(threads[i], &exit_codes[i]);
        assert_true(exit_codes[i]);
    }

    cnd_destroy(&cond);
    mtx_destroy(&mutex);
}

int
main(void) {
    const UnitTest tests[] = {
        unit_test(test_wcore_error_code_unknown_error),
        unit_test(test_wcore_error_code_bad_attribute),
        unit_test(test_wcore_error_code_unknown_error),
        unit_test(test_wcore_error_with_message),
        unit_test(test_wcore_error_internal_error),
        unit_test(test_wcore_error_first_call_without_message_wins),
        unit_test(test_wcore_error_first_call_with_message_wins),
        unit_test(test_wcore_error_disable_then_error),
        unit_test(test_wcore_error_disable_then_errorf),
        unit_test(test_wcore_error_disable_then_error_internal),
        unit_test(test_wcore_error_thread_local),
    };

    return run_tests(tests);
}
