//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/error.h>
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

static void test_vector_context(void **pstate);

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
        cmocka_unit_test_teardown(test_vector_clear_full, test_vector_teardown),
        cmocka_unit_test(test_vector_context)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_vector_init_args(void **pstate)
{
    dmi_unused(pstate);

    assert_false(dmi_vector_initialize(nullptr, nullptr, nullptr));

    assert_false(dmi_vector_initialize(nullptr, nullptr, test_vector_matcher));
}

static void test_vector_init(void **pstate)
{
    dmi_unused(pstate);

    dmi_vector_t vector = {};

    assert_true(dmi_vector_initialize(&vector, nullptr, nullptr));

    assert_null(vector.data);
    assert_int_equal(vector.capacity, 0);
    assert_int_equal(vector.length, 0);
    assert_null(vector.matcher);
}

static void test_vector_init_matcher(void **pstate)
{
    dmi_unused(pstate);

    dmi_vector_t vector = {};

    assert_true(dmi_vector_initialize(&vector, nullptr, test_vector_matcher));

    assert_null(vector.data);
    assert_int_equal(vector.capacity, 0);
    assert_int_equal(vector.length, 0);
    assert_ptr_equal(vector.matcher, test_vector_matcher);
}

static void test_vector_push_args(void **pstate)
{
    dmi_unused(pstate);

    assert_false(dmi_vector_push(nullptr, 0));
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

    assert_false(dmi_vector_get(nullptr, 0, &value));

    assert_false(dmi_vector_get(&vector, 0, nullptr));
}

static void test_vector_get_empty(void **pstate)
{
    dmi_unused(pstate);

    dmi_vector_t vector = {};
    uintptr_t    value  = 0;

    assert_false(dmi_vector_get(&vector, 0, &value));
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
        assert_true(dmi_vector_push(&vector, i));
        assert_false(dmi_vector_get(&vector, i + 1, &value));
    }
}

static void test_vector_find_ags(void **pstate)
{
    dmi_unused(pstate);

    dmi_vector_t vector = { };
    uintptr_t    value  = 0;

    assert_false(dmi_vector_find(nullptr, 0, &value));

    assert_false(dmi_vector_find(&vector, 0, nullptr));
}

static void test_vector_find_empty(void **pstate)
{
    dmi_unused(pstate);

    dmi_vector_t vector = {
        .matcher = test_vector_matcher
    };

    uintptr_t value = 0;

    assert_false(dmi_vector_find(&vector, 0, &value));
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
        assert_false(dmi_vector_find(&vector, -i - 1, &value));
    }
}

static void test_vector_exists_args(void **pstate)
{
    dmi_unused(pstate);

    assert_false(dmi_vector_exists(nullptr, 0));
}

static void test_vector_exists_empty(void **pstate)
{
    dmi_unused(pstate);

    dmi_vector_t vector = {
        .matcher = test_vector_matcher
    };

    assert_false(dmi_vector_exists(&vector, 0));
}

static void test_vector_exists_existing(void **pstate)
{
    dmi_unused(pstate);

    static dmi_vector_t vector = {};

    *pstate = &vector;

    dmi_vector_initialize(&vector, nullptr, test_vector_matcher);

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
        assert_false(dmi_vector_exists(&vector, -i - 1));
    }
}

static void test_vector_pop_args(void **pstate)
{
    dmi_unused(pstate);

    uintptr_t value = 0;

    assert_false(dmi_vector_pop(nullptr, &value));
}

static void test_vector_pop_empty(void **pstate)
{
    dmi_unused(pstate);

    dmi_vector_t vector = {};
    uintptr_t    value  = 0;

    assert_false(dmi_vector_pop(&vector, &value));
    assert_uint_equal(vector.length, 0);

    assert_false(dmi_vector_pop(&vector, nullptr));
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

    assert_false(dmi_vector_clear(nullptr));
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

//
// Errors of a vector are raised against its context, which clearing the
// vector keeps.
//
static void test_vector_context(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(DMI_CONTEXT_FLAG_RELAXED);
    assert_non_null(context);

    dmi_vector_t vector;
    uintptr_t    value = 0;

    assert_true(dmi_vector_initialize(&vector, context, nullptr));
    assert_ptr_equal(vector.context, context);

    assert_false(dmi_vector_get(&vector, 0, &value));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_ARGUMENT_INVALID);

    dmi_error_clear(context);
    assert_false(dmi_vector_exists(&vector, 0));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_STATE_INVALID);

    // Popping an empty vector is not an error
    dmi_error_clear(context);
    assert_false(dmi_vector_pop(&vector, &value));
    assert_null(dmi_error_peek_last(context));

    assert_true(dmi_vector_push(&vector, 1));
    assert_true(dmi_vector_clear(&vector));
    assert_ptr_equal(vector.context, context);

    dmi_destroy(context);
}
