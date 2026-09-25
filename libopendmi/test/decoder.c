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
#include <opendmi/error.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/decoder.h>
#include <opendmi/utils/uuid.h>

#include <opendmi/entity/additional-info.h>
#include <opendmi/entity/pointing-device.h>

typedef struct test_state test_state_t;

struct test_state
{
    dmi_context_t *context;
    dmi_buffer_t  *buffer;
    dmi_entity_t  *entity;
    dmi_decoder_t  decoder;
};

// Offsets of the values the structure carries, counted the way the
// specification counts them: from the beginning of the structure
#define TEST_OFFSET_BYTE    0x04
#define TEST_OFFSET_WORD    0x05
#define TEST_OFFSET_DWORD   0x07
#define TEST_OFFSET_QWORD   0x0B
#define TEST_OFFSET_BCD     0x13
#define TEST_OFFSET_BCD_2   0x14
#define TEST_OFFSET_UUID    0x16
#define TEST_OFFSET_STRINGS 0x26

#define TEST_BODY_LENGTH    0x2A
#define TEST_STRINGS_LENGTH 9
#define TEST_TOTAL_LENGTH   (TEST_BODY_LENGTH + TEST_STRINGS_LENGTH)

#define TEST_VALUE_BYTE     0x12u
#define TEST_VALUE_WORD     0x3456u
#define TEST_VALUE_DWORD    0x789ABCDEuL
#define TEST_VALUE_QWORD    0x0123456789ABCDEFuLL

// Inactive structure, which has no fields of its own, carrying one value of
// every kind a decoder reads
static const dmi_byte_t test_data[TEST_TOTAL_LENGTH] = {
    DMI_TYPE_INACTIVE, TEST_BODY_LENGTH, 0x34, 0x12,    // Header
    0x12,                                               // Byte
    0x56, 0x34,                                         // Word
    0xDE, 0xBC, 0x9A, 0x78,                             // Double word
    0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01,     // Quadruple word
    0x42,                                               // Binary-coded 42
    0x34, 0x12,                                         // Binary-coded 1234
    0x33, 0x22, 0x11, 0x00, 0x55, 0x44, 0x77, 0x66,     // UUID
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    0x01, 0x02, 0x00, 0x09,                             // String references

    'O', 'n', 'e', 0x00,
    'T', 'w', 'o', 0x00,
    0x00
};

static int test_decoder_setup(void **pstate);
static int test_decoder_teardown(void **pstate);

static void test_decoder_initialize_null_decoder(void **pstate);
static void test_decoder_initialize_null_entity(void **pstate);
static void test_decoder_initialize(void **pstate);
static void test_decoder_initialize_header_only(void **pstate);

static void test_decoder_entity(void **pstate);
static void test_decoder_context(void **pstate);
static void test_decoder_reader(void **pstate);

static void test_decoder_get(void **pstate);
static void test_decoder_get_widened(void **pstate);
static void test_decoder_get_truncated(void **pstate);

static void test_decoder_get_at(void **pstate);
static void test_decoder_get_at_out_of_bounds(void **pstate);

static void test_decoder_get_bcd(void **pstate);
static void test_decoder_get_uuid(void **pstate);

static void test_decoder_get_string(void **pstate);
static void test_decoder_get_string_missing(void **pstate);
static void test_decoder_get_string_truncated(void **pstate);

static void test_decoder_get_bytes(void **pstate);
static void test_decoder_get_binary(void **pstate);

static void test_decoder_skip(void **pstate);
static void test_decoder_strings_are_out_of_reach(void **pstate);

static void test_decoder_stop(void **pstate);
static void test_decoder_incomplete(void **pstate);
static void test_decoder_incomplete_at_end(void **pstate);

static void test_decoder_overlay(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_decoder_initialize_null_decoder, test_decoder_setup, test_decoder_teardown),
        cmocka_unit_test_setup_teardown(test_decoder_initialize_null_entity, test_decoder_setup, test_decoder_teardown),
        cmocka_unit_test_setup_teardown(test_decoder_initialize, test_decoder_setup, test_decoder_teardown),
        cmocka_unit_test_setup_teardown(test_decoder_initialize_header_only, test_decoder_setup, test_decoder_teardown),

        cmocka_unit_test_setup_teardown(test_decoder_entity, test_decoder_setup, test_decoder_teardown),
        cmocka_unit_test_setup_teardown(test_decoder_context, test_decoder_setup, test_decoder_teardown),
        cmocka_unit_test_setup_teardown(test_decoder_reader, test_decoder_setup, test_decoder_teardown),

        cmocka_unit_test_setup_teardown(test_decoder_get, test_decoder_setup, test_decoder_teardown),
        cmocka_unit_test_setup_teardown(test_decoder_get_widened, test_decoder_setup, test_decoder_teardown),
        cmocka_unit_test_setup_teardown(test_decoder_get_truncated, test_decoder_setup, test_decoder_teardown),

        cmocka_unit_test_setup_teardown(test_decoder_get_at, test_decoder_setup, test_decoder_teardown),
        cmocka_unit_test_setup_teardown(test_decoder_get_at_out_of_bounds, test_decoder_setup, test_decoder_teardown),

        cmocka_unit_test_setup_teardown(test_decoder_get_bcd, test_decoder_setup, test_decoder_teardown),
        cmocka_unit_test_setup_teardown(test_decoder_get_uuid, test_decoder_setup, test_decoder_teardown),

        cmocka_unit_test_setup_teardown(test_decoder_get_string, test_decoder_setup, test_decoder_teardown),
        cmocka_unit_test_setup_teardown(test_decoder_get_string_missing, test_decoder_setup, test_decoder_teardown),
        cmocka_unit_test_setup_teardown(test_decoder_get_string_truncated, test_decoder_setup, test_decoder_teardown),

        cmocka_unit_test_setup_teardown(test_decoder_get_bytes, test_decoder_setup, test_decoder_teardown),
        cmocka_unit_test_setup_teardown(test_decoder_get_binary, test_decoder_setup, test_decoder_teardown),

        cmocka_unit_test_setup_teardown(test_decoder_skip, test_decoder_setup, test_decoder_teardown),
        cmocka_unit_test_setup_teardown(test_decoder_strings_are_out_of_reach, test_decoder_setup, test_decoder_teardown),

        cmocka_unit_test_setup_teardown(test_decoder_stop, test_decoder_setup, test_decoder_teardown),
        cmocka_unit_test_setup_teardown(test_decoder_incomplete, test_decoder_setup, test_decoder_teardown),
        cmocka_unit_test_setup_teardown(test_decoder_incomplete_at_end, test_decoder_setup, test_decoder_teardown),

        cmocka_unit_test_setup_teardown(test_decoder_overlay, test_decoder_setup, test_decoder_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_decoder_setup(void **pstate)
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

    state->buffer = dmi_buffer_create(state->context);
    if (state->buffer == nullptr) {
        dmi_destroy(state->context);
        free(state);
        return -1;
    }

    state->entity = dmi_test_entity_create(state->buffer, test_data, sizeof(test_data));
    if (state->entity == nullptr) {
        dmi_buffer_destroy(state->buffer);
        dmi_destroy(state->context);
        free(state);
        return -1;
    }

    *pstate = state;
    return 0;
}

static int test_decoder_teardown(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_entity_destroy(state->entity);
    dmi_buffer_destroy(state->buffer);
    dmi_destroy(state->context);
    free(state);

    *pstate = nullptr;
    return 0;
}

// Set a decoder up over the structure of the fixture, and position it at the
// value the test reads
static dmi_decoder_t *test_decoder_open(test_state_t *state, size_t offset)
{
    assert_true(dmi_decoder_initialize(&state->decoder, state->entity));

    if (offset > sizeof(dmi_header_t))
        assert_true(dmi_decoder_skip(&state->decoder, offset - sizeof(dmi_header_t)));

    return &state->decoder;
}

static void test_decoder_initialize_null_decoder(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    assert_false(dmi_decoder_initialize(nullptr, state->entity));

    const dmi_error_t *error = dmi_error_get_last(state->context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_NULL_ARGUMENT);
}

static void test_decoder_initialize_null_entity(void **pstate)
{
    dmi_unused(pstate);

    dmi_decoder_t decoder;
    assert_false(dmi_decoder_initialize(&decoder, nullptr));
}

//
// Decoder reads the body of the structure, starting past the header, which
// has been read before the structure was decoded at all.
//
static void test_decoder_initialize(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = &state->decoder;
    assert_true(dmi_decoder_initialize(decoder, state->entity));

    const dmi_reader_t *reader = dmi_decoder_reader(decoder);

    assert_ptr_equal(reader->buffer, dmi_entity_buffer(state->entity));
    assert_uint_equal(reader->base, dmi_entity_offset(state->entity));
    assert_uint_equal(reader->length, TEST_BODY_LENGTH);

    assert_uint_equal(dmi_reader_tell(reader), sizeof(dmi_header_t));
    assert_uint_equal(dmi_decoder_remaining(decoder), TEST_BODY_LENGTH - sizeof(dmi_header_t));
}

//
// Structure of nothing but its header has a body of no bytes, which the
// decoder stands at the end of from the start.
//
static void test_decoder_initialize_header_only(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    static const dmi_byte_t data[] = {
        DMI_TYPE_INACTIVE, sizeof(dmi_header_t), 0x34, 0x12,
        0x00, 0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(state->context);
    assert_non_null(buffer);

    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_uint_equal(entity->body_length, sizeof(dmi_header_t));

    dmi_decoder_t decoder;
    assert_true(dmi_decoder_initialize(&decoder, entity));

    assert_uint_equal(dmi_reader_tell(dmi_decoder_reader(&decoder)), sizeof(dmi_header_t));
    assert_uint_equal(dmi_decoder_remaining(&decoder), 0);

    dmi_byte_t value = 0;
    assert_false(dmi_decoder_get_bytes(&decoder, &value, sizeof(value)));

    // Header is still within reach of the reads by offset
    assert_true(dmi_decoder_get_bytes_at(&decoder, &value, 0, sizeof(value)));
    assert_int_equal(value, DMI_TYPE_INACTIVE);

    // Data is exhausted, so the structure is partial rather than incomplete
    assert_true(dmi_decoder_stop(&decoder));
    assert_true(entity->state & DMI_ENTITY_STATE_PARTIAL);

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
}

static void test_decoder_entity(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    const dmi_decoder_t *decoder = test_decoder_open(state, TEST_OFFSET_BYTE);

    assert_ptr_equal(dmi_decoder_entity(decoder), state->entity);
    assert_null(dmi_decoder_entity(nullptr));
}

static void test_decoder_context(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    const dmi_decoder_t *decoder = test_decoder_open(state, TEST_OFFSET_BYTE);

    assert_ptr_equal(dmi_decoder_context(decoder), state->context);
    assert_null(dmi_decoder_context(nullptr));
}

static void test_decoder_reader(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = test_decoder_open(state, TEST_OFFSET_BYTE);

    // Reader is the one the decoder reads through, so moving it moves the
    // decoder as well
    dmi_reader_t *reader = dmi_decoder_reader(decoder);
    assert_non_null(reader);

    assert_true(dmi_reader_skip(reader, 1));
    assert_uint_equal(dmi_decoder_remaining(decoder), TEST_BODY_LENGTH - TEST_OFFSET_BYTE - 1);

    assert_null(dmi_decoder_reader(nullptr));
}

//
// Values are carried little-endian, and read into the host byte order.
//
static void test_decoder_get(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = test_decoder_open(state, TEST_OFFSET_BYTE);

    uint8_t  value_byte  = 0;
    uint16_t value_word  = 0;
    uint32_t value_dword = 0;
    uint64_t value_qword = 0;

    assert_true(dmi_decoder_get(decoder, dmi_byte_t, &value_byte));
    assert_int_equal(value_byte, TEST_VALUE_BYTE);

    assert_true(dmi_decoder_get(decoder, dmi_word_t, &value_word));
    assert_int_equal(value_word, TEST_VALUE_WORD);

    assert_true(dmi_decoder_get(decoder, dmi_dword_t, &value_dword));
    assert_int_equal(value_dword, TEST_VALUE_DWORD);

    assert_true(dmi_decoder_get(decoder, dmi_qword_t, &value_qword));
    assert_int_equal(value_qword, TEST_VALUE_QWORD);

    // Every read advances the cursor by the width of the value
    assert_uint_equal(dmi_reader_tell(dmi_decoder_reader(decoder)), TEST_OFFSET_BCD);
}

//
// Member a value is read into may be wider than the field carrying it, so
// that the value standing for "unknown" does not collide with a real one.
//
static void test_decoder_get_widened(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = test_decoder_open(state, TEST_OFFSET_WORD);

    uint64_t value = 0;

    assert_true(dmi_decoder_get(decoder, dmi_word_t, &value));
    assert_int_equal(value, TEST_VALUE_WORD);
}

static void test_decoder_get_truncated(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    // Two bytes of the string references are left
    dmi_decoder_t *decoder = test_decoder_open(state, TEST_BODY_LENGTH - 2);

    uint32_t value = 0;

    assert_false(dmi_decoder_get(decoder, dmi_dword_t, &value));
    assert_int_equal(value, 0);

    // Failed read leaves the cursor where it was
    assert_uint_equal(dmi_reader_tell(dmi_decoder_reader(decoder)), TEST_BODY_LENGTH - 2);
    assert_uint_equal(dmi_decoder_remaining(decoder), 2);
}

//
// Offsets are counted from the beginning of the structure, the way the
// specification counts them, so the header is within reach of a read.
//
static void test_decoder_get_at(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = test_decoder_open(state, TEST_OFFSET_BYTE);

    uint8_t  type   = 0;
    uint16_t handle = 0;
    uint32_t value  = 0;

    assert_true(dmi_decoder_get_at(decoder, 0x00, dmi_byte_t, &type));
    assert_int_equal(type, DMI_TYPE_INACTIVE);

    assert_true(dmi_decoder_get_at(decoder, 0x02, dmi_word_t, &handle));
    assert_int_equal(handle, DMI_HANDLE_TEST);

    assert_true(dmi_decoder_get_at(decoder, TEST_OFFSET_DWORD, dmi_dword_t, &value));
    assert_int_equal(value, TEST_VALUE_DWORD);

    // Random access does not move the cursor
    assert_uint_equal(dmi_reader_tell(dmi_decoder_reader(decoder)), TEST_OFFSET_BYTE);
}

static void test_decoder_get_at_out_of_bounds(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    const dmi_decoder_t *decoder = test_decoder_open(state, TEST_OFFSET_BYTE);

    uint16_t value = 0;

    assert_false(dmi_decoder_get_at(decoder, TEST_BODY_LENGTH - 1, dmi_word_t, &value));
    assert_false(dmi_decoder_get_at(decoder, TEST_BODY_LENGTH, dmi_byte_t, &value));
    assert_int_equal(value, 0);
}

//
// Binary-coded decimals spell the number out as digits, two per byte, with
// the least significant pair first.
//
static void test_decoder_get_bcd(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = test_decoder_open(state, TEST_OFFSET_BCD);

    unsigned int value = 0;

    assert_true(dmi_decoder_get_bcd(decoder, dmi_byte_t, &value));
    assert_int_equal(value, 42);

    assert_true(dmi_decoder_get_bcd(decoder, dmi_word_t, &value));
    assert_int_equal(value, 1234);

    assert_uint_equal(dmi_reader_tell(dmi_decoder_reader(decoder)), TEST_OFFSET_UUID);

    value = 0;
    assert_true(dmi_decoder_get_bcd_at(decoder, TEST_OFFSET_BCD_2, dmi_word_t, &value));
    assert_int_equal(value, 1234);

    // Random access does not move the cursor
    assert_uint_equal(dmi_reader_tell(dmi_decoder_reader(decoder)), TEST_OFFSET_UUID);
}

//
// UUID is converted from the byte order the specification stores it in, which
// `dmi_uuid_decode`(3) is tested for on its own; what the decoder adds is
// reading the sixteen bytes it takes at the right place.
//
static void test_decoder_get_uuid(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = test_decoder_open(state, TEST_OFFSET_UUID);

    dmi_uuid_t expected = dmi_uuid_decode(test_data + TEST_OFFSET_UUID);
    dmi_uuid_t value    = {};

    assert_true(dmi_decoder_get_uuid(decoder, &value));
    assert_memory_equal(&value, &expected, sizeof(value));
    assert_uint_equal(dmi_reader_tell(dmi_decoder_reader(decoder)), TEST_OFFSET_STRINGS);

    memset(&value, 0, sizeof(value));

    assert_true(dmi_decoder_get_uuid_at(decoder, TEST_OFFSET_UUID, &value));
    assert_memory_equal(&value, &expected, sizeof(value));

    // Random access does not move the cursor
    assert_uint_equal(dmi_reader_tell(dmi_decoder_reader(decoder)), TEST_OFFSET_STRINGS);
}

static void test_decoder_get_string(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = test_decoder_open(state, TEST_OFFSET_STRINGS);

    const char *value = nullptr;

    assert_true(dmi_decoder_get_string(decoder, &value));
    assert_string_equal(value, "One");

    assert_true(dmi_decoder_get_string(decoder, &value));
    assert_string_equal(value, "Two");

    // Reference to no string is not an error of the data
    assert_true(dmi_decoder_get_string(decoder, &value));
    assert_null(value);

    assert_null(dmi_error_peek_last(state->context));
}

//
// Reference to a string the structure does not carry is an error of the data,
// which leaves the value unset rather than stopping the decoding.
//
static void test_decoder_get_string_missing(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = test_decoder_open(state, TEST_BODY_LENGTH - 1);

    const char *value = "kept";

    assert_true(dmi_decoder_get_string(decoder, &value));
    assert_null(value);

    const dmi_error_t *error = dmi_error_get_last(state->context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_STRING_NOT_FOUND);
}

static void test_decoder_get_string_truncated(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = test_decoder_open(state, TEST_BODY_LENGTH);

    const char *value = "kept";

    assert_false(dmi_decoder_get_string(decoder, &value));
    assert_string_equal(value, "kept");
}

static void test_decoder_get_bytes(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = test_decoder_open(state, TEST_OFFSET_UUID);

    dmi_byte_t value[4] = {};

    assert_true(dmi_decoder_get_bytes(decoder, value, sizeof(value)));
    assert_memory_equal(value, test_data + TEST_OFFSET_UUID, sizeof(value));
    assert_uint_equal(dmi_reader_tell(dmi_decoder_reader(decoder)), TEST_OFFSET_UUID + 4);

    memset(value, 0, sizeof(value));

    assert_true(dmi_decoder_get_bytes_at(decoder, value, TEST_OFFSET_DWORD, sizeof(value)));
    assert_memory_equal(value, test_data + TEST_OFFSET_DWORD, sizeof(value));

    // Random access does not move the cursor
    assert_uint_equal(dmi_reader_tell(dmi_decoder_reader(decoder)), TEST_OFFSET_UUID + 4);

    assert_false(dmi_decoder_get_bytes_at(decoder, value, TEST_BODY_LENGTH - 2, sizeof(value)));
}

//
// Binary data refers to the bytes of the structure in place, so it stays
// valid for as long as the buffer holds them.
//
static void test_decoder_get_binary(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = test_decoder_open(state, TEST_OFFSET_UUID);

    dmi_binary_t value = {};

    assert_true(dmi_decoder_get_binary(decoder, 16, &value));
    assert_int_equal(value.length, 16);
    assert_ptr_equal(value.data,
                     dmi_buffer_at(state->buffer, TEST_OFFSET_UUID, 16));
    assert_memory_equal(value.data, test_data + TEST_OFFSET_UUID, 16);
    assert_uint_equal(dmi_reader_tell(dmi_decoder_reader(decoder)), TEST_OFFSET_STRINGS);

    // Nothing to refer to has no pointer, and the cursor stays where it is
    assert_true(dmi_decoder_get_binary(decoder, 0, &value));
    assert_int_equal(value.length, 0);
    assert_null(value.data);
    assert_uint_equal(dmi_reader_tell(dmi_decoder_reader(decoder)), TEST_OFFSET_STRINGS);

    assert_false(dmi_decoder_get_binary(decoder, TEST_STRINGS_LENGTH, &value));
}

static void test_decoder_skip(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = test_decoder_open(state, TEST_OFFSET_BYTE);

    assert_true(dmi_decoder_skip(decoder, TEST_OFFSET_UUID - TEST_OFFSET_BYTE));
    assert_uint_equal(dmi_reader_tell(dmi_decoder_reader(decoder)), TEST_OFFSET_UUID);
    assert_uint_equal(dmi_decoder_remaining(decoder), TEST_BODY_LENGTH - TEST_OFFSET_UUID);

    // Skipping past the end of the structure leaves the cursor where it was
    assert_false(dmi_decoder_skip(decoder, TEST_BODY_LENGTH));
    assert_uint_equal(dmi_reader_tell(dmi_decoder_reader(decoder)), TEST_OFFSET_UUID);
}

//
// Decoder reads the body of the structure and nothing else, so the strings
// after it are out of reach even though the buffer holds them.
//
static void test_decoder_strings_are_out_of_reach(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = test_decoder_open(state, TEST_BODY_LENGTH);

    assert_uint_equal(dmi_decoder_remaining(decoder), 0);
    assert_uint_equal(state->buffer->length, TEST_TOTAL_LENGTH);

    dmi_byte_t value = 0;

    assert_false(dmi_decoder_get_bytes(decoder, &value, sizeof(value)));
    assert_false(dmi_decoder_get_bytes_at(decoder, &value, TEST_BODY_LENGTH, sizeof(value)));
}

//
// Structure which the data ends at is partial: it carries the fields of an
// older version of the specification, and none of the newer ones.
//
static void test_decoder_stop(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = test_decoder_open(state, TEST_BODY_LENGTH);

    assert_true(dmi_decoder_stop(decoder));

    assert_true(state->entity->state & DMI_ENTITY_STATE_PARTIAL);
    assert_false(state->entity->state & DMI_ENTITY_STATE_INCOMPLETE);
}

//
// Structure which the data ends in the middle of a set of fields is
// incomplete, and whatever is left of the set is ignored.
//
static void test_decoder_incomplete(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = test_decoder_open(state, TEST_BODY_LENGTH - 3);

    assert_true(dmi_decoder_incomplete(decoder));

    assert_true(state->entity->state & DMI_ENTITY_STATE_INCOMPLETE);
    assert_false(state->entity->state & DMI_ENTITY_STATE_PARTIAL);

    // Bytes left over are ignored rather than consumed
    assert_uint_equal(dmi_decoder_remaining(decoder), 3);
}

static void test_decoder_incomplete_at_end(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_decoder_t *decoder = test_decoder_open(state, TEST_BODY_LENGTH);

    assert_true(dmi_decoder_incomplete(decoder));
    assert_true(state->entity->state & DMI_ENTITY_STATE_INCOMPLETE);
}

// Additional information carrying one entry, which writes `value` over the
// byte at `offset` of the structure `handle` names
static dmi_entity_t *test_decoder_create_overlay(
        dmi_buffer_t *buffer,
        dmi_handle_t  handle,
        dmi_byte_t    offset,
        dmi_byte_t    value)
{
    const dmi_byte_t data[] = {
        DMI_TYPE_ADDITIONAL_INFO, 0x0B, 0x00, 0x40,     // Header
        0x01,                                           // Entry count
        0x06,                                           // Entry length
        (dmi_byte_t)(handle & 0xFF),                    // Referenced handle
        (dmi_byte_t)(handle >> 8),
        offset,                                         // Referenced offset
        0x00,                                           // String
        value,                                          // Value
        0x00, 0x00                                      // Strings
    };

    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    return entity;
}

//
// Decoder reads the copy of the structure with additional information
// applied, which leaves the data the structure has been read from alone.
//
static void test_decoder_overlay(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    // Pointing device with three buttons, which the entry makes seven
    static const dmi_byte_t data[] = {
        DMI_TYPE_POINTING_DEVICE, 0x07, 0x34, 0x12,
        DMI_POINTING_DEVICE_TYPE_MOUSE,
        DMI_POINTING_DEVICE_IFACE_USB,
        0x03,
        0x00, 0x00
    };

    dmi_buffer_t *source_buffer = dmi_buffer_create(state->context);
    dmi_buffer_t *target_buffer = dmi_buffer_create(state->context);

    assert_non_null(source_buffer);
    assert_non_null(target_buffer);

    dmi_entity_t *source = test_decoder_create_overlay(source_buffer, DMI_HANDLE_TEST, 0x06, 0x07);
    dmi_entity_t *target = dmi_test_entity_create(target_buffer, data, sizeof(data));

    assert_non_null(target);
    assert_true(dmi_entity_add_overlay(target, source, 0));
    assert_true(dmi_entity_decode(target));

    const dmi_pointing_device_t *info = dmi_entity_info(target, DMI_TYPE(POINTING_DEVICE));
    assert_non_null(info);
    assert_uint_equal(info->button_count, 0x07);

    // Structure is read out of the copy, and the data it has been read from
    // keeps what it carried
    assert_ptr_not_equal(dmi_entity_buffer(target), target_buffer);
    assert_uint_equal(dmi_entity_offset(target), 0);
    assert_int_equal(*dmi_buffer_at(target_buffer, 0x06, 1), 0x03);

    dmi_decoder_t decoder;
    assert_true(dmi_decoder_initialize(&decoder, target));
    assert_ptr_equal(dmi_decoder_reader(&decoder)->buffer, dmi_entity_buffer(target));

    dmi_byte_t value = 0;
    assert_true(dmi_decoder_get_bytes_at(&decoder, &value, 0x06, sizeof(value)));
    assert_int_equal(value, 0x07);

    dmi_entity_destroy(target);
    dmi_entity_destroy(source);

    dmi_buffer_destroy(target_buffer);
    dmi_buffer_destroy(source_buffer);
}
