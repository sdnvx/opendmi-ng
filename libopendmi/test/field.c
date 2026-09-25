//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
// The field engine is exercised against specifications of its own rather
// than against the ones of the structures the specification defines, so that
// every kind of field is there, and a change of the engine is told apart
// from a change of a structure type.
//
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/decoder.h>
#include <opendmi/encoder.h>
#include <opendmi/entity.h>
#include <opendmi/error.h>
#include <opendmi/field.h>
#include <opendmi/log.h>
#include <opendmi/module.h>
#include <opendmi/internal.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>
#include <opendmi/utils/uuid.h>

//
// Types of the test structures, which are of the range the specification
// leaves to the vendors and belong to the test module alone
//
#define TEST_TYPE_FIELDS   201
#define TEST_TYPE_ARRAY    202
#define TEST_TYPE_OFFSET   203
#define TEST_TYPE_BARE     204

typedef struct test_state test_state_t;

struct test_state
{
    dmi_context_t *context;
    dmi_buffer_t  *buffer;
    dmi_entity_t  *entity;
};

//
// Structure carrying one field of every kind the engine reads
//

typedef struct test_fields
{
    unsigned int preset;

    uint16_t     value_byte;
    uint32_t     value_word;
    uint64_t     value_dword;
    uint64_t     value_qword;

    const char  *text;
    unsigned int bcd;
    dmi_uuid_t   uuid;

    unsigned int bits_low;
    unsigned int bits_high;

    dmi_binary_t blob;

    uint16_t     maybe;
    unsigned int optional;
    uint32_t     newest;
} test_fields_t;

#define TEST_FIELDS_REQUIRED 0x2C
#define TEST_FIELDS_LENGTH   0x30

static const dmi_entity_spec_t test_fields_spec =
{
    .type = TEST_TYPE_FIELDS,
    .code = "test-fields",
    .name = "Test fields",

    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = TEST_FIELDS_REQUIRED,
        .decoded_length  = sizeof(test_fields_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_PRESET(test_fields_t, preset, 42u),

        DMI_FIELD_OFFSET(0x04),
        DMI_FIELD(test_fields_t, value_byte,  dmi_byte_t),
        DMI_FIELD(test_fields_t, value_word,  dmi_word_t),
        DMI_FIELD(test_fields_t, value_dword, dmi_dword_t),
        DMI_FIELD(test_fields_t, value_qword, dmi_qword_t),

        DMI_FIELD_OFFSET(0x13),
        DMI_FIELD_STRING(test_fields_t, text),
        DMI_FIELD_BCD(test_fields_t, bcd, dmi_byte_t),
        DMI_FIELD_UUID(test_fields_t, uuid),

        // Ranges of bits share one byte, which the padding closes
        DMI_FIELD_OFFSET(0x25),
        DMI_FIELD_BITS(test_fields_t, bits_low, 3),
        DMI_FIELD_BITS_SKIP(2),
        DMI_FIELD_BITS(test_fields_t, bits_high, 3),
        DMI_FIELD_PAD(dmi_byte_t),

        DMI_FIELD_OFFSET(0x26),
        DMI_FIELD_SKIP(2),
        DMI_FIELD_BINARY(test_fields_t, blob, 4),

        DMI_FIELD_OFFSET(TEST_FIELDS_REQUIRED),
        DMI_FIELD_GROUP(.since = DMI_VERSION(3, 0, 0)),
        DMI_FIELD(test_fields_t, maybe, dmi_byte_t, .unknown_raw = 0xFF),

        DMI_FIELD_GROUP(.since = DMI_VERSION(3, 1, 0)),
        DMI_FIELD(test_fields_t, optional, dmi_byte_t,
                  .absent = &(const unsigned int){ 7 }),
        DMI_FIELD(test_fields_t, newest, dmi_word_t),
        {}
    })
};

static const dmi_byte_t test_fields_data[] = {
    TEST_TYPE_FIELDS, TEST_FIELDS_LENGTH, 0x34, 0x12,   // Header
    0x12,                                               // Byte
    0x56, 0x34,                                         // Word
    0xDE, 0xBC, 0x9A, 0x78,                             // Double word
    0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01,     // Quadruple word
    0x01,                                               // Reference to the first string
    0x42,                                               // Binary-coded 42
    0x33, 0x22, 0x11, 0x00, 0x55, 0x44, 0x77, 0x66,     // UUID
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    0xAB,                                               // Ranges of bits
    0x7E, 0x7F,                                         // Bytes nothing reads
    0xDE, 0xAD, 0xBE, 0xEF,                             // Binary data
    0xFF,                                               // Value standing for "unknown"
    0x05,                                               // Value the older data leaves out
    0x34, 0x12,                                         // Value of the newest group

    'A', 'l', 'p', 'h', 'a', 0x00,
    0x00
};

//
// Structure carrying the arrays of both kinds: the one whose number of the
// elements the data declares, and the one running to the end of the data
//

typedef struct test_array_item
{
    unsigned int a;
    uint32_t     b;
} test_array_item_t;

typedef struct test_array
{
    test_array_item_t *items;
    size_t             item_count;
    size_t             declared_count;
    size_t             item_stride;

    uint8_t           *plain;
    size_t             plain_count;
} test_array_t;

static const dmi_entity_spec_t test_array_spec =
{
    .type = TEST_TYPE_ARRAY,
    .code = "test-array",
    .name = "Test arrays",

    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x06,
        .decoded_length  = sizeof(test_array_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_OFFSET(0x04),
        DMI_FIELD_ARRAY(test_array_t, items, item_count,
            .count_length  = sizeof(dmi_byte_t),
            .count_member  = dmi_member(test_array_t, declared_count),
            .stride_length = sizeof(dmi_byte_t),
            .stride_member = dmi_member(test_array_t, item_stride),
            .fields        = DMI_FIELDS({
                DMI_FIELD(test_array_item_t, a, dmi_byte_t),
                DMI_FIELD(test_array_item_t, b, dmi_word_t),
                {}
            })),

        // Array the data carries no number of its own for runs to the end of
        // the structure
        DMI_FIELD_ARRAY(test_array_t, plain, plain_count,
            .stride = sizeof(dmi_byte_t),
            .fields = DMI_FIELDS({
                DMI_FIELD_ELEMENT(test_array_t, plain, dmi_byte_t),
                {}
            })),
        {}
    })
};

// Two elements of four bytes each, whose fields take three, and three bytes
// of the array which runs to the end
static const dmi_byte_t test_array_data[] = {
    TEST_TYPE_ARRAY, 0x11, 0x34, 0x12,
    0x02,                       // Number of the elements
    0x04,                       // Bytes an element takes
    0x11, 0x22, 0x33, 0x7E,     // First element, with a byte nothing reads
    0x44, 0x55, 0x66, 0x7F,     // Second element
    0xAA, 0xBB, 0xCC,           // Array running to the end of the structure

    0x00, 0x00
};

//
// Specification whose fields reach another offset than the one they declare,
// which is a declaration that has lost or gained a field
//

static const dmi_entity_spec_t test_offset_spec =
{
    .type = TEST_TYPE_OFFSET,
    .code = "test-offset",
    .name = "Test offsets",

    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x06,
        .decoded_length  = sizeof(test_fields_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(test_fields_t, value_byte, dmi_byte_t),
        DMI_FIELD_OFFSET(0x06),
        {}
    })
};

static const dmi_byte_t test_offset_data[] = {
    TEST_TYPE_OFFSET, 0x06, 0x34, 0x12,
    0x11, 0x22,

    0x00, 0x00
};

//
// Specification which describes no layout at all, and decodes nothing
//

static const dmi_entity_spec_t test_bare_spec =
{
    .type = TEST_TYPE_BARE,
    .code = "test-bare",
    .name = "Test without fields",

    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x04,
        .decoded_length  = sizeof(test_fields_t)
    }
};

// Structure of nothing but its header, which a decoder reads nothing of
static const dmi_byte_t test_bare_data[] = {
    TEST_TYPE_BARE, 0x04, 0x34, 0x12,

    0x00, 0x00
};

static dmi_module_t test_module =
{
    .code     = "test-fields",
    .name     = "Field engine tests",
    .entities = (const dmi_entity_spec_t *[]){
        &test_fields_spec,
        &test_array_spec,
        &test_offset_spec,
        &test_bare_spec,
        nullptr
    }
};

static int  test_field_setup(void **pstate);
static int  test_field_teardown(void **pstate);

static void test_field_decode_values(void **pstate);
static void test_field_decode_widened(void **pstate);
static void test_field_decode_unknown(void **pstate);
static void test_field_decode_bits(void **pstate);
static void test_field_decode_binary(void **pstate);
static void test_field_decode_preset(void **pstate);
static void test_field_decode_skip(void **pstate);

static void test_field_group_stop(void **pstate);
static void test_field_group_absent(void **pstate);
static void test_field_group_incomplete(void **pstate);
static void test_field_required(void **pstate);

static void test_field_array(void **pstate);
static void test_field_array_truncated(void **pstate);
static void test_field_array_empty(void **pstate);

static void test_field_offset_mismatch(void **pstate);
static void test_field_no_fields(void **pstate);
static void test_field_null_argument(void **pstate);

static void test_field_encode_preserve(void **pstate);
static void test_field_encode_canonical(void **pstate);
static void test_field_encode_truncated(void **pstate);
static void test_field_encode_array(void **pstate);

static void test_field_kilobytes(void **pstate);
static void test_field_get_set(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_field_decode_values, test_field_setup, test_field_teardown),
        cmocka_unit_test_setup_teardown(test_field_decode_widened, test_field_setup, test_field_teardown),
        cmocka_unit_test_setup_teardown(test_field_decode_unknown, test_field_setup, test_field_teardown),
        cmocka_unit_test_setup_teardown(test_field_decode_bits, test_field_setup, test_field_teardown),
        cmocka_unit_test_setup_teardown(test_field_decode_binary, test_field_setup, test_field_teardown),
        cmocka_unit_test_setup_teardown(test_field_decode_preset, test_field_setup, test_field_teardown),
        cmocka_unit_test_setup_teardown(test_field_decode_skip, test_field_setup, test_field_teardown),

        cmocka_unit_test_setup_teardown(test_field_group_stop, test_field_setup, test_field_teardown),
        cmocka_unit_test_setup_teardown(test_field_group_absent, test_field_setup, test_field_teardown),
        cmocka_unit_test_setup_teardown(test_field_group_incomplete, test_field_setup, test_field_teardown),
        cmocka_unit_test_setup_teardown(test_field_required, test_field_setup, test_field_teardown),

        cmocka_unit_test_setup_teardown(test_field_array, test_field_setup, test_field_teardown),
        cmocka_unit_test_setup_teardown(test_field_array_truncated, test_field_setup, test_field_teardown),
        cmocka_unit_test_setup_teardown(test_field_array_empty, test_field_setup, test_field_teardown),

        cmocka_unit_test_setup_teardown(test_field_offset_mismatch, test_field_setup, test_field_teardown),
        cmocka_unit_test_setup_teardown(test_field_no_fields, test_field_setup, test_field_teardown),
        cmocka_unit_test(test_field_null_argument),

        cmocka_unit_test_setup_teardown(test_field_encode_preserve, test_field_setup, test_field_teardown),
        cmocka_unit_test_setup_teardown(test_field_encode_canonical, test_field_setup, test_field_teardown),
        cmocka_unit_test_setup_teardown(test_field_encode_truncated, test_field_setup, test_field_teardown),
        cmocka_unit_test_setup_teardown(test_field_encode_array, test_field_setup, test_field_teardown),

        cmocka_unit_test(test_field_kilobytes),
        cmocka_unit_test(test_field_get_set)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_field_setup(void **pstate)
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

    if (not dmi_add_extension(state->context, &test_module)) {
        dmi_destroy(state->context);
        free(state);
        return -1;
    }

    state->buffer = dmi_buffer_create(state->context);
    if (state->buffer == nullptr) {
        dmi_destroy(state->context);
        free(state);
        return -1;
    }

    *pstate = state;
    return 0;
}

static int test_field_teardown(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_entity_destroy(state->entity);
    dmi_buffer_destroy(state->buffer);
    dmi_destroy(state->context);
    free(state);

    *pstate = nullptr;
    return 0;
}

// Build a structure out of the given bytes, cut to the given length, and
// decode it by the fields of its specification
static const void *test_field_decode(
        test_state_t     *state,
        const dmi_byte_t *data,
        size_t            length,
        size_t            body_length,
        dmi_type_t        type,
        bool              expected)
{
    dmi_byte_t copy[64];

    assert_true(length <= sizeof(copy));
    memcpy(copy, data, length);

    // Structure is cut to the given length, and the strings it carries are
    // moved along with it, so that the references keep pointing at them
    size_t declared = data[1];
    size_t strings  = length - declared;

    if ((body_length != 0) and (body_length != declared)) {
        copy[1] = (dmi_byte_t)body_length;

        memcpy(copy + body_length, data + declared, strings);
        length = body_length + strings;
    }

    state->entity = dmi_test_entity_create(state->buffer, copy, length);
    assert_non_null(state->entity);

    assert_int_equal(dmi_entity_decode(state->entity), expected);

    return expected ? dmi_entity_info(state->entity, type) : nullptr;
}

static const test_fields_t *test_field_decode_fields(test_state_t *state, size_t body_length)
{
    const test_fields_t *info = test_field_decode(
            state, test_fields_data, sizeof(test_fields_data),
            body_length, TEST_TYPE_FIELDS, true);

    assert_non_null(info);

    return info;
}

//
// Values are carried little-endian, and read into the host byte order.
//
static void test_field_decode_values(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    const test_fields_t *info = test_field_decode_fields(state, 0);

    assert_int_equal(info->value_byte,  0x12);
    assert_int_equal(info->value_word,  0x3456);
    assert_int_equal(info->value_dword, 0x789ABCDEuL);
    assert_int_equal(info->value_qword, 0x0123456789ABCDEFuLL);

    assert_string_equal(info->text, "Alpha");
    assert_int_equal(info->bcd, 42);

    dmi_uuid_t uuid = dmi_uuid_decode(test_fields_data + 0x15);
    assert_memory_equal(&info->uuid, &uuid, sizeof(uuid));

    assert_int_equal(info->newest, 0x1234);
    assert_int_equal(state->entity->level, DMI_VERSION(3, 1, 0));
}

//
// Member a field is read into may be wider than the field itself, so that
// the value standing for "unknown" does not collide with a real one.
//
static void test_field_decode_widened(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    const test_fields_t *info = test_field_decode_fields(state, 0);

    // Byte read into a word, and a word read into a double word
    assert_int_equal(sizeof(info->value_byte), sizeof(uint16_t));
    assert_int_equal(info->value_byte, 0x12);
    assert_int_equal(sizeof(info->value_word), sizeof(uint32_t));
    assert_int_equal(info->value_word, 0x3456);
}

//
// Data standing for "unknown" becomes the largest value the member can hold,
// which no device can carry in a field of the width the data has.
//
static void test_field_decode_unknown(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    const test_fields_t *info = test_field_decode_fields(state, 0);

    assert_int_equal(info->maybe, UINT16_MAX);
}

//
// Ranges of bits are taken from the least significant bit of the unit up, in
// the order they are declared.
//
static void test_field_decode_bits(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    const test_fields_t *info = test_field_decode_fields(state, 0);

    // 0xAB is 0b10101011: three bits, two the specification reserves, three
    assert_int_equal(info->bits_low,  0x3);
    assert_int_equal(info->bits_high, 0x5);
}

//
// Binary data refers to the bytes of the structure in place.
//
static void test_field_decode_binary(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    const test_fields_t *info = test_field_decode_fields(state, 0);

    assert_int_equal(info->blob.length, 4);
    assert_memory_equal(info->blob.data, test_fields_data + 0x28, 4);
    assert_ptr_equal(info->blob.data, dmi_buffer_at(state->buffer, 0x28, 4));
}

//
// Members the data carries nothing for are set before anything is read.
//
static void test_field_decode_preset(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    const test_fields_t *info = test_field_decode_fields(state, 0);

    assert_int_equal(info->preset, 42);

    // Preset holds even for a structure which ends before any field is read
    info = test_field_decode_fields(state, TEST_FIELDS_REQUIRED);
    assert_int_equal(info->preset, 42);
}

//
// Bytes the structure holds and nothing reads are stepped over, and the
// fields after them are read where they belong.
//
static void test_field_decode_skip(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    const test_fields_t *info = test_field_decode_fields(state, 0);

    // Skipped bytes are not those of the binary field which follows them
    assert_int_equal(info->blob.data[0], 0xDE);
}

//
// Data is allowed to end at the beginning of a group, which leaves the
// fields of that group unread and marks the structure as partial rather than
// as broken. The version of the last group entered becomes its level.
//
static void test_field_group_stop(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    const test_fields_t *info = test_field_decode_fields(state, TEST_FIELDS_REQUIRED);

    assert_true(state->entity->state & DMI_ENTITY_STATE_PARTIAL);
    assert_false(state->entity->state & DMI_ENTITY_STATE_INCOMPLETE);
    assert_int_equal(state->entity->level, DMI_VERSION(2, 0, 0));

    assert_int_equal(info->value_byte, 0x12);
    assert_int_equal(info->maybe, 0);
    assert_int_equal(info->newest, 0);

    dmi_entity_destroy(state->entity);
    state->entity = nullptr;

    // Group which the data reaches the end of is entered, so its version is
    // the level of the structure
    info = test_field_decode_fields(state, TEST_FIELDS_REQUIRED + 1);

    assert_true(state->entity->state & DMI_ENTITY_STATE_PARTIAL);
    assert_int_equal(state->entity->level, DMI_VERSION(3, 0, 0));
    assert_int_equal(info->maybe, UINT16_MAX);
}

//
// Member which means something other than zero when the data ends before its
// field holds the value the field declares.
//
static void test_field_group_absent(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    const test_fields_t *info = test_field_decode_fields(state, TEST_FIELDS_REQUIRED);
    assert_int_equal(info->optional, 7);

    dmi_entity_destroy(state->entity);
    state->entity = nullptr;

    // Field which is there carries its own value
    info = test_field_decode_fields(state, 0);
    assert_int_equal(info->optional, 0x05);
}

//
// Data ending anywhere but at the beginning of a group leaves the structure
// incomplete: the fields which are there in full are read, and the rest is
// left unset.
//
static void test_field_group_incomplete(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    // Structure ends in the middle of the word of the newest group
    const test_fields_t *info = test_field_decode_fields(state, TEST_FIELDS_LENGTH - 1);

    assert_true(state->entity->state & DMI_ENTITY_STATE_INCOMPLETE);
    assert_false(state->entity->state & DMI_ENTITY_STATE_PARTIAL);

    assert_int_equal(info->optional, 0x05);
    assert_int_equal(info->newest, 0);
}

//
// Fields before the first group are required, so the data ending within them
// is a broken structure rather than a short one.
//
static void test_field_required(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    test_field_decode(state, test_fields_data, sizeof(test_fields_data),
                      TEST_FIELDS_REQUIRED - 1, TEST_TYPE_FIELDS, false);

    const dmi_error_t *error = dmi_error_get_last(state->context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_INVALID_ENTITY_LENGTH);
}

//
// Elements of an array are found by the length the data declares for them,
// rather than by the fields they are known to hold, and the array which
// carries no number of its own runs to the end of the structure.
//
static void test_field_array(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    const test_array_t *info = test_field_decode(
            state, test_array_data, sizeof(test_array_data), 0, TEST_TYPE_ARRAY, true);

    assert_non_null(info);
    assert_false(state->entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    assert_int_equal(info->declared_count, 2);
    assert_int_equal(info->item_count, 2);
    assert_int_equal(info->item_stride, 4);

    assert_int_equal(info->items[0].a, 0x11);
    assert_int_equal(info->items[0].b, 0x3322);
    assert_int_equal(info->items[1].a, 0x44);
    assert_int_equal(info->items[1].b, 0x6655);

    assert_int_equal(info->plain_count, 3);
    assert_int_equal(info->plain[0], 0xAA);
    assert_int_equal(info->plain[2], 0xCC);
}

//
// Structure ending in the middle of an element is short of the elements it
// declares rather than broken: the ones before it are read, and the number
// the data declares is kept.
//
static void test_field_array_truncated(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    // Only one element and a half of the second one are there
    const test_array_t *info = test_field_decode(
            state, test_array_data, sizeof(test_array_data), 0x0C, TEST_TYPE_ARRAY, true);

    assert_non_null(info);
    assert_true(state->entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    assert_int_equal(info->declared_count, 2);
    assert_int_equal(info->item_count, 1);
    assert_int_equal(info->items[0].a, 0x11);
}

//
// Array of no elements leaves nothing to read, and the one running to the
// end of a structure which ends where the elements do is complete.
//
static void test_field_array_empty(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_byte_t data[sizeof(test_array_data)];
    memcpy(data, test_array_data, sizeof(data));

    data[0x04] = 0x00;  // No elements
    data[0x05] = 0x00;  // Of no length

    const test_array_t *info = test_field_decode(
            state, data, sizeof(data), 0x06, TEST_TYPE_ARRAY, true);

    assert_non_null(info);
    assert_int_equal(info->declared_count, 0);
    assert_int_equal(info->item_count, 0);
    assert_null(info->items);

    assert_int_equal(info->plain_count, 0);
    assert_false(state->entity->state & DMI_ENTITY_STATE_INCOMPLETE);
}

//
// Declaration whose fields reach another offset than the one it states has
// lost or gained a field, which is caught where it happens rather than in
// the values of everything after it.
//
static void test_field_offset_mismatch(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    test_field_decode(state, test_offset_data, sizeof(test_offset_data),
                      0, TEST_TYPE_OFFSET, false);

    // Mismatch is the cause, which the failure to decode is reported over
    const dmi_error_t *error = dmi_error_get_first(state->context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_INTERNAL);
}

//
// Specification which describes no layout cannot be read by the engine.
//
static void test_field_no_fields(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    // Structure of a type which declares no fields is decoded into nothing
    test_field_decode(state, test_bare_data, sizeof(test_bare_data),
                      0, TEST_TYPE_BARE, true);

    dmi_decoder_t decoder;
    assert_true(dmi_decoder_initialize(&decoder, state->entity));

    assert_false(dmi_fields_decode(&decoder));

    const dmi_error_t *error = dmi_error_get_last(state->context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_INVALID_STATE);
}

static void test_field_null_argument(void **pstate)
{
    dmi_unused(pstate);

    assert_false(dmi_fields_decode(nullptr));
    assert_false(dmi_fields_encode(nullptr));
}

// Encode the structure of the fixture back, and give the buffer it is
// written into
static dmi_buffer_t *test_field_encode(
        test_state_t      *state,
        dmi_encode_mode_t  mode,
        dmi_version_t      version)
{
    dmi_buffer_t *buffer = dmi_buffer_create(state->context);
    assert_non_null(buffer);

    dmi_encoder_t encoder;

    assert_true(dmi_encoder_initialize(&encoder, buffer, state->entity, mode, version));
    assert_true(dmi_fields_encode(&encoder));
    assert_true(dmi_encoder_finish(&encoder));

    dmi_encoder_finalize(&encoder);

    return buffer;
}

//
// Structure which has not been changed gives back the very bytes it has been
// decoded from, including the ones the model does not hold.
//
static void test_field_encode_preserve(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    test_field_decode_fields(state, 0);

    dmi_buffer_t *buffer = test_field_encode(state, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE);

    assert_int_equal(buffer->length, TEST_FIELDS_LENGTH);
    assert_memory_equal(buffer->data, test_fields_data, TEST_FIELDS_LENGTH);

    dmi_buffer_destroy(buffer);
}

//
// Canonical mode writes the groups the version it is given defines, and
// stops before the first group of a later one.
//
static void test_field_encode_canonical(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    test_field_decode_fields(state, 0);

    dmi_buffer_t *buffer = test_field_encode(state, DMI_ENCODE_MODE_CANONICAL,
                                             DMI_VERSION(3, 9, 0));

    assert_int_equal(buffer->length, TEST_FIELDS_LENGTH);
    assert_int_equal(buffer->data[0x01], TEST_FIELDS_LENGTH);

    // Values the model holds are written from it, and the bytes it does not
    // hold are written as zeros
    assert_memory_equal(buffer->data + 0x04, test_fields_data + 0x04, 0x21);

    // Bits the specification reserves are cleared, and the ones the fields
    // carry are written back where they were
    assert_int_equal(test_fields_data[0x25], 0xAB);
    assert_int_equal(buffer->data[0x25], 0xA3);

    assert_int_equal(buffer->data[0x26], 0x00);
    assert_int_equal(buffer->data[0x27], 0x00);
    assert_memory_equal(buffer->data + 0x28, test_fields_data + 0x28, 8);

    dmi_buffer_destroy(buffer);

    // Version older than the groups leaves them out
    buffer = test_field_encode(state, DMI_ENCODE_MODE_CANONICAL, DMI_VERSION(2, 0, 0));

    assert_int_equal(buffer->length, TEST_FIELDS_REQUIRED);
    assert_int_equal(buffer->data[0x01], TEST_FIELDS_REQUIRED);

    dmi_buffer_destroy(buffer);
}

//
// Structure which the data ends before the last group of is written back as
// long as it has been read.
//
static void test_field_encode_truncated(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    test_field_decode_fields(state, TEST_FIELDS_REQUIRED + 1);

    dmi_buffer_t *buffer = test_field_encode(state, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE);

    assert_int_equal(buffer->length, TEST_FIELDS_REQUIRED + 1);
    assert_memory_equal(buffer->data + sizeof(dmi_header_t),
                        test_fields_data + sizeof(dmi_header_t),
                        TEST_FIELDS_REQUIRED + 1 - sizeof(dmi_header_t));

    dmi_buffer_destroy(buffer);
}

//
// Arrays are written back with the numbers the data declared for them, and
// the elements keep the bytes the fields do not describe.
//
static void test_field_encode_array(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    test_field_decode(state, test_array_data, sizeof(test_array_data), 0, TEST_TYPE_ARRAY, true);

    dmi_buffer_t *buffer = test_field_encode(state, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE);

    assert_int_equal(buffer->length, 0x11);
    assert_memory_equal(buffer->data, test_array_data, 0x11);

    dmi_buffer_destroy(buffer);
}

//
// Sizes and addresses the fields of a structure are too narrow for are
// carried in kilobytes.
//
static void test_field_kilobytes(void **pstate)
{
    dmi_unused(pstate);

    static const dmi_field_t field = DMI_FIELD(test_fields_t, value_qword, dmi_dword_t);

    uint64_t value = 0;

    dmi_field_data_t data = { .number = 4 };

    assert_true(dmi_field_decode_kilobytes(&field, &data, &value));
    assert_int_equal(value, 4uLL * 1024);

    data.number = 0;

    assert_true(dmi_field_encode_kilobytes(&field, &value, &data));
    assert_int_equal(data.number, 4);
}

//
// Member of a field is read and written as a number, whatever its width.
//
static void test_field_get_set(void **pstate)
{
    dmi_unused(pstate);

    static const dmi_field_t field = DMI_FIELD(test_fields_t, value_word, dmi_word_t);

    test_fields_t info = {};

    assert_true(dmi_field_set(&field, &info.value_word, 0x1234));
    assert_int_equal(info.value_word, 0x1234);
    assert_int_equal(dmi_field_get(&field, &info.value_word), 0x1234);

    // Member is wider than the field, so it holds what the field cannot
    assert_true(dmi_field_set(&field, &info.value_word, UINT32_MAX));
    assert_int_equal(dmi_field_get(&field, &info.value_word), UINT32_MAX);
}
