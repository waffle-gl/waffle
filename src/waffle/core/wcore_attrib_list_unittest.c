// Copyright 2013 Intel Corporation
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
#include <string.h>

#include <cmocka.h>

#include "waffle.h"
#include "wcore_attrib_list.h"

static void
test_wcore_attrib_list32_get_null(void **state) {
    int32_t *attrib_list = NULL;
    int32_t key = 0;
    int32_t value;

    assert_false(wcore_attrib_list32_get(attrib_list, key, &value));
}

static void
test_wcore_attrib_list32_get_empty(void **state) {
    int32_t attrib_list[] = { 0 };
    int32_t key = 0;
    int32_t value;

    assert_false(wcore_attrib_list32_get(attrib_list, key, &value));
}

static void
test_wcore_attrib_list32_get_missing_value(void **state) {
    int32_t attrib_list[] = {
        1, 11,
        0,
    };
    int32_t key = 2;
    int32_t value;

    assert_false(wcore_attrib_list32_get(attrib_list, key, &value));
}

static void
test_wcore_attrib_list32_get_trailing_items(void **state) {
    int32_t attrib_list[] = {
        1, 11,
        0,
        2, 22,
    };
    int32_t key = 2;
    int32_t value;

    assert_false(wcore_attrib_list32_get(attrib_list, key, &value));
}

static void
test_wcore_attrib_list32_get_value_not_modified_if_not_found(void **state) {
    int32_t attrib_list[] = {
        1, 11,
        0,
    };
    int32_t value = 17;

    assert_false(wcore_attrib_list32_get(attrib_list, 2, &value));
    assert_int_equal(value, 17);
}

static void
test_wcore_attrib_list32_get_key_is_first(void **state) {
    int32_t attrib_list[] = {
        1, 11,
        2, 22,
        3, 33,
        0,
    };
    int32_t key = 1;
    int32_t value;

    assert_true(wcore_attrib_list32_get(attrib_list, key, &value));
    assert_int_equal(value, 11);
}

static void
test_wcore_attrib_list32_get_key_is_last(void **state) {
    int32_t attrib_list[] = {
        1, 11,
        2, 22,
        3, 33,
        0,
    };
    int32_t key = 3;
    int32_t value;

    assert_true(wcore_attrib_list32_get(attrib_list, key, &value));
    assert_int_equal(value, 33);
}

static void
test_wcore_attrib_list32_length_null(void **state) {
    int32_t *attrib_list = NULL;
    assert_int_equal(wcore_attrib_list32_length(attrib_list), 0);
}

static void
test_wcore_attrib_list32_length_is_0(void **state) {
    int32_t attrib_list[] = {0};
    assert_int_equal(wcore_attrib_list32_length(attrib_list), 0);
}

static void
test_wcore_attrib_list32_length_is_1(void **state) {
    int32_t attrib_list[] = {
        1, 1,
        0,
    };
    assert_int_equal(wcore_attrib_list32_length(attrib_list), 1);
}

static void
test_wcore_attrib_list32_length_is_2(void **state) {
    int32_t attrib_list[] = {
        1, 1,
        2, 2,
        0,
    };
    assert_int_equal(wcore_attrib_list32_length(attrib_list), 2);
}

static void
test_wcore_attrib_list32_length_is_37(void **state) {
    int32_t attrib_list[75];
    memset(attrib_list, 0xff, 74 * sizeof(int32_t));
    attrib_list[74] = 0;

    assert_int_equal(wcore_attrib_list32_length(attrib_list), 37);
}

static void
test_wcore_attrib_list32_length_trailing_items(void **state) {
    int32_t attrib_list[] = {
        1, 1,
        2, 2,
        3, 3,
        0,
        4, 4,
        5, 5,
        0,
    };

    assert_int_equal(wcore_attrib_list32_length(attrib_list), 3);
}

static void
test_wcore_attrib_list32_update_null(void **state) {
    int32_t *attrib_list = NULL;
    assert_false(wcore_attrib_list32_update(attrib_list, 7, 7));
}

static void
test_wcore_attrib_list32_update_empty_list(void **state) {
    int32_t attrib_list[] = {0};
    assert_false(wcore_attrib_list32_update(attrib_list, 7, 7));
}

static void
test_wcore_attrib_list32_update_at_0(void **state) {
    int32_t v;
    int32_t attrib_list[] = {
        10, 10,
        20, 20,
        30, 30,
        0,
    };

    assert_true(wcore_attrib_list32_update(attrib_list, 10, 99));
    assert_true(wcore_attrib_list32_get(attrib_list, 10, &v));
    assert_int_equal(v, 99);
}

static void
test_wcore_attrib_list32_update_at_1(void **state) {
    int32_t v;
    int32_t attrib_list[] = {
        10, 10,
        20, 20,
        30, 30,
        0,
    };

    assert_true(wcore_attrib_list32_update(attrib_list, 20, 99));
    assert_true(wcore_attrib_list32_get(attrib_list, 20, &v));
    assert_int_equal(v, 99);
}

static void
test_wcore_attrib_list32_update_missing_key(void **state) {
    int32_t attrib_list[] = {
        10, 10,
        20, 20,
        30, 30,
        0,
    };

    assert_false(wcore_attrib_list32_update(attrib_list, 50, 99));
}

int
main(void) {
    const UnitTest tests[] = {
        unit_test(test_wcore_attrib_list32_get_null),
        unit_test(test_wcore_attrib_list32_get_empty),
        unit_test(test_wcore_attrib_list32_get_missing_value),
        unit_test(test_wcore_attrib_list32_get_trailing_items),
        unit_test(test_wcore_attrib_list32_get_value_not_modified_if_not_found),
        unit_test(test_wcore_attrib_list32_get_key_is_first),
        unit_test(test_wcore_attrib_list32_get_key_is_last),
        unit_test(test_wcore_attrib_list32_length_null),
        unit_test(test_wcore_attrib_list32_length_is_0),
        unit_test(test_wcore_attrib_list32_length_is_1),
        unit_test(test_wcore_attrib_list32_length_is_2),
        unit_test(test_wcore_attrib_list32_length_is_37),
        unit_test(test_wcore_attrib_list32_length_trailing_items),
        unit_test(test_wcore_attrib_list32_update_null),
        unit_test(test_wcore_attrib_list32_update_empty_list),
        unit_test(test_wcore_attrib_list32_update_at_0),
        unit_test(test_wcore_attrib_list32_update_at_1),
        unit_test(test_wcore_attrib_list32_update_missing_key),
    };

    return run_tests(tests);
}
