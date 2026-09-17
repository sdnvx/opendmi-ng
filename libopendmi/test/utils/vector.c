//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <cmocka.h>

#include <opendmi/internal.h>
#include <opendmi/utils/vector.h>

static void test_vector_init_args(void **pstate);
static void test_vector_init(void **pstate);
static void test_vector_init_matcher(void **pstate);

static void test_vector_push_args(void **pstate);
static void test_vector_push(void **pstate);

static void test_vector_get_ags(void **pstate);
static void test_vector_get_empty(void **pstate);
static void test_vector_get_existing(void **pstate);
static void test_vector_get_missing(void **pstate);

static void test_vector_find_ags(void **pstate);
static void test_vector_find_empty(void **pstate);
static void test_vector_find_existing(void **pstate);
static void test_vector_find_missing(void **pstate);

static void test_vector_exists_args(void **pstate);
static void test_vector_exists_empty(void **pstate);
static void test_vector_exists_existing(void **pstate);
static void test_vector_exists_missing(void **pstate);

static void test_vector_pop_args(void **pstate);
static void test_vector_pop_empty(void **pstate);
static void test_vector_pop(void **pstate);

static void test_vector_clear_args(void **pstate);
static void test_vector_clear_empty(void **pstate);
static void test_vector_clear_full(void **pstate);

static bool test_vector_matcher(uintptr_t entry, uintptr_t key);
static int test_vector_teardown(void **pstate);

static const size_t test_vector_size = 100;

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vector_init_args),
        cmocka_unit_test(test_vector_init),
        cmocka_unit_test(test_vector_init_matcher),

        cmocka_unit_test(test_vector_push_args),
        cmocka_unit_test_teardown(test_vector_push, test_vector_teardown),

        cmocka_unit_test(test_vector_get_ags),
        cmocka_unit_test(test_vector_get_empty),
        cmocka_unit_test_teardown(test_vector_get_existing, test_vector_teardown),
        cmocka_unit_test_teardown(test_vector_get_missing, test_vector_teardown),

        cmocka_unit_test(test_vector_find_ags),
        cmocka_unit_test(test_vector_find_empty),
        cmocka_unit_test_teardown(test_vector_find_existing, test_vector_teardown),
        cmocka_unit_test_teardown(test_vector_find_missing, test_vector_teardown),

        cmocka_unit_test(test_vector_exists_args),
        cmocka_unit_test(test_vector_exists_empty),
        cmocka_unit_test_teardown(test_vector_exists_existing, test_vector_teardown),
        cmocka_unit_test_teardown(test_vector_exists_missing, test_vector_teardown),

        cmocka_unit_test(test_vector_pop_args),
        cmocka_unit_test(test_vector_pop_empty),
        cmocka_unit_test_teardown(test_vector_pop, test_vector_teardown),

        cmocka_unit_test(test_vector_clear_args),
        cmocka_unit_test(test_vector_clear_empty),
        cmocka_unit_test_teardown(test_vector_clear_full, test_vector_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_vector_init_args(void **pstate)
{
    dmi_unused(pstate);

    errno = 0;
    assert_false(dmi_vector_init(nullptr, nullptr));
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_false(dmi_vector_init(nullptr, test_vector_matcher));
    assert_int_equal(errno, EINVAL);
}

static void test_vector_init(void **pstate)
{
    dmi_unused(pstate);

    dmi_vector_t vector = {};

    errno = 0;
    assert_true(dmi_vector_init(&vector, nullptr));
    assert_int_equal(errno, 0);

    assert_null(vector.data);
    assert_int_equal(vector.capacity, 0);
    assert_int_equal(vector.length, 0);
    assert_null(vector.matcher);
}

static void test_vector_init_matcher(void **pstate)
{
    dmi_unused(pstate);

    dmi_vector_t vector = {};

    errno = 0;
    assert_true(dmi_vector_init(&vector, test_vector_matcher));
    assert_int_equal(errno, 0);

    assert_null(vector.data);
    assert_int_equal(vector.capacity, 0);
    assert_int_equal(vector.length, 0);
    assert_ptr_equal(vector.matcher, test_vector_matcher);
}

static void test_vector_push_args(void **pstate)
{
    dmi_unused(pstate);

    errno = 0;
    assert_false(dmi_vector_push(nullptr, 0));
    assert_int_equal(errno, EINVAL);
}

static void test_vector_push(void **pstate)
{
    dmi_unused(pstate);

    static dmi_vector_t vector = {};

    *pstate = &vector;

    for (size_t i = 0; i < test_vector_size; i++) {
        uintptr_t value;

        assert_true(dmi_vector_push(&vector, i));
        assert_non_null(vector.data);
        assert_uint_equal(dmi_vector_length(&vector), i + 1);
        assert_true(vector.capacity >= dmi_vector_length(&vector));

        assert_true(dmi_vector_get(&vector, i, &value));
        assert_int_equal(value, i);
    }
}

static void test_vector_get_ags(void **pstate)
{
    dmi_unused(pstate);

    dmi_vector_t vector = {};
    uintptr_t    value  = 0;

    errno = 0;
    assert_false(dmi_vector_get(nullptr, 0, &value));
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_false(dmi_vector_get(&vector, 0, nullptr));
    assert_int_equal(errno, EINVAL);
}

static void test_vector_get_empty(void **pstate)
{
    dmi_unused(pstate);

    dmi_vector_t vector = {};
    uintptr_t    value  = 0;

    errno = 0;
    assert_false(dmi_vector_get(&vector, 0, &value));
    assert_int_equal(errno, ENOENT);
}

static void test_vector_get_existing(void **pstate)
{
    dmi_unused(pstate);

    static dmi_vector_t vector = {};

    *pstate = &vector;

    for (size_t i = 0; i < test_vector_size; i++) {
        assert_true(dmi_vector_push(&vector, i));
    }

    for (size_t i = 0; i < test_vector_size; i++) {
        uintptr_t value  = 0;

        assert_true(dmi_vector_get(&vector, i, &value));
        assert_uint_equal(value, i);
    }
}

static void test_vector_get_missing(void **pstate)
{
    dmi_unused(pstate);

    static dmi_vector_t vector = {};

    *pstate = &vector;

    for (size_t i = 0; i < test_vector_size; i++) {
        uintptr_t value = 0;

        errno = 0;
        assert_true(dmi_vector_push(&vector, i));
        assert_false(dmi_vector_get(&vector, i + 1, &value));
        assert_int_equal(errno, ENOENT);
    }
}

static void test_vector_find_ags(void **pstate)
{
    dmi_unused(pstate);

    dmi_vector_t vector = { };
    uintptr_t    value  = 0;

    errno = 0;
    assert_false(dmi_vector_find(nullptr, 0, &value));
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_false(dmi_vector_find(&vector, 0, nullptr));
    assert_int_equal(errno, ENOTSUP);
}

static void test_vector_find_empty(void **pstate)
{
    dmi_unused(pstate);

    dmi_vector_t vector = {
        .matcher = test_vector_matcher
    };

    uintptr_t value = 0;

    errno = 0;
    assert_false(dmi_vector_find(&vector, 0, &value));
    assert_int_equal(errno, ENOENT);
}

static void test_vector_find_existing(void **pstate)
{
    dmi_unused(pstate);

    static dmi_vector_t vector = {
        .matcher = test_vector_matcher
    };

    *pstate = &vector;

    for (size_t i = 0; i < test_vector_size; i++) {
        assert_true(dmi_vector_push(&vector, i));
    }

    for (size_t i = 0; i < test_vector_size; i++) {
        uintptr_t value  = 0;

        assert_true(dmi_vector_find(&vector, -i, &value));
        assert_uint_equal(value, i);
    }
}

static void test_vector_find_missing(void **pstate)
{
    dmi_unused(pstate);

    static dmi_vector_t vector = {
        .matcher = test_vector_matcher
    };

    *pstate = &vector;

    for (size_t i = 0; i < test_vector_size; i++) {
        uintptr_t value  = 0;

        assert_true(dmi_vector_push(&vector, i));

        errno = 0;
        assert_false(dmi_vector_find(&vector, -i - 1, &value));
        assert_int_equal(errno, ENOENT);
    }
}

static void test_vector_exists_args(void **pstate)
{
    dmi_unused(pstate);

    errno = 0;
    assert_false(dmi_vector_exists(nullptr, 0));
    assert_int_equal(errno, EINVAL);
}

static void test_vector_exists_empty(void **pstate)
{
    dmi_unused(pstate);

    dmi_vector_t vector = {
        .matcher = test_vector_matcher
    };

    errno = 0;
    assert_false(dmi_vector_exists(&vector, 0));
    assert_int_equal(errno, ENOENT);
}

static void test_vector_exists_existing(void **pstate)
{
    dmi_unused(pstate);

    static dmi_vector_t vector = {};

    *pstate = &vector;

    dmi_vector_init(&vector, test_vector_matcher);

    for (size_t i = 0; i < test_vector_size; i++) {
        assert_true(dmi_vector_push(&vector, i));
    }

    for (size_t i = 0; i < test_vector_size; i++) {
        assert_true(dmi_vector_exists(&vector, -i));
    }
}

static void test_vector_exists_missing(void **pstate)
{
    dmi_unused(pstate);

    static dmi_vector_t vector = {
        .matcher = test_vector_matcher
    };

    *pstate = &vector;

    for (size_t i = 0; i < test_vector_size; i++) {
        assert_true(dmi_vector_push(&vector, i));

        errno = 0;
        assert_false(dmi_vector_exists(&vector, -i - 1));
        assert_int_equal(errno, ENOENT);
    }
}

static void test_vector_pop_args(void **pstate)
{
    dmi_unused(pstate);

    uintptr_t value = 0;

    errno = 0;
    assert_false(dmi_vector_pop(nullptr, &value));
    assert_int_equal(errno, EINVAL);
}

static void test_vector_pop_empty(void **pstate)
{
    dmi_unused(pstate);

    dmi_vector_t vector = {};
    uintptr_t    value  = 0;

    errno = 0;
    assert_false(dmi_vector_pop(&vector, &value));
    assert_int_equal(errno, ENOENT);
    assert_uint_equal(vector.length, 0);

    errno = 0;
    assert_false(dmi_vector_pop(&vector, nullptr));
    assert_int_equal(errno, ENOENT);
    assert_uint_equal(vector.length, 0);
}

static void test_vector_pop(void **pstate)
{
    dmi_unused(pstate);

    static dmi_vector_t vector = {};

    *pstate = &vector;

    for (size_t i = 0; i < test_vector_size; i++) {
        assert_true(dmi_vector_push(&vector, i));
    }

    for (size_t i = 0; i < test_vector_size; i++) {
        uintptr_t value  = 0;

        assert_true(dmi_vector_pop(&vector, &value));

        if (i < test_vector_size - 1) {
            assert_non_null(vector.data);
        } else {
            assert_null(vector.data);
            assert_int_equal(vector.capacity, 0);
        }

        assert_uint_equal(dmi_vector_length(&vector), test_vector_size - i - 1);
        assert_true(vector.capacity >= dmi_vector_length(&vector));
    }
}

static void test_vector_clear_args(void **pstate)
{
    dmi_unused(pstate);

    errno = 0;
    assert_false(dmi_vector_clear(nullptr));
    assert_int_equal(errno, EINVAL);
}

static void test_vector_clear_empty(void **pstate)
{
    dmi_unused(pstate);

    static dmi_vector_t vector = {
        .matcher = test_vector_matcher
    };

    assert_true(dmi_vector_clear(&vector));

    assert_null(vector.data);
    assert_uint_equal(vector.capacity, 0);
    assert_uint_equal(vector.length, 0);
    assert_ptr_equal(vector.matcher, test_vector_matcher);
}

static void test_vector_clear_full(void **pstate)
{
    dmi_unused(pstate);

    static dmi_vector_t vector = {
        .matcher = test_vector_matcher
    };

    *pstate = &vector;

    for (size_t i = 0; i < test_vector_size; i++) {
        assert_true(dmi_vector_push(&vector, i));
    }

    assert_true(dmi_vector_clear(&vector));

    assert_null(vector.data);
    assert_uint_equal(vector.capacity, 0);
    assert_uint_equal(vector.length, 0);
    assert_ptr_equal(vector.matcher, test_vector_matcher);
}

static bool test_vector_matcher(uintptr_t entry, uintptr_t key)
{
    return (intptr_t)entry == -(intptr_t)key;
}

static int test_vector_teardown(void **pstate)
{
    if (*pstate == nullptr)
        return 0;

    dmi_vector_clear((dmi_vector_t *)*pstate);
    *pstate = nullptr;

    return 0;
}
