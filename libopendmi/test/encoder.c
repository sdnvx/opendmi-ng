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
static dmi_entity_t *test_decode(dmi_context_t *context, const uint8_t *data, size_t size)
{
    dmi_entity_t *entity = dmi_entity_create(context, data, size);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    return entity;
}

// Encode a decoded structure by the fields of its specification
static void test_encode(
        const dmi_entity_t *entity,
        dmi_encode_mode_t   mode,
        dmi_version_t       version,
        dmi_encoder_t      *encoder)
{
    assert_true(dmi_encoder_initialize(encoder, entity, mode, version));
    assert_true(dmi_fields_encode(encoder));
}

//
// Bits the specification reserves are kept in the preserve mode, since the
// model holds nothing of them, and cleared in the canonical one.
//
static void test_encoder_reserved_bits(void **pstate)
{
    dmi_context_t *context = *pstate;

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

    dmi_entity_t *entity = test_decode(context, data, sizeof(data));

    const dmi_power_supply_t *info = dmi_entity_info(entity, DMI_TYPE(POWER_SUPPLY));
    assert_non_null(info);
    assert_int_equal(info->type, 4);

    dmi_encoder_t encoder;

    test_encode(entity, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE, &encoder);
    assert_int_equal(encoder.length, 0x16);
    assert_memory_equal(encoder.data, data, 0x16);
    dmi_encoder_destroy(&encoder);

    test_encode(entity, DMI_ENCODE_MODE_CANONICAL, DMI_VERSION(3, 9, 0), &encoder);
    assert_int_equal(encoder.length, 0x16);
    assert_int_equal(encoder.data[0x0E], 0x93);
    assert_int_equal(encoder.data[0x0F], 0x11);
    dmi_encoder_destroy(&encoder);

    dmi_entity_destroy(entity);
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

    dmi_entity_t *entity = test_decode(context, test_array_addr_ex, sizeof(test_array_addr_ex));

    const dmi_memory_array_addr_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_ARRAY_ADDR));
    assert_non_null(info);
    assert_int_equal(info->start_addr, 0x0000010000000000uLL);
    assert_int_equal(info->end_addr,   0x000001FFFFFFFFFFuLL);

    dmi_encoder_t encoder;

    test_encode(entity, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE, &encoder);
    assert_int_equal(encoder.length, 0x1F);
    assert_memory_equal(encoder.data, test_array_addr_ex, 0x1F);
    dmi_encoder_destroy(&encoder);

    // Starting address fits the plain field, but the ending one does not, so
    // both are carried by the extended fields
    test_encode(entity, DMI_ENCODE_MODE_CANONICAL, DMI_VERSION(3, 9, 0), &encoder);
    assert_int_equal(encoder.length, 0x1F);
    assert_memory_equal(encoder.data, test_array_addr_ex, 0x1F);
    dmi_encoder_destroy(&encoder);

    dmi_entity_destroy(entity);
}

//
// Addresses which fit the plain fields are written to them in the canonical
// mode, and decoding the result gives back the same structure.
//
static void test_encoder_extended_canonical(void **pstate)
{
    dmi_context_t *context = *pstate;

    uint8_t data[sizeof(test_array_addr_ex)];
    memcpy(data, test_array_addr_ex, sizeof(data));

    // Extended ending address is a whole number of kilobytes
    data[0x17] = 0x00;
    data[0x18] = 0xFC;

    dmi_entity_t *entity = test_decode(context, data, sizeof(data));

    const dmi_memory_array_addr_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_ARRAY_ADDR));
    assert_non_null(info);

    dmi_encoder_t encoder;

    test_encode(entity, DMI_ENCODE_MODE_CANONICAL, DMI_VERSION(3, 9, 0), &encoder);
    assert_int_equal(encoder.length, 0x1F);

    // Plain fields carry the addresses in kilobytes, and the extended ones
    // are left zero
    static const uint8_t plain[] = { 0x00, 0x00, 0x00, 0x40, 0xFF, 0xFF, 0xFF, 0x7F };
    assert_memory_equal(encoder.data + 0x04, plain, sizeof(plain));

    for (size_t i = 0x0F; i < 0x1F; i++)
        assert_int_equal(encoder.data[i], 0x00);

    // Structure written this way decodes to the same addresses
    uint8_t encoded[0x1F + 2] = {};
    memcpy(encoded, encoder.data, 0x1F);
    dmi_encoder_destroy(&encoder);

    dmi_entity_t *decoded = test_decode(context, encoded, sizeof(encoded));

    const dmi_memory_array_addr_t *decoded_info = dmi_entity_info(decoded, DMI_TYPE(MEMORY_ARRAY_ADDR));
    assert_non_null(decoded_info);
    assert_int_equal(decoded_info->start_addr, info->start_addr);
    assert_int_equal(decoded_info->end_addr,   info->end_addr);

    dmi_entity_destroy(decoded);
    dmi_entity_destroy(entity);
}

//
// Data which ends in the middle of a field is kept as it is, since the model
// holds nothing of a field it has not read in full.
//
static void test_encoder_truncated(void **pstate)
{
    dmi_context_t *context = *pstate;

    // System information ending in the middle of the UUID
    static const uint8_t data[] = {
        1, 0x0D, 0x01, 0x00,
        0x01, 0x00, 0x00, 0x00,                     // Strings
        0x10, 0x32, 0x54, 0x76, 0x98,               // Beginning of the UUID
        'A', 0x00, 0x00
    };

    dmi_entity_t *entity = test_decode(context, data, sizeof(data));
    assert_true(entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    dmi_encoder_t encoder;

    test_encode(entity, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE, &encoder);
    assert_int_equal(encoder.length, 0x0D);
    assert_memory_equal(encoder.data, data, 0x0D);
    dmi_encoder_destroy(&encoder);

    dmi_entity_destroy(entity);
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

    dmi_entity_t *entity = test_decode(context, test_system_strings, sizeof(test_system_strings));

    dmi_encoder_t encoder;

    test_encode(entity, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE, &encoder);
    assert_int_equal(encoder.length, 0x1B);
    assert_memory_equal(encoder.data, test_system_strings, 0x1B);
    assert_int_equal(encoder.string_count, 4);
    assert_string_equal(encoder.strings[3], "unused");
    dmi_encoder_destroy(&encoder);

    test_encode(entity, DMI_ENCODE_MODE_CANONICAL, DMI_VERSION(3, 9, 0), &encoder);
    assert_int_equal(encoder.data[0x04], 1);
    assert_int_equal(encoder.data[0x05], 2);
    assert_int_equal(encoder.data[0x06], 1);
    assert_int_equal(encoder.data[0x07], 0);
    assert_int_equal(encoder.string_count, 2);
    assert_string_equal(encoder.strings[0], "A");
    assert_string_equal(encoder.strings[1], "B");
    dmi_encoder_destroy(&encoder);

    dmi_entity_destroy(entity);
}

//
// Canonical mode writes the groups of the fields the version it is given
// defines, and stops before the first group of a later one.
//
static void test_encoder_groups(void **pstate)
{
    dmi_context_t *context = *pstate;

    dmi_entity_t *entity = test_decode(context, test_system_strings, sizeof(test_system_strings));

    dmi_encoder_t encoder;

    test_encode(entity, DMI_ENCODE_MODE_CANONICAL, DMI_VERSION(2, 1, 0), &encoder);
    assert_int_equal(encoder.length, 0x19);
    assert_int_equal(encoder.data[0x01], 0x19);
    dmi_encoder_destroy(&encoder);

    test_encode(entity, DMI_ENCODE_MODE_CANONICAL, DMI_VERSION(2, 0, 0), &encoder);
    assert_int_equal(encoder.length, 0x08);
    dmi_encoder_destroy(&encoder);

    dmi_entity_destroy(entity);
}

//
// Compare two decoded structures member by member, as far as the fields of
// their specification describe them, which is the model the encoder writes
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
    dmi_encoder_t encoder;

    if (not dmi_encoder_initialize(&encoder, entity, DMI_ENCODE_MODE_CANONICAL, version))
        return nullptr;

    if (not dmi_entity_encode(&encoder)) {
        dmi_encoder_destroy(&encoder);
        return nullptr;
    }

    *size = encoder.length + 1;
    for (size_t i = 0; i < encoder.string_count; i++)
        *size += strlen(encoder.strings[i]) + 1;
    if (encoder.string_count == 0)
        (*size)++;

    dmi_byte_t *data = calloc(1, *size);
    if (data == nullptr) {
        dmi_encoder_destroy(&encoder);
        return nullptr;
    }

    memcpy(data, encoder.data, encoder.length);

    size_t position = encoder.length;
    for (size_t i = 0; i < encoder.string_count; i++) {
        size_t length = strlen(encoder.strings[i]) + 1;
        memcpy(data + position, encoder.strings[i], length);
        position += length;
    }

    dmi_encoder_destroy(&encoder);

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

    dmi_entity_t *decoded = dmi_entity_create(context, data, size);
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
    dmi_entity_t *entity = test_decode(context, data, size);

    dmi_encoder_t encoder;
    assert_true(dmi_encoder_initialize(&encoder, entity, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE));
    assert_true(dmi_entity_encode(&encoder));
    assert_int_equal(encoder.length, entity->body_length);
    assert_memory_equal(encoder.data, data, encoder.length);
    dmi_encoder_destroy(&encoder);

    size_t      first_size = 0;
    dmi_byte_t *first      = test_canonical_bytes(entity, DMI_VERSION(3, 9, 0), &first_size);
    assert_non_null(first);

    if (canonical_length != 0)
        assert_int_equal(first[1], canonical_length);

    dmi_entity_t *decoded = test_decode(context, first, first_size);

    size_t      second_size = 0;
    dmi_byte_t *second      = test_canonical_bytes(decoded, DMI_VERSION(3, 9, 0), &second_size);
    assert_non_null(second);
    assert_int_equal(second_size, first_size);
    assert_memory_equal(second, first, first_size);

    free(second);
    dmi_entity_destroy(decoded);
    free(first);
    dmi_entity_destroy(entity);
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
        // decode into as they are, and the encoder writes them back
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

            dmi_encoder_t encoder;
            assert_true(dmi_encoder_initialize(&encoder, entity, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE));

            if (not dmi_entity_encode(&encoder)) {
                const dmi_error_t *error = dmi_error_peek_last(context);

                print_error("%s: handle 0x%04X (%s) cannot be encoded: %s\n",
                            test_dumps[i], entity->handle, code,
                            ((error != nullptr) and (error->message != nullptr)) ? error->message : "");
                failed++;
                dmi_encoder_destroy(&encoder);
                continue;
            }

            const dmi_data_t *data = (entity->overlay_data != nullptr) ? entity->overlay_data : entity->data;

            bool matches = (encoder.length == entity->body_length) and
                           (memcmp(encoder.data, data, encoder.length) == 0) and
                           (encoder.string_count == entity->string_count);

            for (size_t k = 0; matches and (k < encoder.string_count); k++)
                matches = ((encoder.strings[k] == nullptr) and (entity->strings[k].raw == nullptr)) or
                          ((encoder.strings[k] != nullptr) and (entity->strings[k].raw != nullptr) and
                           (strcmp(encoder.strings[k], entity->strings[k].raw) == 0));

            dmi_encoder_destroy(&encoder);

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
