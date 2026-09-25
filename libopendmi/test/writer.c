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

#include <opendmi/writer.h>

typedef struct test_state test_state_t;

struct test_state
{
    dmi_context_t *context;
    dmi_buffer_t  *buffer;
    dmi_writer_t   writer;
};

static const dmi_byte_t test_data[] = {
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
};

#define TEST_DATA_LENGTH sizeof(test_data)

// Length of the range the bounded tests write into, which the data of the
// fixture is longer than
#define TEST_RANGE_LENGTH 4

// Range of no length of its own, which the writer refuses nothing of
#define TEST_RANGE_FREE   (-1)

static int test_writer_setup(void **pstate);
static int test_writer_teardown(void **pstate);

static void test_writer_initialize_null_writer(void **pstate);
static void test_writer_initialize_null_buffer(void **pstate);
static void test_writer_initialize_past_data(void **pstate);
static void test_writer_initialize(void **pstate);
static void test_writer_initialize_appends(void **pstate);

static void test_writer_put_bytes(void **pstate);
static void test_writer_put_bytes_overwrites(void **pstate);
static void test_writer_put_bytes_bounded(void **pstate);
static void test_writer_put_bytes_within_range(void **pstate);

static void test_writer_put_bytes_at(void **pstate);
static void test_writer_put_bytes_at_grows(void **pstate);
static void test_writer_put_bytes_at_bounded(void **pstate);

static void test_writer_skip(void **pstate);
static void test_writer_skip_keeps_data(void **pstate);
static void test_writer_skip_bounded(void **pstate);

static void test_writer_mark_rewind(void **pstate);
static void test_writer_mark_null(void **pstate);
static void test_writer_mark_foreign(void **pstate);

static void test_writer_skip_ex(void **pstate);
static void test_writer_skip_ex_overrun(void **pstate);
static void test_writer_skip_ex_foreign(void **pstate);

static void test_writer_seek(void **pstate);
static void test_writer_seek_past_data(void **pstate);
static void test_writer_seek_bounded(void **pstate);

static void test_writer_tell(void **pstate);
static void test_writer_reset(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_writer_initialize_null_writer, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_initialize_null_buffer, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_initialize_past_data, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_initialize, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_initialize_appends, test_writer_setup, test_writer_teardown),

        cmocka_unit_test_setup_teardown(test_writer_put_bytes, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_put_bytes_overwrites, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_put_bytes_bounded, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_put_bytes_within_range, test_writer_setup, test_writer_teardown),

        cmocka_unit_test_setup_teardown(test_writer_put_bytes_at, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_put_bytes_at_grows, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_put_bytes_at_bounded, test_writer_setup, test_writer_teardown),

        cmocka_unit_test_setup_teardown(test_writer_skip, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_skip_keeps_data, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_skip_bounded, test_writer_setup, test_writer_teardown),

        cmocka_unit_test_setup_teardown(test_writer_mark_rewind, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_mark_null, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_mark_foreign, test_writer_setup, test_writer_teardown),

        cmocka_unit_test_setup_teardown(test_writer_skip_ex, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_skip_ex_overrun, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_skip_ex_foreign, test_writer_setup, test_writer_teardown),

        cmocka_unit_test_setup_teardown(test_writer_seek, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_seek_past_data, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_seek_bounded, test_writer_setup, test_writer_teardown),

        cmocka_unit_test_setup_teardown(test_writer_tell, test_writer_setup, test_writer_teardown),
        cmocka_unit_test_setup_teardown(test_writer_reset, test_writer_setup, test_writer_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_writer_setup(void **pstate)
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

static int test_writer_teardown(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_buffer_destroy(state->buffer);
    dmi_destroy(state->context);
    free(state);

    *pstate = nullptr;
    return 0;
}

// Fill the buffer of the fixture, and set a writer up over a range of it
static dmi_writer_t *test_writer_open(test_state_t *state, size_t offset, ssize_t length)
{
    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));
    assert_true(dmi_writer_initialize(&state->writer, state->buffer, offset, length));

    return &state->writer;
}

static void test_writer_initialize_null_writer(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    assert_false(dmi_writer_initialize(nullptr, state->buffer, 0, TEST_RANGE_FREE));
}

static void test_writer_initialize_null_buffer(void **pstate)
{
    dmi_unused(pstate);

    dmi_writer_t writer;
    assert_false(dmi_writer_initialize(&writer, nullptr, 0, TEST_RANGE_FREE));
}

//
// Range begins within the data or right at its end, so that the data is
// written or appended to rather than written past.
//
static void test_writer_initialize_past_data(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    assert_true(dmi_buffer_assign(state->buffer, test_data, TEST_DATA_LENGTH));

    dmi_writer_t writer;
    assert_false(dmi_writer_initialize(&writer, state->buffer, TEST_DATA_LENGTH + 1, TEST_RANGE_FREE));
}

static void test_writer_initialize(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    const dmi_writer_t *writer = test_writer_open(state, 2, TEST_RANGE_LENGTH);

    assert_ptr_equal(writer->buffer, state->buffer);
    assert_uint_equal(writer->base, 2);
    assert_int_equal(writer->length, TEST_RANGE_LENGTH);
    assert_uint_equal(dmi_writer_tell(writer), 0);
}

//
// Range beginning at the end of the data appends to it, which is how the
// structures of a table are written one after another.
//
static void test_writer_initialize_appends(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_writer_t *writer = test_writer_open(state, TEST_DATA_LENGTH, TEST_RANGE_FREE);

    assert_true(dmi_writer_put_bytes(writer, test_data, TEST_DATA_LENGTH));

    assert_uint_equal(state->buffer->length, TEST_DATA_LENGTH * 2);
    assert_memory_equal(state->buffer->data, test_data, TEST_DATA_LENGTH);
    assert_memory_equal(state->buffer->data + TEST_DATA_LENGTH, test_data, TEST_DATA_LENGTH);
}

static void test_writer_put_bytes(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t value[] = { 0xAA, 0xBB };

    // Buffer which holds nothing yet, written from its beginning
    dmi_writer_t writer;
    assert_true(dmi_writer_initialize(&writer, state->buffer, 0, TEST_RANGE_FREE));

    assert_true(dmi_writer_put_bytes(&writer, value, sizeof(value)));
    assert_uint_equal(dmi_writer_tell(&writer), sizeof(value));
    assert_uint_equal(state->buffer->length, sizeof(value));
    assert_memory_equal(state->buffer->data, value, sizeof(value));

    // Every write goes on where the last one has ended
    assert_true(dmi_writer_put_bytes(&writer, value, sizeof(value)));
    assert_uint_equal(dmi_writer_tell(&writer), sizeof(value) * 2);
    assert_memory_equal(state->buffer->data + sizeof(value), value, sizeof(value));
}

//
// Range beginning within the data written already writes over it, and the
// bytes after the range are left as they are.
//
static void test_writer_put_bytes_overwrites(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t value[] = { 0xAA, 0xBB };

    dmi_writer_t *writer = test_writer_open(state, 2, TEST_RANGE_FREE);

    assert_true(dmi_writer_put_bytes(writer, value, sizeof(value)));

    assert_uint_equal(state->buffer->length, TEST_DATA_LENGTH);
    assert_memory_equal(state->buffer->data, test_data, 2);
    assert_memory_equal(state->buffer->data + 2, value, sizeof(value));
    assert_memory_equal(state->buffer->data + 4, test_data + 4, TEST_DATA_LENGTH - 4);
}

//
// Range of a length of its own is one which something else follows, so the
// writer refuses to write past it rather than writing over whatever comes
// next.
//
static void test_writer_put_bytes_bounded(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_writer_t *writer = test_writer_open(state, 0, TEST_RANGE_LENGTH);

    assert_false(dmi_writer_put_bytes(writer, test_data, TEST_RANGE_LENGTH + 1));

    // Refused write moves nothing and keeps the data as it was
    assert_uint_equal(dmi_writer_tell(writer), 0);
    assert_memory_equal(state->buffer->data, test_data, TEST_DATA_LENGTH);
}

static void test_writer_put_bytes_within_range(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t value[TEST_RANGE_LENGTH] = { 0xAA, 0xBB, 0xCC, 0xDD };

    dmi_writer_t *writer = test_writer_open(state, 0, TEST_RANGE_LENGTH);

    // Range is written up to its very end, and no further
    assert_true(dmi_writer_put_bytes(writer, value, sizeof(value)));
    assert_uint_equal(dmi_writer_tell(writer), TEST_RANGE_LENGTH);
    assert_false(dmi_writer_put_bytes(writer, value, 1));

    assert_memory_equal(state->buffer->data, value, sizeof(value));
    assert_memory_equal(state->buffer->data + TEST_RANGE_LENGTH,
                        test_data + TEST_RANGE_LENGTH,
                        TEST_DATA_LENGTH - TEST_RANGE_LENGTH);
}

//
// Value the rest of the data decides, such as a length, is filled in once it
// is known, without the cursor being moved.
//
static void test_writer_put_bytes_at(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t value[] = { 0xAA, 0xBB };

    dmi_writer_t *writer = test_writer_open(state, 0, TEST_RANGE_FREE);

    assert_true(dmi_writer_skip(writer, 4));
    assert_true(dmi_writer_put_bytes_at(writer, value, 1, sizeof(value)));

    assert_uint_equal(dmi_writer_tell(writer), 4);
    assert_memory_equal(state->buffer->data + 1, value, sizeof(value));
    assert_memory_equal(state->buffer->data + 3, test_data + 3, TEST_DATA_LENGTH - 3);
}

//
// Data grows if the bytes are written past its end, and the ones between the
// end and the offset are made up as zeros.
//
static void test_writer_put_bytes_at_grows(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t value[] = { 0xAA, 0xBB };
    static const dmi_byte_t zeros[2] = {};

    dmi_writer_t *writer = test_writer_open(state, TEST_DATA_LENGTH, TEST_RANGE_FREE);

    assert_true(dmi_writer_put_bytes_at(writer, value, 2, sizeof(value)));

    assert_uint_equal(state->buffer->length, TEST_DATA_LENGTH + 2 + sizeof(value));
    assert_memory_equal(state->buffer->data, test_data, TEST_DATA_LENGTH);
    assert_memory_equal(state->buffer->data + TEST_DATA_LENGTH, zeros, sizeof(zeros));
    assert_memory_equal(state->buffer->data + TEST_DATA_LENGTH + 2, value, sizeof(value));
}

static void test_writer_put_bytes_at_bounded(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_writer_t *writer = test_writer_open(state, 0, TEST_RANGE_LENGTH);

    assert_false(dmi_writer_put_bytes_at(writer, test_data, TEST_RANGE_LENGTH - 1, 2));
    assert_false(dmi_writer_put_bytes_at(writer, test_data, TEST_RANGE_LENGTH, 1));

    assert_memory_equal(state->buffer->data, test_data, TEST_DATA_LENGTH);
}

//
// Padding makes up only the bytes past the end of the data, as zeros, which
// is what the bytes a structure holds and nothing reads are left as.
//
static void test_writer_skip(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t zeros[4] = {};

    dmi_writer_t writer;
    assert_true(dmi_writer_initialize(&writer, state->buffer, 0, TEST_RANGE_FREE));

    assert_true(dmi_writer_skip(&writer, sizeof(zeros)));

    assert_uint_equal(dmi_writer_tell(&writer), sizeof(zeros));
    assert_uint_equal(state->buffer->length, sizeof(zeros));
    assert_memory_equal(state->buffer->data, zeros, sizeof(zeros));
}

//
// Bytes written already are stepped over rather than written over, so that
// padding over a part of the data which is being written again keeps it.
//
static void test_writer_skip_keeps_data(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t zeros[4] = {};

    dmi_writer_t *writer = test_writer_open(state, 0, TEST_RANGE_FREE);

    assert_true(dmi_writer_skip(writer, TEST_DATA_LENGTH));

    assert_uint_equal(state->buffer->length, TEST_DATA_LENGTH);
    assert_memory_equal(state->buffer->data, test_data, TEST_DATA_LENGTH);

    // Padding past the data is the only part which is made up
    assert_true(dmi_writer_skip(writer, sizeof(zeros)));
    assert_uint_equal(state->buffer->length, TEST_DATA_LENGTH + sizeof(zeros));
    assert_memory_equal(state->buffer->data, test_data, TEST_DATA_LENGTH);
    assert_memory_equal(state->buffer->data + TEST_DATA_LENGTH, zeros, sizeof(zeros));
}

static void test_writer_skip_bounded(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_writer_t *writer = test_writer_open(state, 0, TEST_RANGE_LENGTH);

    assert_false(dmi_writer_skip(writer, TEST_RANGE_LENGTH + 1));
    assert_uint_equal(dmi_writer_tell(writer), 0);

    assert_true(dmi_writer_skip(writer, TEST_RANGE_LENGTH));
    assert_false(dmi_writer_skip(writer, 1));
}

static void test_writer_mark_rewind(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t value[] = { 0xAA, 0xBB };

    dmi_writer_t *writer = test_writer_open(state, 0, TEST_RANGE_FREE);

    dmi_writer_mark_t mark = dmi_writer_mark(writer);

    assert_true(dmi_writer_skip(writer, 4));
    assert_uint_equal(dmi_writer_tell(writer), 4);

    // Coming back to the mark writes the bytes counted over it
    assert_true(dmi_writer_rewind(writer, mark));
    assert_uint_equal(dmi_writer_tell(writer), 0);

    assert_true(dmi_writer_put_bytes(writer, value, sizeof(value)));
    assert_memory_equal(state->buffer->data, value, sizeof(value));
    assert_uint_equal(state->buffer->length, TEST_DATA_LENGTH);
}

static void test_writer_mark_null(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_writer_t *writer = test_writer_open(state, 2, TEST_RANGE_FREE);

    // Mark of no writer belongs to no buffer, so it is rejected rather than
    // taken for the beginning of the data
    dmi_writer_mark_t mark = dmi_writer_mark(nullptr);

    assert_false(dmi_writer_rewind(writer, mark));
    assert_false(dmi_writer_skip_ex(writer, mark, 1));
    assert_uint_equal(dmi_writer_tell(writer), 0);
}

static void test_writer_mark_foreign(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_writer_t *writer = test_writer_open(state, 0, TEST_RANGE_FREE);

    dmi_writer_t other;
    assert_true(dmi_writer_initialize(&other, state->buffer, 2, TEST_RANGE_FREE));

    // Marks belong to the range they were taken of, even when the other
    // writer writes into the same buffer
    dmi_writer_mark_t mark = dmi_writer_mark(&other);

    assert_true(dmi_writer_skip(writer, 4));

    assert_false(dmi_writer_rewind(writer, mark));
    assert_false(dmi_writer_skip_ex(writer, mark, 8));
    assert_uint_equal(dmi_writer_tell(writer), 4);
}

//
// Record is padded out to its declared length rather than to the number of
// the bytes written into it.
//
static void test_writer_skip_ex(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t value[] = { 0xAA, 0xBB };
    static const dmi_byte_t zeros[2] = {};

    dmi_writer_t writer;
    assert_true(dmi_writer_initialize(&writer, state->buffer, 0, TEST_RANGE_FREE));

    dmi_writer_mark_t mark = dmi_writer_mark(&writer);

    assert_true(dmi_writer_put_bytes(&writer, value, sizeof(value)));
    assert_true(dmi_writer_skip_ex(&writer, mark, 4));

    assert_uint_equal(dmi_writer_tell(&writer), 4);
    assert_uint_equal(state->buffer->length, 4);
    assert_memory_equal(state->buffer->data, value, sizeof(value));
    assert_memory_equal(state->buffer->data + sizeof(value), zeros, sizeof(zeros));

    // The same mark counts from where it was taken, not from the cursor
    assert_true(dmi_writer_skip_ex(&writer, mark, 6));
    assert_uint_equal(dmi_writer_tell(&writer), 6);
}

//
// Record longer than its own length is one the caller has miscounted, and
// padding it out cannot undo that.
//
static void test_writer_skip_ex_overrun(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_writer_t writer;
    assert_true(dmi_writer_initialize(&writer, state->buffer, 0, TEST_RANGE_FREE));

    dmi_writer_mark_t mark = dmi_writer_mark(&writer);

    assert_true(dmi_writer_put_bytes(&writer, test_data, TEST_DATA_LENGTH));

    assert_false(dmi_writer_skip_ex(&writer, mark, TEST_DATA_LENGTH - 1));
    assert_uint_equal(dmi_writer_tell(&writer), TEST_DATA_LENGTH);

    // Record written to exactly its own length needs no padding at all
    assert_true(dmi_writer_skip_ex(&writer, mark, TEST_DATA_LENGTH));
    assert_uint_equal(dmi_writer_tell(&writer), TEST_DATA_LENGTH);
}

static void test_writer_skip_ex_foreign(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_buffer_t *other_buffer = dmi_buffer_create(state->context);
    assert_non_null(other_buffer);

    dmi_writer_t *writer = test_writer_open(state, 0, TEST_RANGE_FREE);

    dmi_writer_t other;
    assert_true(dmi_writer_initialize(&other, other_buffer, 0, TEST_RANGE_FREE));

    assert_false(dmi_writer_skip_ex(writer, dmi_writer_mark(&other), 1));
    assert_false(dmi_writer_rewind(writer, dmi_writer_mark(&other)));

    dmi_buffer_destroy(other_buffer);
}

//
// Cursor is moved over the bytes written so far, which the data of the
// buffer holds past the beginning of the range.
//
static void test_writer_seek(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t value[] = { 0xAA, 0xBB };

    dmi_writer_t *writer = test_writer_open(state, 2, TEST_RANGE_FREE);

    assert_true(dmi_writer_seek(writer, 2));
    assert_uint_equal(dmi_writer_tell(writer), 2);

    assert_true(dmi_writer_put_bytes(writer, value, sizeof(value)));
    assert_memory_equal(state->buffer->data + 4, value, sizeof(value));

    // End of the data is a position the cursor may be moved to
    assert_true(dmi_writer_seek(writer, TEST_DATA_LENGTH - 2));
    assert_uint_equal(dmi_writer_tell(writer), TEST_DATA_LENGTH - 2);
}

static void test_writer_seek_past_data(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_writer_t *writer = test_writer_open(state, 2, TEST_RANGE_FREE);

    assert_false(dmi_writer_seek(writer, TEST_DATA_LENGTH - 1));
    assert_uint_equal(dmi_writer_tell(writer), 0);
}

//
// Data past the end of a range of a length of its own belongs to whatever
// comes next, and is none of the writer's business.
//
static void test_writer_seek_bounded(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_writer_t *writer = test_writer_open(state, 0, TEST_RANGE_LENGTH);

    assert_true(dmi_writer_seek(writer, TEST_RANGE_LENGTH));
    assert_false(dmi_writer_seek(writer, TEST_RANGE_LENGTH + 1));
    assert_uint_equal(dmi_writer_tell(writer), TEST_RANGE_LENGTH);
}

static void test_writer_tell(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_writer_t *writer = test_writer_open(state, 0, TEST_RANGE_FREE);

    assert_uint_equal(dmi_writer_tell(writer), 0);
    assert_true(dmi_writer_put_bytes(writer, test_data, 3));
    assert_uint_equal(dmi_writer_tell(writer), 3);

    assert_uint_equal(dmi_writer_tell(nullptr), 0);
}

//
// Data written already is kept until it is written over, the way the data of
// a buffer is kept until it is.
//
static void test_writer_reset(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t value[] = { 0xAA, 0xBB };

    dmi_writer_t writer;
    assert_true(dmi_writer_initialize(&writer, state->buffer, 0, TEST_RANGE_FREE));

    assert_true(dmi_writer_put_bytes(&writer, test_data, TEST_DATA_LENGTH));

    dmi_writer_reset(&writer);
    assert_uint_equal(dmi_writer_tell(&writer), 0);
    assert_uint_equal(state->buffer->length, TEST_DATA_LENGTH);

    // Writing again from the beginning replaces only what it covers
    assert_true(dmi_writer_put_bytes(&writer, value, sizeof(value)));
    assert_uint_equal(state->buffer->length, TEST_DATA_LENGTH);
    assert_memory_equal(state->buffer->data, value, sizeof(value));
    assert_memory_equal(state->buffer->data + sizeof(value),
                        test_data + sizeof(value),
                        TEST_DATA_LENGTH - sizeof(value));
}
