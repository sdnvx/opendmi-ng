//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/test/logger.h>

#include <opendmi/buffer.h>

typedef struct test_state test_state_t;

struct test_state
{
    dmi_context_t *context;
    dmi_buffer_t  *buffer;
};

static const dmi_byte_t test_data[] = {
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
};

#define TEST_DATA_LENGTH sizeof(test_data)

static int test_buffer_setup(void **pstate);
static int test_buffer_teardown(void **pstate);

static void test_buffer_create(void **pstate);
static void test_buffer_context(void **pstate);
static void test_buffer_destroy_null(void **pstate);

static void test_buffer_is_empty(void **pstate);

static void test_buffer_assign(void **pstate);
static void test_buffer_assign_copies(void **pstate);
static void test_buffer_assign_replaces(void **pstate);
static void test_buffer_assign_empty(void **pstate);

static void test_buffer_resize_grow(void **pstate);
static void test_buffer_resize_shrink(void **pstate);
static void test_buffer_resize_regrow(void **pstate);

static void test_buffer_read(void **pstate);
static void test_buffer_read_out_of_bounds(void **pstate);

static void test_buffer_write(void **pstate);
static void test_buffer_write_grows(void **pstate);
static void test_buffer_write_fills_gap(void **pstate);
static void test_buffer_write_empty(void **pstate);

static void test_buffer_at(void **pstate);
static void test_buffer_at_out_of_bounds(void **pstate);

static void test_buffer_clear(void **pstate);
static void test_buffer_capacity_steps(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_buffer_create, test_buffer_setup, test_buffer_teardown),
        cmocka_unit_test_setup_teardown(test_buffer_context, test_buffer_setup, test_buffer_teardown),
        cmocka_unit_test(test_buffer_destroy_null),

        cmocka_unit_test_setup_teardown(test_buffer_is_empty, test_buffer_setup, test_buffer_teardown),

        cmocka_unit_test_setup_teardown(test_buffer_assign, test_buffer_setup, test_buffer_teardown),
        cmocka_unit_test_setup_teardown(test_buffer_assign_copies, test_buffer_setup, test_buffer_teardown),
        cmocka_unit_test_setup_teardown(test_buffer_assign_replaces, test_buffer_setup, test_buffer_teardown),
        cmocka_unit_test_setup_teardown(test_buffer_assign_empty, test_buffer_setup, test_buffer_teardown),

        cmocka_unit_test_setup_teardown(test_buffer_resize_grow, test_buffer_setup, test_buffer_teardown),
        cmocka_unit_test_setup_teardown(test_buffer_resize_shrink, test_buffer_setup, test_buffer_teardown),
        cmocka_unit_test_setup_teardown(test_buffer_resize_regrow, test_buffer_setup, test_buffer_teardown),

        cmocka_unit_test_setup_teardown(test_buffer_read, test_buffer_setup, test_buffer_teardown),
        cmocka_unit_test_setup_teardown(test_buffer_read_out_of_bounds, test_buffer_setup, test_buffer_teardown),

        cmocka_unit_test_setup_teardown(test_buffer_write, test_buffer_setup, test_buffer_teardown),
        cmocka_unit_test_setup_teardown(test_buffer_write_grows, test_buffer_setup, test_buffer_teardown),
        cmocka_unit_test_setup_teardown(test_buffer_write_fills_gap, test_buffer_setup, test_buffer_teardown),
        cmocka_unit_test_setup_teardown(test_buffer_write_empty, test_buffer_setup, test_buffer_teardown),

        cmocka_unit_test_setup_teardown(test_buffer_at, test_buffer_setup, test_buffer_teardown),
        cmocka_unit_test_setup_teardown(test_buffer_at_out_of_bounds, test_buffer_setup, test_buffer_teardown),

        cmocka_unit_test_setup_teardown(test_buffer_clear, test_buffer_setup, test_buffer_teardown),
        cmocka_unit_test_setup_teardown(test_buffer_capacity_steps, test_buffer_setup, test_buffer_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_buffer_setup(void **pstate)
{
    test_state_t *state = calloc(1, sizeof(*state));
    if (state == nullptr)
        return -1;

    state->context = dmi_create(0);
    if (state->context == nullptr) {
        free(state);
        return -1;
    }

    dmi_set_logger(state->context, &test_logger);

    state->buffer = dmi_buffer_create(state->context);
    if (state->buffer == nullptr) {
        dmi_destroy(state->context);
        free(state);
        return -1;
    }

    *pstate = state;
    return 0;
}

static int test_buffer_teardown(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_buffer_destroy(state->buffer);
    dmi_destroy(state->context);
    free(state);

    *pstate = nullptr;
    return 0;
}

//
// Memory for the data is not allocated until the buffer is written to or
// grown, so a buffer nothing is put into costs nothing but itself.
//
static void test_buffer_create(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    assert_null(state->buffer->data);
    assert_uint_equal(state->buffer->length, 0);
    assert_uint_equal(state->buffer->capacity, 0);
}

static void test_buffer_context(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    assert_ptr_equal(dmi_buffer_context(state->buffer), state->context);
    assert_null(dmi_buffer_context(nullptr));
}

static void test_buffer_destroy_null(void **pstate)
{
    dmi_unused(pstate);

    dmi_buffer_destroy(nullptr);
}

static void test_buffer_is_empty(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    assert_true(dmi_buffer_is_empty(nullptr));
    assert_true(dmi_buffer_is_empty(state->buffer));

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));
    assert_false(dmi_buffer_is_empty(state->buffer));

    // Buffer which keeps the memory it has had still holds no data
    dmi_buffer_clear(state->buffer);
    assert_true(dmi_buffer_is_empty(state->buffer));
    assert_non_null(state->buffer->data);
}

static void test_buffer_assign(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));

    assert_uint_equal(state->buffer->length, TEST_DATA_LENGTH);
    assert_memory_equal(state->buffer->data, test_data, TEST_DATA_LENGTH);
}

//
// Buffers own their memory, so the data is copied rather than referred to.
//
static void test_buffer_assign_copies(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_byte_t source[TEST_DATA_LENGTH];
    memcpy(source, test_data, sizeof(source));

    assert_true(dmi_buffer_assign(state->buffer, source, sizeof(source)));
    assert_ptr_not_equal(state->buffer->data, source);

    memset(source, 0xFF, sizeof(source));
    assert_memory_equal(state->buffer->data, test_data, TEST_DATA_LENGTH);
}

static void test_buffer_assign_replaces(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t other[] = { 0xAA, 0xBB };

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));
    assert_true(dmi_buffer_assign(state->buffer, other, sizeof(other)));

    assert_uint_equal(state->buffer->length, sizeof(other));
    assert_memory_equal(state->buffer->data, other, sizeof(other));
}

static void test_buffer_assign_empty(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));

    // Nothing to copy needs no data to copy it from
    assert_true(dmi_buffer_assign(state->buffer, nullptr, 0));
    assert_true(dmi_buffer_is_empty(state->buffer));
}

//
// Bytes a longer buffer gains are its own rather than whatever the memory
// held before.
//
static void test_buffer_resize_grow(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t zeros[4] = {};

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));
    assert_true(dmi_buffer_resize(state->buffer, TEST_DATA_LENGTH + 4));

    assert_uint_equal(state->buffer->length, TEST_DATA_LENGTH + 4);
    assert_memory_equal(state->buffer->data, test_data, TEST_DATA_LENGTH);
    assert_memory_equal(state->buffer->data + TEST_DATA_LENGTH, zeros, sizeof(zeros));
}

//
// Bytes a shorter buffer loses are dropped without the memory being given
// back, so that the data written next needs no allocation.
//
static void test_buffer_resize_shrink(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));

    size_t capacity = state->buffer->capacity;

    assert_true(dmi_buffer_resize(state->buffer, 2));
    assert_uint_equal(state->buffer->length, 2);
    assert_uint_equal(state->buffer->capacity, capacity);
    assert_memory_equal(state->buffer->data, test_data, 2);
}

//
// Buffer grown again past what it has been shrunk to gains its own bytes
// rather than the ones it has dropped.
//
static void test_buffer_resize_regrow(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t zeros[TEST_DATA_LENGTH - 2] = {};

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));
    assert_true(dmi_buffer_resize(state->buffer, 2));
    assert_true(dmi_buffer_resize(state->buffer, TEST_DATA_LENGTH));

    assert_memory_equal(state->buffer->data, test_data, 2);
    assert_memory_equal(state->buffer->data + 2, zeros, sizeof(zeros));
}

static void test_buffer_read(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));

    dmi_byte_t value[4] = {};

    assert_true(dmi_buffer_read(state->buffer, value, 2, sizeof(value)));
    assert_memory_equal(value, test_data + 2, sizeof(value));

    // Whole data, and nothing at its very end
    assert_true(dmi_buffer_read(state->buffer, value, 0, 1));
    assert_int_equal(value[0], test_data[0]);

    assert_true(dmi_buffer_read(state->buffer, nullptr, TEST_DATA_LENGTH, 0));
}

static void test_buffer_read_out_of_bounds(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));

    dmi_byte_t value[4] = {};

    assert_false(dmi_buffer_read(state->buffer, value, TEST_DATA_LENGTH - 2, sizeof(value)));
    assert_false(dmi_buffer_read(state->buffer, value, TEST_DATA_LENGTH + 1, 0));

    // Length which would overflow the sum with the offset is rejected as
    // well, rather than wrapping around into a range which fits
    assert_false(dmi_buffer_read(state->buffer, value, 1, SIZE_MAX));
}

static void test_buffer_write(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t value[] = { 0xAA, 0xBB };

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));
    assert_true(dmi_buffer_write(state->buffer, value, 2, sizeof(value)));

    // Data within the buffer is written over, and its length is unchanged
    assert_uint_equal(state->buffer->length, TEST_DATA_LENGTH);
    assert_memory_equal(state->buffer->data, test_data, 2);
    assert_memory_equal(state->buffer->data + 2, value, sizeof(value));
    assert_memory_equal(state->buffer->data + 4, test_data + 4, TEST_DATA_LENGTH - 4);
}

static void test_buffer_write_grows(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    // Buffer which holds nothing yet is written into from its beginning
    assert_true(dmi_buffer_write(state->buffer, test_data, 0, TEST_DATA_LENGTH));
    assert_uint_equal(state->buffer->length, TEST_DATA_LENGTH);
    assert_memory_equal(state->buffer->data, test_data, TEST_DATA_LENGTH);

    // Data past the end of the buffer makes it longer
    assert_true(dmi_buffer_write(state->buffer, test_data, TEST_DATA_LENGTH, TEST_DATA_LENGTH));
    assert_uint_equal(state->buffer->length, TEST_DATA_LENGTH * 2);
    assert_memory_equal(state->buffer->data + TEST_DATA_LENGTH, test_data, TEST_DATA_LENGTH);
}

//
// Bytes between the end of the data and the offset written at are the
// buffer's own, the way the ones a longer buffer gains are.
//
static void test_buffer_write_fills_gap(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t value[] = { 0xAA, 0xBB };
    static const dmi_byte_t zeros[4] = {};

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));
    assert_true(dmi_buffer_write(state->buffer, value, TEST_DATA_LENGTH + 4, sizeof(value)));

    assert_uint_equal(state->buffer->length, TEST_DATA_LENGTH + 4 + sizeof(value));
    assert_memory_equal(state->buffer->data, test_data, TEST_DATA_LENGTH);
    assert_memory_equal(state->buffer->data + TEST_DATA_LENGTH, zeros, sizeof(zeros));
    assert_memory_equal(state->buffer->data + TEST_DATA_LENGTH + 4, value, sizeof(value));
}

static void test_buffer_write_empty(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));

    // Nothing to write leaves the buffer as it is, wherever it is written
    assert_true(dmi_buffer_write(state->buffer, nullptr, TEST_DATA_LENGTH * 4, 0));
    assert_uint_equal(state->buffer->length, TEST_DATA_LENGTH);
}

//
// Data is referred to in place rather than copied, which is how the strings
// and the binary members of the decoded structures refer to it.
//
static void test_buffer_at(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));

    const dmi_data_t *ptr = dmi_buffer_at(state->buffer, 2, 4);

    assert_ptr_equal(ptr, state->buffer->data + 2);
    assert_memory_equal(ptr, test_data + 2, 4);

    // Nothing to refer to at the very end of the data is still within it
    assert_non_null(dmi_buffer_at(state->buffer, TEST_DATA_LENGTH, 0));
}

static void test_buffer_at_out_of_bounds(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));

    assert_null(dmi_buffer_at(state->buffer, TEST_DATA_LENGTH - 2, 4));
    assert_null(dmi_buffer_at(state->buffer, TEST_DATA_LENGTH + 1, 0));
    assert_null(dmi_buffer_at(state->buffer, 1, SIZE_MAX));
}

//
// Clearing drops the data and keeps the memory, so the data written next is
// written into the memory the buffer already has.
//
static void test_buffer_clear(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));

    const dmi_byte_t *data     = state->buffer->data;
    size_t            capacity = state->buffer->capacity;

    dmi_buffer_clear(state->buffer);

    assert_uint_equal(state->buffer->length, 0);
    assert_uint_equal(state->buffer->capacity, capacity);
    assert_ptr_equal(state->buffer->data, data);

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));
    assert_ptr_equal(state->buffer->data, data);
}

//
// Memory grows by whole steps, so that the data written byte by byte does not
// reallocate at every write.
//
static void test_buffer_capacity_steps(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    assert_true(dmi_buffer_write(state->buffer, test_data, 0, 1));

    size_t capacity = state->buffer->capacity;
    assert_true(capacity > 1);

    for (size_t i = 1; i < capacity; i++)
        assert_true(dmi_buffer_write(state->buffer, test_data, i, 1));

    assert_uint_equal(state->buffer->length, capacity);
    assert_uint_equal(state->buffer->capacity, capacity);

    // One byte past the memory it has grows the buffer by another step
    assert_true(dmi_buffer_write(state->buffer, test_data, capacity, 1));
    assert_true(state->buffer->capacity > capacity);
}
