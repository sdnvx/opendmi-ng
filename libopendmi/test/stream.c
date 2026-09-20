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

#include <opendmi/stream.h>

typedef struct test_state    test_state_t;
typedef struct test_envelope test_envelope_t;
typedef struct test_entity   test_entity_t;

struct test_state
{
    dmi_context_t *context;
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

static int  test_stream_setup(void **pstate);
static int  test_stream_teardown(void **pstate);

static void test_stream_initialize_null_stream(void **pstate);
static void test_stream_initialize_null_entity(void **pstate);
static void test_stream_initialize(void **pstate);

static void test_stream_seek(void **pstate);
static void test_stream_seek_out_of_bounds(void **pstate);

static void test_stream_read_data(void **pstate);
static void test_stream_read_data_does_not_advance_on_failure(void **pstate);
static void test_stream_read_data_insufficient(void **pstate);

static void test_stream_read_data_at(void **pstate);
static void test_stream_read_data_at_does_not_advance(void **pstate);
static void test_stream_read_data_at_out_of_bounds(void **pstate);

static void test_stream_skip(void **pstate);
static void test_stream_skip_to_end(void **pstate);
static void test_stream_skip_out_of_bounds(void **pstate);

static void test_stream_decode_bin(void **pstate);
static void test_stream_decode_bin_out_of_bounds(void **pstate);
static void test_stream_decode_bin_overlay(void **pstate);

static void test_stream_remaining(void **pstate);
static void test_stream_remaining_null(void **pstate);

static void test_stream_is_done_false(void **pstate);
static void test_stream_is_done_true(void **pstate);
static void test_stream_is_done_null(void **pstate);

static void test_stream_reset(void **pstate);

static void test_stream_has(void **pstate);

static void test_stream_read_macro(void **pstate);
static void test_stream_decode_macro(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };
static test_envelope_t test_envelope = {};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_stream_initialize_null_stream),
        cmocka_unit_test(test_stream_initialize_null_entity),
        cmocka_unit_test(test_stream_initialize),

        cmocka_unit_test(test_stream_seek),
        cmocka_unit_test(test_stream_seek_out_of_bounds),

        cmocka_unit_test(test_stream_read_data),
        cmocka_unit_test(test_stream_read_data_does_not_advance_on_failure),
        cmocka_unit_test(test_stream_read_data_insufficient),

        cmocka_unit_test(test_stream_read_data_at),
        cmocka_unit_test(test_stream_read_data_at_does_not_advance),
        cmocka_unit_test(test_stream_read_data_at_out_of_bounds),

        cmocka_unit_test(test_stream_skip),
        cmocka_unit_test(test_stream_skip_to_end),
        cmocka_unit_test(test_stream_skip_out_of_bounds),

        cmocka_unit_test(test_stream_decode_bin),
        cmocka_unit_test(test_stream_decode_bin_out_of_bounds),
        cmocka_unit_test(test_stream_decode_bin_overlay),

        cmocka_unit_test(test_stream_remaining),
        cmocka_unit_test(test_stream_remaining_null),

        cmocka_unit_test(test_stream_is_done_false),
        cmocka_unit_test(test_stream_is_done_true),
        cmocka_unit_test(test_stream_is_done_null),

        cmocka_unit_test(test_stream_reset),

        cmocka_unit_test(test_stream_has),

        cmocka_unit_test(test_stream_read_macro),
        cmocka_unit_test(test_stream_decode_macro),
    };

    return cmocka_run_group_tests(tests, test_stream_setup, test_stream_teardown);
}

static int test_stream_setup(void **pstate)
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

    state->entity = dmi_entity_create(state->context, &test_envelope, sizeof(test_envelope));
    if (state->entity == nullptr) {
        dmi_destroy(state->context);
        free(state);
        return -1;
    }

    *pstate = state;
    return 0;
}

static int test_stream_teardown(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_entity_destroy(state->entity);
    dmi_destroy(state->context);
    free(state);

    *pstate = nullptr;
    return 0;
}

static void test_stream_initialize_null_stream(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);
    assert_false(dmi_stream_initialize(nullptr, state->entity));
}

static void test_stream_initialize_null_entity(void **pstate)
{
    dmi_unused(pstate);

    dmi_stream_t stream;
    assert_false(dmi_stream_initialize(&stream, nullptr));
}

static void test_stream_initialize(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_stream_t stream;
    assert_true(dmi_stream_initialize(&stream, state->entity));

    assert_ptr_equal(stream.entity, state->entity);
    assert_uint_equal(stream.position, 0);
    assert_uint_equal(stream.remaining, TEST_ENTITY_LENGTH);
}

static void test_stream_seek(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    assert_true(dmi_stream_seek(&stream, 0));
    assert_uint_equal(stream.position, 0);

    // Seek to the start of the body (past header)
    assert_true(dmi_stream_seek(&stream, TEST_BODY_OFFSET));
    assert_uint_equal(stream.position, TEST_BODY_OFFSET);
    assert_uint_equal(stream.remaining, TEST_BODY_SIZE);

    // Last valid position
    assert_true(dmi_stream_seek(&stream, TEST_ENTITY_LENGTH - 1));
    assert_uint_equal(stream.position, TEST_ENTITY_LENGTH - 1);
    assert_uint_equal(stream.remaining, 1);
}

static void test_stream_seek_out_of_bounds(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    // Position equal to body_length is beyond the last valid position
    assert_false(dmi_stream_seek(&stream, TEST_ENTITY_LENGTH));
    assert_false(dmi_stream_seek(&stream, TEST_ENTITY_LENGTH + 1));

    // Failed seek must leave the cursor unchanged
    assert_uint_equal(stream.position, 0);
}

static void test_stream_read_data(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    dmi_stream_seek(&stream, TEST_BODY_OFFSET);

    dmi_byte_t buf[4] = {0};
    assert_true(dmi_stream_read_data(&stream, buf, sizeof(buf)));
    assert_memory_equal(buf, test_body, sizeof(buf));
    assert_uint_equal(stream.position, TEST_BODY_OFFSET + 4);
}

static void test_stream_read_data_does_not_advance_on_failure(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    // Position 2 bytes before the end
    dmi_stream_seek(&stream, TEST_ENTITY_LENGTH - 2);

    dmi_byte_t buf[4] = {0};
    assert_false(dmi_stream_read_data(&stream, buf, sizeof(buf)));
    assert_uint_equal(stream.position, TEST_ENTITY_LENGTH - 2);
}

static void test_stream_read_data_insufficient(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    // Seek to the last byte and try to read two
    dmi_stream_seek(&stream, TEST_ENTITY_LENGTH - 1);

    dmi_byte_t buf[2] = {0};
    assert_false(dmi_stream_read_data(&stream, buf, sizeof(buf)));
}

static void test_stream_read_data_at(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    // Read the middle 4 body bytes
    dmi_byte_t buf[4] = {0};
    assert_true(dmi_stream_read_data_at(&stream, buf, TEST_BODY_OFFSET + 2, sizeof(buf)));
    assert_memory_equal(buf, test_body + 2, sizeof(buf));
}

static void test_stream_read_data_at_does_not_advance(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    dmi_byte_t buf[1] = {0};
    dmi_stream_read_data_at(&stream, buf, TEST_BODY_OFFSET, sizeof(buf));

    assert_uint_equal(stream.position, 0);
}

static void test_stream_read_data_at_out_of_bounds(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    dmi_byte_t buf[4] = {0};
    // offset + length > body_length
    assert_false(dmi_stream_read_data_at(&stream, buf, TEST_ENTITY_LENGTH - 2, sizeof(buf)));
}

static void test_stream_skip(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    dmi_stream_seek(&stream, TEST_BODY_OFFSET);

    assert_true(dmi_stream_skip(&stream, 3));
    assert_uint_equal(stream.position, TEST_BODY_OFFSET + 3);
    assert_uint_equal(stream.remaining, TEST_BODY_SIZE - 3);
}

static void test_stream_skip_to_end(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    // Skip can advance the cursor to position == body_length (exhausted)
    assert_true(dmi_stream_skip(&stream, TEST_ENTITY_LENGTH));
    assert_true(dmi_stream_is_done(&stream));
    assert_uint_equal(dmi_stream_remaining(&stream), 0);
}

static void test_stream_skip_out_of_bounds(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    assert_false(dmi_stream_skip(&stream, TEST_ENTITY_LENGTH + 1));
    assert_uint_equal(stream.position, 0);
}

static void test_stream_decode_bin(void **pstate)
{
    dmi_stream_t stream;
    dmi_binary_t value;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    dmi_stream_seek(&stream, TEST_BODY_OFFSET);

    // Data is referenced in place, and the cursor is advanced
    assert_true(dmi_stream_decode_bin(&stream, 3, &value));
    assert_ptr_equal(value.data, state->entity->data + TEST_BODY_OFFSET);
    assert_uint_equal(value.length, 3);
    assert_uint_equal(value.data[2], 0x33);
    assert_uint_equal(stream.position, TEST_BODY_OFFSET + 3);
    assert_uint_equal(stream.remaining, TEST_BODY_SIZE - 3);

    // Empty data has no pointer
    assert_true(dmi_stream_decode_bin(&stream, 0, &value));
    assert_null(value.data);
    assert_uint_equal(value.length, 0);
    assert_uint_equal(stream.position, TEST_BODY_OFFSET + 3);
}

static void test_stream_decode_bin_out_of_bounds(void **pstate)
{
    dmi_stream_t stream;
    dmi_binary_t value = {};

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    assert_false(dmi_stream_decode_bin(&stream, TEST_ENTITY_LENGTH + 1, &value));
    assert_null(value.data);
    assert_uint_equal(stream.position, 0);

    assert_false(dmi_stream_decode_bin(&stream, 1, nullptr));
}

static void test_stream_decode_bin_overlay(void **pstate)
{
    dmi_stream_t stream;
    dmi_binary_t value;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_entity_t *entity = state->entity;

    dmi_data_t overlay[TEST_ENTITY_LENGTH];
    memcpy(overlay, entity->data, sizeof(overlay));
    overlay[TEST_BODY_OFFSET] = 0xAA;

    // Data is referenced in the copy of structure body with additional
    // information applied
    entity->overlay_data = overlay;
    dmi_stream_initialize(&stream, entity);
    entity->overlay_data = nullptr;

    dmi_stream_seek(&stream, TEST_BODY_OFFSET);

    assert_true(dmi_stream_decode_bin(&stream, 1, &value));
    assert_ptr_equal(value.data, overlay + TEST_BODY_OFFSET);
    assert_uint_equal(value.data[0], 0xAA);
}

static void test_stream_remaining(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    assert_uint_equal(dmi_stream_remaining(&stream), TEST_ENTITY_LENGTH);

    dmi_stream_seek(&stream, TEST_BODY_OFFSET);
    assert_uint_equal(dmi_stream_remaining(&stream), TEST_BODY_SIZE);

    dmi_stream_seek(&stream, TEST_ENTITY_LENGTH - 1);
    assert_uint_equal(dmi_stream_remaining(&stream), 1);
}

static void test_stream_remaining_null(void **pstate)
{
    dmi_unused(pstate);
    assert_uint_equal(dmi_stream_remaining(nullptr), 0);
}

static void test_stream_is_done_false(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    assert_false(dmi_stream_is_done(&stream));

    dmi_stream_seek(&stream, TEST_ENTITY_LENGTH - 1);
    assert_false(dmi_stream_is_done(&stream));
}

static void test_stream_is_done_true(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    dmi_stream_seek(&stream, TEST_ENTITY_LENGTH - 1);
    dmi_stream_skip(&stream, 1);
    assert_true(dmi_stream_is_done(&stream));
}

static void test_stream_is_done_null(void **pstate)
{
    dmi_unused(pstate);
    assert_true(dmi_stream_is_done(nullptr));
}

static void test_stream_reset(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    dmi_stream_seek(&stream, TEST_BODY_OFFSET + 4);
    assert_uint_equal(stream.position, TEST_BODY_OFFSET + 4);

    dmi_stream_reset(&stream);
    assert_uint_equal(stream.position, 0);
    assert_uint_equal(stream.remaining, TEST_ENTITY_LENGTH);
}

static void test_stream_has(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);
    dmi_stream_seek(&stream, TEST_BODY_OFFSET);

    assert_true(dmi_stream_has(&stream, 0));
    assert_true(dmi_stream_has(&stream, 1));
    assert_true(dmi_stream_has(&stream, TEST_BODY_SIZE));
    assert_false(dmi_stream_has(&stream, TEST_BODY_SIZE + 1));

    // Stream position is not modified
    assert_uint_equal(stream.position, TEST_BODY_OFFSET);

    assert_true(dmi_stream_skip(&stream, TEST_BODY_SIZE));
    assert_true(dmi_stream_has(&stream, 0));
    assert_false(dmi_stream_has(&stream, 1));

    assert_false(dmi_stream_has(nullptr, 0));
}

static void test_stream_read_macro(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    dmi_stream_seek(&stream, TEST_BODY_OFFSET);

    dmi_byte_t val = 0;
    assert_true(dmi_stream_read(&stream, &val));
    assert_uint_equal(val, test_body[0]);
    assert_uint_equal(stream.position, TEST_BODY_OFFSET + sizeof(dmi_byte_t));
}

static void test_stream_decode_macro(void **pstate)
{
    dmi_stream_t stream;

    test_state_t *state = dmi_cast(state, *pstate);
    dmi_stream_initialize(&stream, state->entity);

    dmi_stream_seek(&stream, TEST_BODY_OFFSET);

    // dmi_byte_t is single-byte, so dmi_decode is a no-op
    dmi_byte_t val = 0;
    assert_true(dmi_stream_decode(&stream, dmi_byte_t, &val));
    assert_uint_equal(val, test_body[0]);
    assert_uint_equal(stream.position, TEST_BODY_OFFSET + sizeof(dmi_byte_t));
}
