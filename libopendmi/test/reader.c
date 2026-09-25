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
#include <opendmi/entity.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/test/logger.h>

#include <opendmi/reader.h>

typedef struct test_state    test_state_t;
typedef struct test_envelope test_envelope_t;
typedef struct test_entity   test_entity_t;

struct test_state
{
    dmi_context_t *context;
    dmi_buffer_t  *buffer;
    dmi_entity_t  *entity;
};

#define TEST_BODY_OFFSET   sizeof(dmi_header_t)
#define TEST_BODY_SIZE     8
#define TEST_ENTITY_LENGTH (TEST_BODY_OFFSET + TEST_BODY_SIZE)

static const dmi_byte_t test_body[TEST_BODY_SIZE] = {
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
};

dmi_packed_struct(test_entity)
{
    dmi_header_t header;
    dmi_byte_t   body[TEST_BODY_SIZE];
};

dmi_packed_struct(test_envelope) {
        test_entity_t data;
        dmi_word_t    terminator;
};

static int  test_reader_setup(void **pstate);
static int  test_reader_teardown(void **pstate);

static void test_reader_initialize_null_stream(void **pstate);
static void test_reader_initialize_null_buffer(void **pstate);
static void test_reader_initialize_past_data(void **pstate);
static void test_reader_initialize(void **pstate);

static void test_reader_seek(void **pstate);
static void test_reader_seek_out_of_bounds(void **pstate);

static void test_reader_read_data(void **pstate);
static void test_reader_read_data_does_not_advance_on_failure(void **pstate);
static void test_reader_read_data_insufficient(void **pstate);

static void test_reader_read_data_at(void **pstate);
static void test_reader_read_data_at_does_not_advance(void **pstate);
static void test_reader_read_data_at_out_of_bounds(void **pstate);

static void test_reader_skip(void **pstate);
static void test_reader_skip_to_end(void **pstate);
static void test_reader_skip_out_of_bounds(void **pstate);

static void test_reader_mark_rewind(void **pstate);
static void test_reader_mark_null(void **pstate);
static void test_reader_mark_foreign(void **pstate);
static void test_reader_skip_ex(void **pstate);
static void test_reader_skip_ex_to_end(void **pstate);
static void test_reader_skip_ex_out_of_bounds(void **pstate);

static void test_reader_ref_bytes(void **pstate);
static void test_reader_ref_bytes_out_of_bounds(void **pstate);

static void test_reader_remaining(void **pstate);
static void test_reader_remaining_null(void **pstate);

static void test_reader_is_done_false(void **pstate);
static void test_reader_is_done_true(void **pstate);
static void test_reader_is_done_null(void **pstate);

static void test_reader_reset(void **pstate);

static void test_reader_has(void **pstate);


static dmi_log_t test_logger = { dmi_test_log_handler };
static test_envelope_t test_envelope = {};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_reader_initialize_null_stream),
        cmocka_unit_test(test_reader_initialize_null_buffer),
        cmocka_unit_test(test_reader_initialize_past_data),
        cmocka_unit_test(test_reader_initialize),

        cmocka_unit_test(test_reader_seek),
        cmocka_unit_test(test_reader_seek_out_of_bounds),

        cmocka_unit_test(test_reader_read_data),
        cmocka_unit_test(test_reader_read_data_does_not_advance_on_failure),
        cmocka_unit_test(test_reader_read_data_insufficient),

        cmocka_unit_test(test_reader_read_data_at),
        cmocka_unit_test(test_reader_read_data_at_does_not_advance),
        cmocka_unit_test(test_reader_read_data_at_out_of_bounds),

        cmocka_unit_test(test_reader_skip),
        cmocka_unit_test(test_reader_skip_to_end),
        cmocka_unit_test(test_reader_skip_out_of_bounds),

        cmocka_unit_test(test_reader_mark_rewind),
        cmocka_unit_test(test_reader_mark_null),
        cmocka_unit_test(test_reader_mark_foreign),
        cmocka_unit_test(test_reader_skip_ex),
        cmocka_unit_test(test_reader_skip_ex_to_end),
        cmocka_unit_test(test_reader_skip_ex_out_of_bounds),

        cmocka_unit_test(test_reader_ref_bytes),
        cmocka_unit_test(test_reader_ref_bytes_out_of_bounds),

        cmocka_unit_test(test_reader_remaining),
        cmocka_unit_test(test_reader_remaining_null),

        cmocka_unit_test(test_reader_is_done_false),
        cmocka_unit_test(test_reader_is_done_true),
        cmocka_unit_test(test_reader_is_done_null),

        cmocka_unit_test(test_reader_reset),

        cmocka_unit_test(test_reader_has),

    };

    return cmocka_run_group_tests(tests, test_reader_setup, test_reader_teardown);
}

static int test_reader_setup(void **pstate)
{
    test_state_t *state = calloc(1, sizeof(*state));
    if (state == nullptr)
        return -1;

    state->context = dmi_create(DMI_CONTEXT_FLAG_RELAXED);
    if (state->context == nullptr) {
        free(state);
        return -1;
    }

    dmi_set_logger(state->context, &test_logger);

    test_envelope.data.header = (dmi_header_t){
        .type   = dmi_encode((dmi_byte_t)DMI_TYPE_INACTIVE),
        .length = sizeof(test_entity_t),
        .handle = dmi_encode(DMI_HANDLE_TEST)
    };
    memcpy(test_envelope.data.body, test_body, TEST_BODY_SIZE);

    state->buffer = dmi_buffer_create(state->context);

    if ((state->buffer == nullptr) or
        not dmi_buffer_assign(state->buffer, &test_envelope, sizeof(test_envelope)))
    {
        dmi_destroy(state->context);
        free(state);
        return -1;
    }

    state->entity = dmi_entity_create(state->context, state->buffer, 0);
    if (state->entity == nullptr) {
        dmi_buffer_destroy(state->buffer);
        dmi_destroy(state->context);
        free(state);
        return -1;
    }

    *pstate = state;
    return 0;
}

static int test_reader_teardown(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_entity_destroy(state->entity);
    dmi_buffer_destroy(state->buffer);
    dmi_destroy(state->context);
    free(state);

    *pstate = nullptr;
    return 0;
}

static void test_reader_initialize_null_stream(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);
    assert_false(dmi_reader_initialize(nullptr, state->buffer, 0, TEST_ENTITY_LENGTH));
}

static void test_reader_initialize_null_buffer(void **pstate)
{
    dmi_unused(pstate);

    dmi_reader_t reader;
    assert_false(dmi_reader_initialize(&reader, nullptr, 0, 0));
}

//
// Range of a reader is required to be there in the first place
//
static void test_reader_initialize_past_data(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_reader_t reader;
    assert_false(dmi_reader_initialize(&reader, state->buffer, 0, state->buffer->length + 1));
}

static void test_reader_initialize(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_reader_t reader;
    assert_true(dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH));

    assert_ptr_equal(reader.buffer, state->buffer);
    assert_uint_equal(reader.position, 0);
    assert_uint_equal(dmi_reader_remaining(&reader), TEST_ENTITY_LENGTH);
}

static void test_reader_seek(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    assert_true(dmi_reader_seek(&reader, 0));
    assert_uint_equal(reader.position, 0);

    // Seek to the start of the body (past header)
    assert_true(dmi_reader_seek(&reader, TEST_BODY_OFFSET));
    assert_uint_equal(reader.position, TEST_BODY_OFFSET);
    assert_uint_equal(dmi_reader_remaining(&reader), TEST_BODY_SIZE);

    // Last byte of the range
    assert_true(dmi_reader_seek(&reader, TEST_ENTITY_LENGTH - 1));
    assert_uint_equal(reader.position, TEST_ENTITY_LENGTH - 1);
    assert_uint_equal(dmi_reader_remaining(&reader), 1);

    // End of the range, where the cursor stands once everything is read
    assert_true(dmi_reader_seek(&reader, TEST_ENTITY_LENGTH));
    assert_true(dmi_reader_is_done(&reader));
    assert_uint_equal(dmi_reader_remaining(&reader), 0);
}

static void test_reader_seek_out_of_bounds(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    // Position past the end of the range is not one to read from
    assert_false(dmi_reader_seek(&reader, TEST_ENTITY_LENGTH + 1));
    assert_false(dmi_reader_seek(&reader, SIZE_MAX));

    // Failed seek must leave the cursor unchanged
    assert_uint_equal(reader.position, 0);
}

static void test_reader_read_data(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    dmi_reader_seek(&reader, TEST_BODY_OFFSET);

    dmi_byte_t buf[4] = {0};
    assert_true(dmi_reader_get_bytes(&reader, buf, sizeof(buf)));
    assert_memory_equal(buf, test_body, sizeof(buf));
    assert_uint_equal(reader.position, TEST_BODY_OFFSET + 4);
}

static void test_reader_read_data_does_not_advance_on_failure(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    // Position 2 bytes before the end
    dmi_reader_seek(&reader, TEST_ENTITY_LENGTH - 2);

    dmi_byte_t buf[4] = {0};
    assert_false(dmi_reader_get_bytes(&reader, buf, sizeof(buf)));
    assert_uint_equal(reader.position, TEST_ENTITY_LENGTH - 2);
}

static void test_reader_read_data_insufficient(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    // Seek to the last byte and try to read two
    dmi_reader_seek(&reader, TEST_ENTITY_LENGTH - 1);

    dmi_byte_t buf[2] = {0};
    assert_false(dmi_reader_get_bytes(&reader, buf, sizeof(buf)));
}

static void test_reader_read_data_at(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    // Read the middle 4 body bytes
    dmi_byte_t buf[4] = {0};
    assert_true(dmi_reader_get_bytes_at(&reader, buf, TEST_BODY_OFFSET + 2, sizeof(buf)));
    assert_memory_equal(buf, test_body + 2, sizeof(buf));
}

static void test_reader_read_data_at_does_not_advance(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    dmi_byte_t buf[1] = {0};
    dmi_reader_get_bytes_at(&reader, buf, TEST_BODY_OFFSET, sizeof(buf));

    assert_uint_equal(reader.position, 0);
}

static void test_reader_read_data_at_out_of_bounds(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    dmi_byte_t buf[4] = {0};
    // offset + length > body_length
    assert_false(dmi_reader_get_bytes_at(&reader, buf, TEST_ENTITY_LENGTH - 2, sizeof(buf)));
}

static void test_reader_skip(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    dmi_reader_seek(&reader, TEST_BODY_OFFSET);

    assert_true(dmi_reader_skip(&reader, 3));
    assert_uint_equal(reader.position, TEST_BODY_OFFSET + 3);
    assert_uint_equal(dmi_reader_remaining(&reader), TEST_BODY_SIZE - 3);
}

static void test_reader_skip_to_end(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    // Skip can advance the cursor to position == body_length (exhausted)
    assert_true(dmi_reader_skip(&reader, TEST_ENTITY_LENGTH));
    assert_true(dmi_reader_is_done(&reader));
    assert_uint_equal(dmi_reader_remaining(&reader), 0);
}

static void test_reader_skip_out_of_bounds(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    assert_false(dmi_reader_skip(&reader, TEST_ENTITY_LENGTH + 1));
    assert_uint_equal(reader.position, 0);
}

static void test_reader_ref_bytes(void **pstate)
{
    dmi_reader_t      reader;
    const dmi_data_t *ptr = nullptr;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    dmi_reader_seek(&reader, TEST_BODY_OFFSET);

    // Bytes are referred to in place, and the cursor is advanced
    assert_true(dmi_reader_ref_bytes(&reader, 3, &ptr));
    assert_ptr_equal(ptr, state->buffer->data + TEST_BODY_OFFSET);
    assert_uint_equal(ptr[2], 0x33);
    assert_uint_equal(reader.position, TEST_BODY_OFFSET + 3);
    assert_uint_equal(dmi_reader_remaining(&reader), TEST_BODY_SIZE - 3);

    // Nothing to refer to has no pointer
    assert_true(dmi_reader_ref_bytes(&reader, 0, &ptr));
    assert_null(ptr);
    assert_uint_equal(reader.position, TEST_BODY_OFFSET + 3);
}

static void test_reader_ref_bytes_out_of_bounds(void **pstate)
{
    dmi_reader_t      reader;
    const dmi_data_t *ptr = nullptr;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    assert_false(dmi_reader_ref_bytes(&reader, TEST_ENTITY_LENGTH + 1, &ptr));
    assert_null(ptr);
    assert_uint_equal(reader.position, 0);

    assert_false(dmi_reader_ref_bytes(&reader, 1, nullptr));
}


static void test_reader_remaining(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    assert_uint_equal(dmi_reader_remaining(&reader), TEST_ENTITY_LENGTH);

    dmi_reader_seek(&reader, TEST_BODY_OFFSET);
    assert_uint_equal(dmi_reader_remaining(&reader), TEST_BODY_SIZE);

    dmi_reader_seek(&reader, TEST_ENTITY_LENGTH - 1);
    assert_uint_equal(dmi_reader_remaining(&reader), 1);
}

static void test_reader_remaining_null(void **pstate)
{
    dmi_unused(pstate);
    assert_uint_equal(dmi_reader_remaining(nullptr), 0);
}

static void test_reader_is_done_false(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    assert_false(dmi_reader_is_done(&reader));

    dmi_reader_seek(&reader, TEST_ENTITY_LENGTH - 1);
    assert_false(dmi_reader_is_done(&reader));
}

static void test_reader_is_done_true(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    dmi_reader_seek(&reader, TEST_ENTITY_LENGTH - 1);
    dmi_reader_skip(&reader, 1);
    assert_true(dmi_reader_is_done(&reader));
}

static void test_reader_is_done_null(void **pstate)
{
    dmi_unused(pstate);
    assert_true(dmi_reader_is_done(nullptr));
}

static void test_reader_reset(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    dmi_reader_seek(&reader, TEST_BODY_OFFSET + 4);
    assert_uint_equal(reader.position, TEST_BODY_OFFSET + 4);

    dmi_reader_reset(&reader);
    assert_uint_equal(reader.position, 0);
    assert_uint_equal(dmi_reader_remaining(&reader), TEST_ENTITY_LENGTH);
}

static void test_reader_has(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);
    dmi_reader_seek(&reader, TEST_BODY_OFFSET);

    assert_true(dmi_reader_has(&reader, 0));
    assert_true(dmi_reader_has(&reader, 1));
    assert_true(dmi_reader_has(&reader, TEST_BODY_SIZE));
    assert_false(dmi_reader_has(&reader, TEST_BODY_SIZE + 1));

    // Reader position is not modified
    assert_uint_equal(reader.position, TEST_BODY_OFFSET);

    assert_true(dmi_reader_skip(&reader, TEST_BODY_SIZE));
    assert_true(dmi_reader_has(&reader, 0));
    assert_false(dmi_reader_has(&reader, 1));

    assert_false(dmi_reader_has(nullptr, 0));
}


static void test_reader_mark_rewind(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    dmi_reader_seek(&reader, TEST_BODY_OFFSET);

    dmi_reader_mark_t mark = dmi_reader_mark(&reader);

    assert_true(dmi_reader_skip(&reader, 3));
    assert_uint_equal(reader.position, TEST_BODY_OFFSET + 3);

    // Coming back to the mark restores both the position and the size left
    assert_true(dmi_reader_rewind(&reader, mark));
    assert_uint_equal(reader.position, TEST_BODY_OFFSET);
    assert_uint_equal(dmi_reader_remaining(&reader), TEST_BODY_SIZE);

    // Data can be read again from where the mark was taken
    dmi_byte_t val = 0;
    assert_true(dmi_reader_get_bytes(&reader, &val, sizeof(val)));
    assert_uint_equal(val, test_body[0]);
}

static void test_reader_mark_null(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    // Mark of no reader belongs to no entity, so it is rejected rather than
    // taken for the beginning of the data
    dmi_reader_mark_t mark = dmi_reader_mark(nullptr);

    assert_false(dmi_reader_rewind(&reader, mark));
    assert_false(dmi_reader_skip_ex(&reader, mark, 1));
    assert_uint_equal(reader.position, 0);

    assert_false(dmi_reader_rewind(nullptr, dmi_reader_mark(&reader)));
    assert_false(dmi_reader_skip_ex(nullptr, dmi_reader_mark(&reader), 1));
}

static void test_reader_mark_foreign(void **pstate)
{
    dmi_reader_t reader;
    dmi_reader_t other_stream;

    test_state_t *state = dmi_cast(state, *pstate);

    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);
    dmi_reader_initialize(&other_stream, state->buffer, TEST_BODY_OFFSET, TEST_BODY_SIZE);

    dmi_reader_seek(&reader, TEST_BODY_OFFSET);

    // Marks belong to the range they were taken of, even when the other
    // reader reads the same buffer
    dmi_reader_mark_t mark = dmi_reader_mark(&other_stream);

    assert_false(dmi_reader_rewind(&reader, mark));
    assert_false(dmi_reader_skip_ex(&reader, mark, 1));
    assert_uint_equal(reader.position, TEST_BODY_OFFSET);
}

static void test_reader_skip_ex(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    dmi_reader_seek(&reader, TEST_BODY_OFFSET);

    dmi_reader_mark_t mark = dmi_reader_mark(&reader);

    // Record is stepped over by its length, whatever has been read from it
    dmi_byte_t val = 0;
    assert_true(dmi_reader_get_bytes(&reader, &val, sizeof(val)));
    assert_uint_equal(val, test_body[0]);

    assert_true(dmi_reader_skip_ex(&reader, mark, 4));
    assert_uint_equal(reader.position, TEST_BODY_OFFSET + 4);
    assert_uint_equal(dmi_reader_remaining(&reader), TEST_BODY_SIZE - 4);

    // The same mark counts from where it was taken, not from the cursor
    assert_true(dmi_reader_skip_ex(&reader, mark, 6));
    assert_uint_equal(reader.position, TEST_BODY_OFFSET + 6);

    // A record shorter than what has been read of it moves the cursor back
    assert_true(dmi_reader_skip_ex(&reader, mark, 2));
    assert_uint_equal(reader.position, TEST_BODY_OFFSET + 2);
}

static void test_reader_skip_ex_to_end(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    dmi_reader_mark_t mark = dmi_reader_mark(&reader);

    // Mark may be reached at the end of the data, and taken there
    assert_true(dmi_reader_skip_ex(&reader, mark, TEST_ENTITY_LENGTH));
    assert_true(dmi_reader_is_done(&reader));

    dmi_reader_mark_t end = dmi_reader_mark(&reader);

    assert_true(dmi_reader_rewind(&reader, mark));
    assert_true(dmi_reader_rewind(&reader, end));
    assert_uint_equal(dmi_reader_remaining(&reader), 0);
}

static void test_reader_skip_ex_out_of_bounds(void **pstate)
{
    dmi_reader_t reader;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_reader_initialize(&reader, state->buffer, 0, TEST_ENTITY_LENGTH);

    dmi_reader_mark_t mark = dmi_reader_mark(&reader);

    assert_true(dmi_reader_skip(&reader, 2));

    assert_false(dmi_reader_skip_ex(&reader, mark, TEST_ENTITY_LENGTH + 1));

    // Length which would overflow the sum with the position is rejected as
    // well, rather than wrapping around into a valid position
    assert_false(dmi_reader_skip_ex(&reader, mark, SIZE_MAX));

    // Failure leaves the cursor where it was
    assert_uint_equal(reader.position, 2);
}
