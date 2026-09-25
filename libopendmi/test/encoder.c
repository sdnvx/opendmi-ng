//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/encoder.h>
#include <opendmi/entity.h>
#include <opendmi/error.h>
#include <opendmi/field.h>
#include <opendmi/log.h>
#include <opendmi/module.h>
#include <opendmi/registry.h>
#include <opendmi/internal.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/memory-array-addr.h>
#include <opendmi/entity/power-supply.h>
#include <opendmi/entity/system.h>

static int test_encoder_setup(void **pstate);
static int test_encoder_teardown(void **pstate);

static void test_encoder_reserved_bits(void **pstate);
static void test_encoder_extended_governed(void **pstate);
static void test_encoder_extended_canonical(void **pstate);
static void test_encoder_truncated(void **pstate);
static void test_encoder_strings(void **pstate);
static void test_encoder_groups(void **pstate);
static void test_encoder_modules(void **pstate);
static void test_encoder_corpus(void **pstate);

static void test_encoder_null_arguments(void **pstate);
static void test_encoder_header(void **pstate);
static void test_encoder_appends(void **pstate);
static void test_encoder_put(void **pstate);
static void test_encoder_put_at(void **pstate);
static void test_encoder_source_in_step(void **pstate);
static void test_encoder_no_source(void **pstate);
static void test_encoder_copy(void **pstate);
static void test_encoder_copy_past_source(void **pstate);
static void test_encoder_string_preserve(void **pstate);
static void test_encoder_string_missing(void **pstate);
static void test_encoder_string_canonical(void **pstate);
static void test_encoder_string_raw(void **pstate);
static void test_encoder_finish(void **pstate);
static void test_encoder_finish_too_long(void **pstate);
static void test_encoder_finish_end_of_table(void **pstate);
static void test_encoder_finalize(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

// Dumps the corpus test encodes, given on the command line
static char **test_dumps;
static int    test_dump_count;

int main(int argc, char **argv)
{
    test_dumps      = argv + 1;
    test_dump_count = argc - 1;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_encoder_reserved_bits, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_extended_governed, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_extended_canonical, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_truncated, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_strings, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_groups, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_modules, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_null_arguments, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_header, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_appends, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_put, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_put_at, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_source_in_step, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_no_source, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_copy, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_copy_past_source, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_string_preserve, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_string_missing, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_string_canonical, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_string_raw, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_finish, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_finish_too_long, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_finish_end_of_table, test_encoder_setup, test_encoder_teardown),
        cmocka_unit_test_setup_teardown(test_encoder_finalize, test_encoder_setup, test_encoder_teardown),

        cmocka_unit_test(test_encoder_corpus)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_encoder_setup(void **pstate)
{
    dmi_context_t *context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_encoder_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

// Create a structure from its bytes, and decode it
static dmi_entity_t *test_decode(
        dmi_buffer_t  *buffer,
        const uint8_t *data,
        size_t         size)
{
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, size);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    return entity;
}

// Encode a decoded structure by the fields of its specification
static void test_encode(
        const dmi_entity_t *entity,
        dmi_encode_mode_t   mode,
        dmi_version_t       version,
        dmi_buffer_t       *buffer,
        dmi_encoder_t      *encoder)
{
    dmi_buffer_clear(buffer);

    assert_true(dmi_encoder_initialize(encoder, buffer, entity, mode, version));
    assert_true(dmi_fields_encode(encoder));
}

//
// Bits the specification reserves are kept in the preserve mode, since the
// model holds nothing of them, and cleared in the canonical one.
//
static void test_encoder_reserved_bits(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);

    // Power supply whose characteristics set the two reserved bits on top of
    // the ones carrying the fields
    static const uint8_t data[] = {
        39, 0x16, 0x00, 0x50,
        0x01,                                       // Group
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // Strings
        0x64, 0x00,                                 // Maximum capacity
        0x93, 0xD1,                                 // Characteristics
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,         // Probe and cooling device handles
        0x00, 0x00
    };

    dmi_entity_t *entity = test_decode(entity_buffer, data, sizeof(data));

    const dmi_power_supply_t *info = dmi_entity_info(entity, DMI_TYPE(POWER_SUPPLY));
    assert_non_null(info);
    assert_int_equal(info->type, 4);

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_encoder_t encoder;

    test_encode(entity, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE, buffer, &encoder);
    assert_int_equal(buffer->length, 0x16);
    assert_memory_equal(buffer->data, data, 0x16);
    dmi_encoder_finalize(&encoder);

    test_encode(entity, DMI_ENCODE_MODE_CANONICAL, DMI_VERSION(3, 9, 0), buffer, &encoder);
    assert_int_equal(buffer->length, 0x16);
    assert_int_equal(buffer->data[0x0E], 0x93);
    assert_int_equal(buffer->data[0x0F], 0x11);
    dmi_encoder_finalize(&encoder);

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}

//
// Extended fields widened together are chosen by the plain field governing
// them, which is the one of the starting address for the mapped ranges.
//
static const uint8_t test_array_addr_ex[] = {
    19, 0x1F, 0x00, 0x20,
    0xFF, 0xFF, 0xFF, 0xFF,                             // Starting address
    0xFF, 0xFF, 0xFF, 0xFF,                             // Ending address
    0x01, 0x00,                                         // Memory array handle
    0x01,                                               // Partition width
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,     // Extended starting address
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00,     // Extended ending address
    0x00, 0x00
};

static void test_encoder_extended_governed(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);

    dmi_entity_t *entity = test_decode(entity_buffer, test_array_addr_ex, sizeof(test_array_addr_ex));

    const dmi_memory_array_addr_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_ARRAY_ADDR));
    assert_non_null(info);
    assert_int_equal(info->start_addr, 0x0000010000000000uLL);
    assert_int_equal(info->end_addr,   0x000001FFFFFFFFFFuLL);

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_encoder_t encoder;

    test_encode(entity, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE, buffer, &encoder);
    assert_int_equal(buffer->length, 0x1F);
    assert_memory_equal(buffer->data, test_array_addr_ex, 0x1F);
    dmi_encoder_finalize(&encoder);

    // Starting address fits the plain field, but the ending one does not, so
    // both are carried by the extended fields
    test_encode(entity, DMI_ENCODE_MODE_CANONICAL, DMI_VERSION(3, 9, 0), buffer, &encoder);
    assert_int_equal(buffer->length, 0x1F);
    assert_memory_equal(buffer->data, test_array_addr_ex, 0x1F);
    dmi_encoder_finalize(&encoder);

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}

//
// Addresses which fit the plain fields are written to them in the canonical
// mode, and decoding the result gives back the same structure.
//
static void test_encoder_extended_canonical(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);

    uint8_t data[sizeof(test_array_addr_ex)];
    memcpy(data, test_array_addr_ex, sizeof(data));

    // Extended ending address is a whole number of kilobytes
    data[0x17] = 0x00;
    data[0x18] = 0xFC;

    dmi_entity_t *entity = test_decode(entity_buffer, data, sizeof(data));

    const dmi_memory_array_addr_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_ARRAY_ADDR));
    assert_non_null(info);

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_encoder_t encoder;

    test_encode(entity, DMI_ENCODE_MODE_CANONICAL, DMI_VERSION(3, 9, 0), buffer, &encoder);
    assert_int_equal(buffer->length, 0x1F);

    // Plain fields carry the addresses in kilobytes, and the extended ones
    // are left zero
    static const uint8_t plain[] = { 0x00, 0x00, 0x00, 0x40, 0xFF, 0xFF, 0xFF, 0x7F };
    assert_memory_equal(buffer->data + 0x04, plain, sizeof(plain));

    for (size_t i = 0x0F; i < 0x1F; i++)
        assert_int_equal(buffer->data[i], 0x00);

    // Structure written this way decodes to the same addresses
    uint8_t encoded[0x1F + 2] = {};
    memcpy(encoded, buffer->data, 0x1F);
    dmi_encoder_finalize(&encoder);

    dmi_buffer_t *decoded_buffer = dmi_buffer_create(context);
    dmi_entity_t *decoded = test_decode(decoded_buffer, encoded, sizeof(encoded));

    const dmi_memory_array_addr_t *decoded_info = dmi_entity_info(decoded, DMI_TYPE(MEMORY_ARRAY_ADDR));
    assert_non_null(decoded_info);
    assert_int_equal(decoded_info->start_addr, info->start_addr);
    assert_int_equal(decoded_info->end_addr,   info->end_addr);

    dmi_entity_destroy(decoded);

    dmi_buffer_destroy(decoded_buffer);
    dmi_entity_destroy(entity);
    dmi_buffer_destroy(entity_buffer);
}

//
// Data which ends in the middle of a field is kept as it is, since the model
// holds nothing of a field it has not read in full.
//
static void test_encoder_truncated(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);

    // System information ending in the middle of the UUID
    static const uint8_t data[] = {
        1, 0x0D, 0x01, 0x00,
        0x01, 0x00, 0x00, 0x00,                     // Strings
        0x10, 0x32, 0x54, 0x76, 0x98,               // Beginning of the UUID
        'A', 0x00, 0x00
    };

    dmi_entity_t *entity = test_decode(entity_buffer, data, sizeof(data));
    assert_true(entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_encoder_t encoder;

    test_encode(entity, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE, buffer, &encoder);
    assert_int_equal(buffer->length, 0x0D);
    assert_memory_equal(buffer->data, data, 0x0D);
    dmi_encoder_finalize(&encoder);

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}

//
// Strings keep their numbers in the preserve mode, including the ones nothing
// refers to, and are shared by their text and renumbered in the canonical
// mode.
//
static const uint8_t test_system_strings[] = {
    1, 0x1B, 0x01, 0x00,
    0x01, 0x02, 0x03, 0x00,                         // Vendor, product, version and serial number
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, // UUID
    0x06,                                           // Wake-up type
    0x00, 0x00,                                     // SKU number and family
    'A', 0x00, 'B', 0x00, 'A', 0x00, 'u', 'n', 'u', 's', 'e', 'd', 0x00, 0x00
};

static void test_encoder_strings(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);
    dmi_entity_t  *entity  = test_decode(entity_buffer, test_system_strings, sizeof(test_system_strings));

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_encoder_t encoder;

    test_encode(entity, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE, buffer, &encoder);
    assert_int_equal(buffer->length, 0x1B);
    assert_memory_equal(buffer->data, test_system_strings, 0x1B);
    assert_int_equal(encoder.string_count, 4);
    assert_string_equal(encoder.strings[3], "unused");
    dmi_encoder_finalize(&encoder);

    test_encode(entity, DMI_ENCODE_MODE_CANONICAL, DMI_VERSION(3, 9, 0), buffer, &encoder);
    assert_int_equal(buffer->data[0x04], 1);
    assert_int_equal(buffer->data[0x05], 2);
    assert_int_equal(buffer->data[0x06], 1);
    assert_int_equal(buffer->data[0x07], 0);
    assert_int_equal(encoder.string_count, 2);
    assert_string_equal(encoder.strings[0], "A");
    assert_string_equal(encoder.strings[1], "B");
    dmi_encoder_finalize(&encoder);

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}

//
// Canonical mode writes the groups of the fields the version it is given
// defines, and stops before the first group of a later one.
//
static void test_encoder_groups(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);

    dmi_entity_t *entity = test_decode(entity_buffer, test_system_strings, sizeof(test_system_strings));

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_encoder_t encoder;

    test_encode(entity, DMI_ENCODE_MODE_CANONICAL, DMI_VERSION(2, 1, 0), buffer, &encoder);
    assert_int_equal(buffer->length, 0x19);
    assert_int_equal(buffer->data[0x01], 0x19);
    dmi_encoder_finalize(&encoder);

    test_encode(entity, DMI_ENCODE_MODE_CANONICAL, DMI_VERSION(2, 0, 0), buffer, &encoder);
    assert_int_equal(buffer->length, 0x08);
    dmi_encoder_finalize(&encoder);

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}

//
// Compare two decoded structures member by member, as far as the fields of
// their specification describe them, which is the model the writer writes
// from. Members a field splits the data into are not known to the fields,
// and are left out.
//
static bool test_fields_equal(
        const dmi_field_t *fields,
        const dmi_data_t  *a,
        const dmi_data_t  *b,
        const char       **where);

static bool test_member_equal(const dmi_field_t *field, const dmi_data_t *a, const dmi_data_t *b)
{
    const void *x = a + field->member.offset;
    const void *y = b + field->member.offset;

    // Members of the fields decoded by their own handlers are whatever the
    // handlers make of the data, e.g. the date a string spells
    if (field->params.decode != nullptr)
        return memcmp(x, y, field->member.size) == 0;

    switch (field->type) {
    case DMI_FIELD_TYPE_STRING: {
        const char *p = *(const char * const *)x;
        const char *q = *(const char * const *)y;
        return ((p == nullptr) and (q == nullptr)) or
               ((p != nullptr) and (q != nullptr) and (strcmp(p, q) == 0));
    }

    case DMI_FIELD_TYPE_BINARY: {
        const dmi_binary_t *p = x;
        const dmi_binary_t *q = y;
        return (p->length == q->length) and
               ((p->length == 0) or (memcmp(p->data, q->data, p->length) == 0));
    }

    default:
        return memcmp(x, y, field->member.size) == 0;
    }
}

static bool test_fields_equal(
        const dmi_field_t *fields,
        const dmi_data_t  *a,
        const dmi_data_t  *b,
        const char       **where)
{
    for (const dmi_field_t *field = fields; field->type != DMI_FIELD_TYPE_NONE; field++) {
        if (not dmi_member_is_present(field->member))
            continue;

        // Fields splitting the data into several members name the whole
        // structure, whose pointers differ between two decodings
        if ((field->params.decode != nullptr) and (field->member.offset == 0) and
            (field->member.size > 16))
            continue;

        switch (field->type) {
        case DMI_FIELD_TYPE_PRESET:
            continue;


        case DMI_FIELD_TYPE_STRUCT:
            if (not test_fields_equal(field->params.fields, a + field->member.offset,
                                      b + field->member.offset, where))
                return false;
            continue;

        case DMI_FIELD_TYPE_ARRAY: {
            size_t count = *(const size_t *)(a + field->params.counter.offset);

            if (count != *(const size_t *)(b + field->params.counter.offset)) {
                *where = "number of the elements of an array";
                return false;
            }

            const dmi_data_t *x = *(dmi_data_t * const *)(a + field->member.offset);
            const dmi_data_t *y = *(dmi_data_t * const *)(b + field->member.offset);

            for (size_t i = 0; i < count; i++) {
                if (not test_fields_equal(field->params.fields, x + (i * field->member.size),
                                          y + (i * field->member.size), where))
                    return false;
            }
            continue;
        }

        default:
            break;
        }

        if (not test_member_equal(field, a, b)) {
            *where = "member";
            return false;
        }
    }

    return true;
}

//
// Write a structure the canonical way for a version, as the formatted area
// followed by the strings and the terminator of the set.
//
static dmi_byte_t *test_canonical_bytes(const dmi_entity_t *entity, dmi_version_t version, size_t *size)
{
    dmi_buffer_t *buffer = dmi_buffer_create(dmi_entity_context(entity));
    dmi_encoder_t encoder;

    assert_non_null(buffer);

    if (not dmi_encoder_initialize(&encoder, buffer, entity, DMI_ENCODE_MODE_CANONICAL, version)) {
        return nullptr;
    }

    if (not dmi_entity_encode(&encoder)) {
        dmi_encoder_finalize(&encoder);
        return nullptr;
    }

    *size = buffer->length + 1;
    for (size_t i = 0; i < encoder.string_count; i++)
        *size += strlen(encoder.strings[i]) + 1;
    if (encoder.string_count == 0)
        (*size)++;

    dmi_byte_t *data = calloc(1, *size);
    if (data == nullptr) {
        dmi_encoder_finalize(&encoder);
        return nullptr;
    }

    memcpy(data, buffer->data, buffer->length);

    size_t position = buffer->length;
    for (size_t i = 0; i < encoder.string_count; i++) {
        size_t length = strlen(encoder.strings[i]) + 1;
        memcpy(data + position, encoder.strings[i], length);
        position += length;
    }

    dmi_encoder_finalize(&encoder);
    dmi_buffer_destroy(buffer);

    return data;
}

//
// Write a structure the canonical way for the version it has been decoded
// at, decode what has been written, and check the result, which is how the
// encoding handlers are checked: the preserve mode keeps the data of the
// source whenever it decodes into the same values. Structures described by
// fields are compared member by member, and the rest are written the
// canonical way once more, which has to give the same bytes.
//
static bool test_canonical_roundtrip(dmi_context_t *context, const dmi_entity_t *entity, const char **where)
{
    size_t      size = 0;
    dmi_byte_t *data = test_canonical_bytes(entity, entity->level, &size);

    *where = "encoding";

    if (data == nullptr)
        return false;

    bool equal = false;

    *where = "decoding";

    dmi_buffer_t *decoded_buffer = dmi_buffer_create(context);

    dmi_entity_t *decoded = dmi_test_entity_create(decoded_buffer, data, size);
    if ((decoded != nullptr) and dmi_entity_decode(decoded)) {
        const dmi_entity_spec_t *spec = entity->spec;

        if ((spec->fields != nullptr) and (spec->handlers.decode == nullptr)) {
            equal = (decoded->info != nullptr) and
                    test_fields_equal(spec->fields, entity->info, decoded->info, where);
        } else {
            size_t      again_size = 0;
            dmi_byte_t *again      = test_canonical_bytes(decoded, entity->level, &again_size);

            *where = "writing the decoded structure again";

            equal = (again != nullptr) and (again_size == size) and (memcmp(again, data, size) == 0);

            free(again);
        }
    }

    dmi_entity_destroy(decoded);

    free(data);

    return equal;
}

//
// Encode a structure both ways: the preserve mode gives back its bytes, and
// the canonical one is a fixed point, which gives the same bytes once what it
// has written is decoded and written again. The canonical length is checked
// too, when given.
//
static void test_encode_both_ways(dmi_context_t *context, const uint8_t *data, size_t size,
                                  size_t canonical_length)
{
    dmi_buffer_t *entity_buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = test_decode(entity_buffer, data, size);

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_encoder_t encoder;

    assert_true(dmi_encoder_initialize(&encoder, buffer, entity, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE));
    assert_true(dmi_entity_encode(&encoder));
    assert_int_equal(buffer->length, entity->body_length);
    assert_memory_equal(buffer->data, data, buffer->length);
    dmi_encoder_finalize(&encoder);

    size_t      first_size = 0;
    dmi_byte_t *first      = test_canonical_bytes(entity, DMI_VERSION(3, 9, 0), &first_size);
    assert_non_null(first);

    if (canonical_length != 0)
        assert_int_equal(first[1], canonical_length);

    dmi_buffer_t *decoded_buffer = dmi_buffer_create(context);
    dmi_entity_t *decoded = test_decode(decoded_buffer, first, first_size);

    size_t      second_size = 0;
    dmi_byte_t *second      = test_canonical_bytes(decoded, DMI_VERSION(3, 9, 0), &second_size);
    assert_non_null(second);
    assert_int_equal(second_size, first_size);
    assert_memory_equal(second, first, first_size);

    free(second);
    dmi_entity_destroy(decoded);
    dmi_buffer_destroy(decoded_buffer);
    free(first);
    dmi_entity_destroy(entity);
    dmi_buffer_destroy(entity_buffer);
}

//
// Structures of the modules the corpus has none of are written by their own
// handlers: the tokens of Dell, which drop the unused records and end at a
// marker, and the CPUID leaves of Intel RSD, whose number the subtype says.
//
static void test_encoder_modules(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_add_extension(context, dmi_module_find("dell")));
    assert_true(dmi_add_extension(context, dmi_module_find("intel")));

    // Calling interface of Dell Precision 490, with an unused token added,
    // which the canonical mode leaves out along with the rest of the marker
    static const uint8_t calling_iface[] = {
        218, 0x23, 0x00, 0xDA,
        0xB2, 0x00, 0x17,
        0x0B, 0x0E, 0x38, 0x00,
        0x00, 0x80, 0x00, 0x80, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x80, 0x02, 0x80, 0x01, 0x00,
        0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00
    };

    test_encode_both_ways(context, calling_iface, sizeof(calling_iface), 0x19);

    // Indexed I/O with an unused token and a marker cut short
    static const uint8_t indexed_io[] = {
        212, 0x23, 0x00, 0xD4,
        0x70, 0x00, 0x71, 0x00,
        0x00, 0x10, 0x2D, 0x2E,
        0x03, 0x00, 0x11, 0x7F, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x04, 0x00, 0x11, 0x7F, 0x00,
        0x5C, 0x00, 0x40, 0x00, 0x08,
        0xFF, 0xFF, 0x00,
        0x00, 0x00
    };

    test_encode_both_ways(context, indexed_io, sizeof(indexed_io), 0x1D);

    // CPUID leaves of the basic subtype, all fourteen of them, and of a
    // subtype no table knows, which is kept as it is stored
    for (uint8_t subtype = 1; subtype <= 7; subtype += 6) {
        uint8_t data[6 + 14 * 16 + sizeof("Socket 0") + 1] = {};

        size_t length = 6 + 14 * 16;

        data[0] = 193;
        data[1] = (uint8_t)length;
        data[3] = 0xC1;
        data[4] = 1;
        data[5] = subtype;

        for (size_t i = 0; i < 14 * 16; i++)
            data[6 + i] = (uint8_t)i;

        memcpy(data + length, "Socket 0", sizeof("Socket 0"));

        test_encode_both_ways(context, data, sizeof(data), length);
    }
}

//
// Every structure of the corpus is encoded in the preserve mode into the very
// bytes and strings it has been decoded from, and the ones a specification
// describes decode into the same structure once written the canonical way.
//
static void test_encoder_corpus(void **pstate)
{
    dmi_unused(pstate);

    if (test_dump_count == 0)
        skip();

    size_t encoded = 0;
    size_t failed  = 0;

    for (int i = 0; i < test_dump_count; i++) {
        // Structures are linked, since linking leaves the members the fields
        // decode into as they are, and the writer writes them back
        dmi_context_t *context = dmi_create(DMI_CONTEXT_FLAG_LINK);
        assert_non_null(context);

        if (not dmi_load(context, test_dumps[i])) {
            print_error("%s: cannot be loaded\n", test_dumps[i]);
            failed++;
            dmi_destroy(context);
            continue;
        }

        dmi_registry_iter_t iter;
        assert_true(dmi_registry_iter_init(&iter, dmi_get_registry(context), nullptr));

        dmi_entity_t *entity;
        while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
            const dmi_entity_spec_t *spec = entity->spec;

            // Structures whose model failed to decode have nothing to write
            // back from, while the ones with no model are kept as they are
            bool modelled = (spec != nullptr) and
                            ((spec->fields != nullptr) or (spec->handlers.decode != nullptr));

            if (modelled and ((entity->state & DMI_ENTITY_STATE_DECODED) == 0))
                continue;

            const char *code = (spec != nullptr) ? spec->code : "unknown";

            dmi_buffer_t *buffer = dmi_buffer_create(context);
            dmi_encoder_t encoder;

                    assert_true(dmi_encoder_initialize(&encoder, buffer, entity,
                                               DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE));

            if (not dmi_entity_encode(&encoder)) {
                const dmi_error_t *error = dmi_error_peek_last(context);

                print_error("%s: handle 0x%04X (%s) cannot be encoded: %s\n",
                            test_dumps[i], entity->handle, code,
                            ((error != nullptr) and (error->message != nullptr)) ? error->message : "");
                failed++;
                dmi_encoder_finalize(&encoder);
                continue;
            }

            const dmi_data_t *data = dmi_buffer_at(dmi_entity_buffer(entity),
                                                  dmi_entity_offset(entity), entity->body_length);

            bool matches = (buffer->length == entity->body_length) and
                           (memcmp(buffer->data, data, buffer->length) == 0) and
                           (encoder.string_count == entity->string_count);

            for (size_t k = 0; matches and (k < encoder.string_count); k++)
                matches = ((encoder.strings[k] == nullptr) and (entity->strings[k].raw == nullptr)) or
                          ((encoder.strings[k] != nullptr) and (entity->strings[k].raw != nullptr) and
                           (strcmp(encoder.strings[k], entity->strings[k].raw) == 0));

            dmi_encoder_finalize(&encoder);

            if (not matches) {
                print_error("%s: handle 0x%04X (%s) is encoded into other bytes\n",
                            test_dumps[i], entity->handle, code);
                failed++;
                continue;
            }

            // Structures the data cuts short have lost what the canonical
            // mode would write in full
            const char *where = nullptr;

            if ((spec != nullptr) and ((entity->state & DMI_ENTITY_STATE_INCOMPLETE) == 0) and
                not test_canonical_roundtrip(context, entity, &where)) {
                print_error("%s: handle 0x%04X (%s) is decoded into another structure "
                            "once written the canonical way: %s\n",
                            test_dumps[i], entity->handle, code, where);
                failed++;
                continue;
            }

            encoded++;
        }

        dmi_destroy(context);
    }

    print_message("%zu structures encoded back both ways\n", encoded);

    assert_int_equal(failed, 0);
    assert_true(encoded > 0);
}

//
// Tests of the encoder itself, which the ones above reach only through the
// field engine: the header it writes, the source it keeps in step with what
// is written, and the strings it numbers.
//

// Inactive structure, which has no fields of its own, carrying the bytes the
// tests read as a source and two strings the references name
static const dmi_byte_t test_source[] = {
    DMI_TYPE_INACTIVE, 0x0A, 0x34, 0x12,    // Header
    0x11, 0x22, 0x33, 0x44,                 // Bytes the model does not hold
    0x02,                                   // Reference to the second string
    0x09,                                   // Reference to a string which is not there

    ' ', 'A', ' ', 0x00,
    'B', 0x00,
    0x00
};

#define TEST_SOURCE_BODY   0x0A
#define TEST_SOURCE_VALUES 0x04
#define TEST_SOURCE_STRING 0x08

// End-of-table structure as the firmware writes it: a header of no length,
// which stands for the end of the table whatever type it names
static const dmi_byte_t test_end_of_table[] = {
    0x7F, 0x00, 0x00, 0x00,
    0x00, 0x00
};

typedef struct test_encoder_fixture test_encoder_fixture_t;

struct test_encoder_fixture
{
    dmi_buffer_t *source;
    dmi_buffer_t *buffer;
    dmi_entity_t *entity;
    dmi_encoder_t encoder;
};

// Set an encoder up over a structure built out of the given bytes
static void test_encoder_open(
        dmi_context_t          *context,
        test_encoder_fixture_t *fixture,
        const dmi_byte_t       *data,
        size_t                  length,
        dmi_encode_mode_t       mode)
{
    fixture->source = dmi_buffer_create(context);
    fixture->buffer = dmi_buffer_create(context);

    assert_non_null(fixture->source);
    assert_non_null(fixture->buffer);

    fixture->entity = dmi_test_entity_create(fixture->source, data, length);
    assert_non_null(fixture->entity);

    assert_true(dmi_encoder_initialize(&fixture->encoder, fixture->buffer,
                                       fixture->entity, mode, DMI_VERSION(3, 9, 0)));
}

static void test_encoder_close(test_encoder_fixture_t *fixture)
{
    dmi_encoder_finalize(&fixture->encoder);
    dmi_entity_destroy(fixture->entity);

    dmi_buffer_destroy(fixture->buffer);
    dmi_buffer_destroy(fixture->source);
}

static void test_encoder_null_arguments(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *buffer  = dmi_buffer_create(context);

    assert_non_null(buffer);

    dmi_encoder_t encoder;

    assert_false(dmi_encoder_initialize(nullptr, buffer, nullptr,
                                        DMI_ENCODE_MODE_CANONICAL, DMI_VERSION_NONE));

    const dmi_error_t *error = dmi_error_get_last(context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_NULL_ARGUMENT);

    assert_false(dmi_encoder_initialize(&encoder, nullptr, nullptr,
                                        DMI_ENCODE_MODE_CANONICAL, DMI_VERSION_NONE));

    assert_null(dmi_encoder_entity(nullptr));
    assert_null(dmi_encoder_writer(nullptr));

    // Encoder which holds no strings is finished with all the same
    dmi_encoder_finalize(nullptr);

    dmi_buffer_destroy(buffer);
}

//
// Header of the structure is written on initialization, and its length is
// left for `dmi_encoder_finish()` to fill in.
//
static void test_encoder_header(void **pstate)
{
    dmi_context_t *context = *pstate;

    test_encoder_fixture_t fixture;
    test_encoder_open(context, &fixture, test_source, sizeof(test_source),
                      DMI_ENCODE_MODE_PRESERVE);

    assert_uint_equal(fixture.buffer->length, sizeof(dmi_header_t));
    assert_uint_equal(dmi_encoder_tell(&fixture.encoder), sizeof(dmi_header_t));

    assert_int_equal(fixture.buffer->data[0x00], DMI_TYPE_INACTIVE);
    assert_int_equal(fixture.buffer->data[0x01], 0x00);
    assert_int_equal(fixture.buffer->data[0x02], 0x34);
    assert_int_equal(fixture.buffer->data[0x03], 0x12);

    assert_ptr_equal(dmi_encoder_entity(&fixture.encoder), fixture.entity);
    assert_ptr_equal(dmi_encoder_writer(&fixture.encoder), &fixture.encoder.writer);

    test_encoder_close(&fixture);
}

//
// Structure is written at the end of the data the buffer holds, so that the
// structures of a table follow one another.
//
static void test_encoder_appends(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *source  = dmi_buffer_create(context);
    dmi_buffer_t  *buffer  = dmi_buffer_create(context);

    assert_non_null(source);
    assert_non_null(buffer);
    assert_true(dmi_buffer_assign(buffer, test_source, sizeof(test_source)));

    dmi_entity_t *entity = dmi_test_entity_create(source, test_source, sizeof(test_source));
    assert_non_null(entity);

    dmi_encoder_t encoder;
    assert_true(dmi_encoder_initialize(&encoder, buffer, entity,
                                       DMI_ENCODE_MODE_CANONICAL, DMI_VERSION(3, 9, 0)));

    // Data which was there is left alone, and the structure begins past it
    assert_uint_equal(dmi_encoder_tell(&encoder), sizeof(dmi_header_t));
    assert_uint_equal(buffer->length, sizeof(test_source) + sizeof(dmi_header_t));
    assert_memory_equal(buffer->data, test_source, sizeof(test_source));
    assert_int_equal(buffer->data[sizeof(test_source)], DMI_TYPE_INACTIVE);

    dmi_encoder_finalize(&encoder);
    dmi_entity_destroy(entity);

    dmi_buffer_destroy(buffer);
    dmi_buffer_destroy(source);
}

//
// Values are written into the wire format the specification stores them in.
//
static void test_encoder_put(void **pstate)
{
    dmi_context_t *context = *pstate;

    test_encoder_fixture_t fixture;
    test_encoder_open(context, &fixture, test_source, sizeof(test_source),
                      DMI_ENCODE_MODE_CANONICAL);

    dmi_encoder_t *encoder = &fixture.encoder;

    assert_true(dmi_encoder_put(encoder, dmi_word_t, 0x3456));
    assert_true(dmi_encoder_put(encoder, dmi_dword_t, 0x789ABCDEuL));
    assert_true(dmi_encoder_put_bcd(encoder, dmi_byte_t, 42));
    assert_true(dmi_encoder_put_uuid(encoder, dmi_uuid_decode(test_source)));

    static const dmi_byte_t expected[] = {
        0x56, 0x34,
        0xDE, 0xBC, 0x9A, 0x78,
        0x42
    };

    assert_memory_equal(fixture.buffer->data + sizeof(dmi_header_t), expected, sizeof(expected));
    assert_memory_equal(fixture.buffer->data + sizeof(dmi_header_t) + sizeof(expected),
                        test_source, 16);

    assert_uint_equal(dmi_encoder_tell(encoder), sizeof(dmi_header_t) + sizeof(expected) + 16);

    // Nothing to write leaves the position where it is
    assert_true(dmi_encoder_put_bytes(encoder, nullptr, 0));
    assert_uint_equal(dmi_encoder_tell(encoder), sizeof(dmi_header_t) + sizeof(expected) + 16);

    test_encoder_close(&fixture);
}

//
// Value the rest of the structure decides, such as a length, is filled in
// once it is known, without the position being moved.
//
static void test_encoder_put_at(void **pstate)
{
    dmi_context_t *context = *pstate;

    test_encoder_fixture_t fixture;
    test_encoder_open(context, &fixture, test_source, sizeof(test_source),
                      DMI_ENCODE_MODE_CANONICAL);

    dmi_encoder_t *encoder = &fixture.encoder;

    assert_true(dmi_encoder_put(encoder, dmi_word_t, 0x0000));
    assert_true(dmi_encoder_put(encoder, dmi_word_t, 0x1111));

    size_t position = dmi_encoder_tell(encoder);

    assert_true(dmi_encoder_put_at(encoder, sizeof(dmi_header_t), dmi_word_t, 0x3456));

    assert_uint_equal(dmi_encoder_tell(encoder), position);
    assert_int_equal(fixture.buffer->data[sizeof(dmi_header_t)], 0x56);
    assert_int_equal(fixture.buffer->data[sizeof(dmi_header_t) + 1], 0x34);
    assert_int_equal(fixture.buffer->data[sizeof(dmi_header_t) + 2], 0x11);

    test_encoder_close(&fixture);
}

//
// Source goes alongside in step with what is written, so that the bytes the
// model does not hold are found at the position they are written at.
//
static void test_encoder_source_in_step(void **pstate)
{
    dmi_context_t *context = *pstate;

    test_encoder_fixture_t fixture;
    test_encoder_open(context, &fixture, test_source, sizeof(test_source),
                      DMI_ENCODE_MODE_PRESERVE);

    dmi_encoder_t *encoder = &fixture.encoder;

    // Header has been written, so the source stands at the first value
    assert_uint_equal(dmi_encoder_remaining(encoder), TEST_SOURCE_BODY - sizeof(dmi_header_t));

    dmi_byte_t value = 0;

    assert_true(dmi_encoder_peek(encoder, &value, sizeof(value)));
    assert_int_equal(value, test_source[TEST_SOURCE_VALUES]);

    // Peeking reads the source without advancing it
    assert_true(dmi_encoder_peek(encoder, &value, sizeof(value)));
    assert_int_equal(value, test_source[TEST_SOURCE_VALUES]);

    assert_true(dmi_encoder_put(encoder, dmi_byte_t, 0xFF));

    assert_true(dmi_encoder_peek(encoder, &value, sizeof(value)));
    assert_int_equal(value, test_source[TEST_SOURCE_VALUES + 1]);
    assert_uint_equal(dmi_encoder_remaining(encoder), TEST_SOURCE_BODY - sizeof(dmi_header_t) - 1);

    // Source ends with the structure it has been read from, and reading past
    // it gives nothing rather than the bytes which follow
    assert_true(dmi_encoder_put_bytes(encoder, test_source, TEST_SOURCE_BODY));
    assert_uint_equal(dmi_encoder_remaining(encoder), 0);
    assert_false(dmi_encoder_peek(encoder, &value, sizeof(value)));

    test_encoder_close(&fixture);
}

//
// Canonical mode reads no source at all, so the bytes the model does not
// hold are written as zeros.
//
static void test_encoder_no_source(void **pstate)
{
    dmi_context_t *context = *pstate;

    test_encoder_fixture_t fixture;
    test_encoder_open(context, &fixture, test_source, sizeof(test_source),
                      DMI_ENCODE_MODE_CANONICAL);

    dmi_encoder_t *encoder = &fixture.encoder;

    dmi_byte_t value = 0xFF;

    assert_uint_equal(dmi_encoder_remaining(encoder), 0);
    assert_false(dmi_encoder_peek(encoder, &value, sizeof(value)));
    assert_int_equal(value, 0xFF);

    static const dmi_byte_t zeros[4] = {};

    assert_true(dmi_encoder_copy(encoder, sizeof(zeros)));
    assert_memory_equal(fixture.buffer->data + sizeof(dmi_header_t), zeros, sizeof(zeros));

    test_encoder_close(&fixture);
}

//
// Preserve mode takes the bytes the model does not hold from the data the
// structure has been decoded from.
//
static void test_encoder_copy(void **pstate)
{
    dmi_context_t *context = *pstate;

    test_encoder_fixture_t fixture;
    test_encoder_open(context, &fixture, test_source, sizeof(test_source),
                      DMI_ENCODE_MODE_PRESERVE);

    dmi_encoder_t *encoder = &fixture.encoder;

    assert_true(dmi_encoder_copy(encoder, 4));

    assert_memory_equal(fixture.buffer->data + sizeof(dmi_header_t),
                        test_source + TEST_SOURCE_VALUES, 4);
    assert_uint_equal(dmi_encoder_tell(encoder), sizeof(dmi_header_t) + 4);

    test_encoder_close(&fixture);
}

//
// Bytes past the end of the source are written as zeros, the way the
// canonical mode writes all of them.
//
static void test_encoder_copy_past_source(void **pstate)
{
    dmi_context_t *context = *pstate;

    test_encoder_fixture_t fixture;
    test_encoder_open(context, &fixture, test_source, sizeof(test_source),
                      DMI_ENCODE_MODE_PRESERVE);

    dmi_encoder_t *encoder = &fixture.encoder;

    static const dmi_byte_t zeros[4] = {};

    assert_true(dmi_encoder_copy(encoder, TEST_SOURCE_BODY - sizeof(dmi_header_t) + 4));

    assert_memory_equal(fixture.buffer->data + sizeof(dmi_header_t),
                        test_source + TEST_SOURCE_VALUES,
                        TEST_SOURCE_BODY - sizeof(dmi_header_t));
    assert_memory_equal(fixture.buffer->data + TEST_SOURCE_BODY, zeros, sizeof(zeros));

    test_encoder_close(&fixture);
}

//
// Preserve mode takes the strings of the structure over as they are
// numbered, so that a field keeps the number of the string it refers to.
//
static void test_encoder_string_preserve(void **pstate)
{
    dmi_context_t *context = *pstate;

    test_encoder_fixture_t fixture;
    test_encoder_open(context, &fixture, test_source, sizeof(test_source),
                      DMI_ENCODE_MODE_PRESERVE);

    dmi_encoder_t *encoder = &fixture.encoder;

    assert_uint_equal(encoder->string_count, 2);
    assert_string_equal(encoder->strings[0], " A ");
    assert_string_equal(encoder->strings[1], "B");

    assert_true(dmi_encoder_copy(encoder, TEST_SOURCE_STRING - sizeof(dmi_header_t)));
    assert_true(dmi_encoder_put_string(encoder, dmi_entity_string(fixture.entity, 2)));

    assert_int_equal(fixture.buffer->data[TEST_SOURCE_STRING], 2);
    assert_uint_equal(encoder->string_count, 2);

    test_encoder_close(&fixture);
}

//
// Reference to a string the structure does not have is kept as it is, since
// the model says nothing about the number it carried.
//
static void test_encoder_string_missing(void **pstate)
{
    dmi_context_t *context = *pstate;

    test_encoder_fixture_t fixture;
    test_encoder_open(context, &fixture, test_source, sizeof(test_source),
                      DMI_ENCODE_MODE_PRESERVE);

    dmi_encoder_t *encoder = &fixture.encoder;

    assert_true(dmi_encoder_copy(encoder, TEST_SOURCE_STRING + 1 - sizeof(dmi_header_t)));
    assert_true(dmi_encoder_put_string(encoder, nullptr));

    assert_int_equal(fixture.buffer->data[TEST_SOURCE_STRING + 1], 0x09);

    test_encoder_close(&fixture);
}

//
// Canonical mode numbers the strings as they are written, and the fields
// referring to the same text share a number.
//
static void test_encoder_string_canonical(void **pstate)
{
    dmi_context_t *context = *pstate;

    test_encoder_fixture_t fixture;
    test_encoder_open(context, &fixture, test_source, sizeof(test_source),
                      DMI_ENCODE_MODE_CANONICAL);

    dmi_encoder_t *encoder = &fixture.encoder;

    assert_uint_equal(encoder->string_count, 0);

    assert_true(dmi_encoder_put_string(encoder, "One"));
    assert_true(dmi_encoder_put_string(encoder, "Two"));
    assert_true(dmi_encoder_put_string(encoder, "One"));

    // Missing string has no number of its own to keep
    assert_true(dmi_encoder_put_string(encoder, nullptr));

    assert_uint_equal(encoder->string_count, 2);
    assert_int_equal(fixture.buffer->data[sizeof(dmi_header_t) + 0], 1);
    assert_int_equal(fixture.buffer->data[sizeof(dmi_header_t) + 1], 2);
    assert_int_equal(fixture.buffer->data[sizeof(dmi_header_t) + 2], 1);
    assert_int_equal(fixture.buffer->data[sizeof(dmi_header_t) + 3], 0);

    test_encoder_close(&fixture);
}

//
// Strings are written as they were stored, rather than as they are printed.
//
static void test_encoder_string_raw(void **pstate)
{
    dmi_context_t *context = *pstate;

    test_encoder_fixture_t fixture;
    test_encoder_open(context, &fixture, test_source, sizeof(test_source),
                      DMI_ENCODE_MODE_CANONICAL);

    dmi_encoder_t *encoder = &fixture.encoder;

    const char *pretty = dmi_entity_string(fixture.entity, 1);
    assert_string_equal(pretty, "A");

    assert_true(dmi_encoder_put_string(encoder, pretty));

    assert_uint_equal(encoder->string_count, 1);
    assert_string_equal(encoder->strings[0], " A ");

    test_encoder_close(&fixture);
}

//
// Formatted area is completed by filling in the length of the header.
//
static void test_encoder_finish(void **pstate)
{
    dmi_context_t *context = *pstate;

    test_encoder_fixture_t fixture;
    test_encoder_open(context, &fixture, test_source, sizeof(test_source),
                      DMI_ENCODE_MODE_CANONICAL);

    dmi_encoder_t *encoder = &fixture.encoder;

    assert_true(dmi_encoder_copy(encoder, 6));
    assert_true(dmi_encoder_finish(encoder));

    assert_int_equal(fixture.buffer->data[0x01], sizeof(dmi_header_t) + 6);

    test_encoder_close(&fixture);
}

static void test_encoder_finish_too_long(void **pstate)
{
    dmi_context_t *context = *pstate;

    test_encoder_fixture_t fixture;
    test_encoder_open(context, &fixture, test_source, sizeof(test_source),
                      DMI_ENCODE_MODE_CANONICAL);

    dmi_encoder_t *encoder = &fixture.encoder;

    assert_true(dmi_encoder_copy(encoder, UINT8_MAX));
    assert_false(dmi_encoder_finish(encoder));

    const dmi_error_t *error = dmi_error_get_last(context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_INVALID_ARGUMENT);

    test_encoder_close(&fixture);
}

//
// Header the source data has is kept whenever it reads as the one written:
// a header of no length stands for the end of the table, whatever type it
// names, and is written back as it was.
//
static void test_encoder_finish_end_of_table(void **pstate)
{
    dmi_context_t *context = *pstate;

    test_encoder_fixture_t fixture;
    test_encoder_open(context, &fixture, test_end_of_table, sizeof(test_end_of_table),
                      DMI_ENCODE_MODE_PRESERVE);

    assert_int_equal(fixture.entity->type, DMI_TYPE(END_OF_TABLE));
    assert_uint_equal(fixture.entity->body_length, sizeof(dmi_header_t));

    assert_true(dmi_encoder_finish(&fixture.encoder));
    assert_memory_equal(fixture.buffer->data, test_end_of_table, sizeof(dmi_header_t));

    test_encoder_close(&fixture);

    // Canonical mode has no source to keep, and writes the length the
    // structure takes
    test_encoder_open(context, &fixture, test_end_of_table, sizeof(test_end_of_table),
                      DMI_ENCODE_MODE_CANONICAL);

    assert_true(dmi_encoder_finish(&fixture.encoder));
    assert_int_equal(fixture.buffer->data[0x01], sizeof(dmi_header_t));

    test_encoder_close(&fixture);
}

//
// Strings an encoder holds are freed once it is finished with, and the data
// written belongs to the buffer it has been written into.
//
static void test_encoder_finalize(void **pstate)
{
    dmi_context_t *context = *pstate;

    test_encoder_fixture_t fixture;
    test_encoder_open(context, &fixture, test_source, sizeof(test_source),
                      DMI_ENCODE_MODE_PRESERVE);

    assert_uint_equal(fixture.encoder.string_count, 2);

    dmi_encoder_finalize(&fixture.encoder);

    assert_null(fixture.encoder.strings);
    assert_uint_equal(fixture.encoder.string_count, 0);
    assert_uint_equal(fixture.encoder.string_capacity, 0);

    // Encoder which has been finished with is finished with again safely
    dmi_encoder_finalize(&fixture.encoder);

    assert_uint_equal(fixture.buffer->length, sizeof(dmi_header_t));

    dmi_entity_destroy(fixture.entity);

    dmi_buffer_destroy(fixture.buffer);
    dmi_buffer_destroy(fixture.source);
}
