//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/cache.h>

static void test_cache_type_name(void **pstate);
static void test_cache_assoc_name(void **pstate);
static void test_cache_location_name(void **pstate);
static void test_cache_mode_name(void **pstate);
static void test_cache_size(void **pstate);
static void test_cache_size_ex(void **pstate);
static void test_cache_decode_size(void **pstate);
static void test_cache_decode_v21(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_cache_type_name),
        cmocka_unit_test(test_cache_assoc_name),
        cmocka_unit_test(test_cache_location_name),
        cmocka_unit_test(test_cache_mode_name),
        cmocka_unit_test(test_cache_size),
        cmocka_unit_test(test_cache_size_ex),
        cmocka_unit_test(test_cache_decode_size),
        cmocka_unit_test(test_cache_decode_v21)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_cache_type_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_cache_type_name(__DMI_CACHE_TYPE_COUNT));

    for (int i = 0; i < __DMI_CACHE_TYPE_COUNT; i++) {
        assert_non_null(dmi_cache_type_name(i));
    }
}

static void test_cache_assoc_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_cache_assoc_name(__DMI_CACHE_ASSOC_COUNT));

    for (int i = 0; i < __DMI_CACHE_ASSOC_COUNT; i++) {
        assert_non_null(dmi_cache_assoc_name(i));
    }
}

static void test_cache_location_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_cache_location_name(__DMI_CACHE_LOCATION_COUNT));

    for (int i = 1; i < __DMI_CACHE_LOCATION_COUNT; i++) {
        assert_non_null(dmi_cache_location_name(i));
    }
}

static void test_cache_mode_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_cache_mode_name(__DMI_CACHE_MODE_COUNT));

    for (int i = 1; i < __DMI_CACHE_MODE_COUNT; i++) {
        assert_non_null(dmi_cache_mode_name(i));
    }
}

static void test_cache_size(void **pstate)
{
    dmi_unused(pstate);

    for (size_t i = 0; i < 15; i++) {
        dmi_size_t base_size = (dmi_size_t)1u << i;

        assert_uint_equal(dmi_cache_size(1u << i), base_size * 1024);
        assert_uint_equal(dmi_cache_size(0x8000u | (1u << i)), base_size * 65536);
    }
}

static void test_cache_size_ex(void **pstate)
{
    dmi_unused(pstate);

    for (size_t i = 0; i < 31; i++) {
        size_t base_size = (dmi_size_t)1u << i;

        assert_uint_equal(dmi_cache_size_ex(1u << i), base_size * 1024);
        assert_uint_equal(dmi_cache_size_ex(0x80000000u | (1u << i)), base_size * 65536);
    }
}

static void decode_cache_size(
        dmi_context_t *context,
        uint16_t       maximum_size,
        uint16_t       installed_size,
        uint32_t       maximum_size_ex,
        uint32_t       installed_size_ex,
        uint8_t        length,
        dmi_cache_t   *result,
        unsigned int  *state)
{
    // SMBIOS 3.1 cache information structure without strings
    uint8_t data[0x1B + 2] = {
        7, length, 0x00, 0x10,              // Header
        0x00, 0x80, 0x01,                   // Socket designation, configuration
        maximum_size & 0xFF, maximum_size >> 8,
        installed_size & 0xFF, installed_size >> 8,
        0x20, 0x00, 0x20, 0x00,             // Supported and current SRAM type
        0x00, 0x06, 0x05, 0x07              // Speed, ECC, type, associativity
    };

    for (size_t i = 0; i < 4; i++) {
        data[0x13 + i] = (maximum_size_ex >> (8 * i)) & 0xFF;
        data[0x17 + i] = (installed_size_ex >> (8 * i)) & 0xFF;
    }

    // String set terminator right after the structure
    data[length]     = 0;
    data[length + 1] = 0;

    dmi_entity_t *entity = dmi_entity_create(context, data, length + 2);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_cache_t *info = dmi_entity_info(entity, DMI_TYPE(CACHE));
    assert_non_null(info);

    // Copy numeric fields only, string pointers are owned by the entity
    *result = *info;
    *state  = entity->state;
    dmi_entity_destroy(entity);
}

static void test_cache_decode_size(void **pstate)
{
    dmi_unused(pstate);

    const dmi_size_t kib = 1024;
    const dmi_size_t mib = 1024 * kib;
    const dmi_size_t gib = 1024 * mib;

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    dmi_cache_t info;
    unsigned int state;

    // Legacy size fields only
    decode_cache_size(context, 0x0100, 0x0080, 0, 0, 0x1B, &info, &state);
    assert_uint_equal(info.maximum_size, 256 * kib);
    assert_uint_equal(info.installed_size, 128 * kib);

    // Extended size fields (in 64 KiB granularity) are used for 2 GiB and larger caches
    decode_cache_size(context, 0xFFFF, 0xFFFF, 0x80010000, 0x80008000, 0x1B, &info, &state);
    assert_uint_equal(info.maximum_size, 4 * gib);
    assert_uint_equal(info.installed_size, 2 * gib);

    // Extended size fields in 1 KiB granularity
    decode_cache_size(context, 0xFFFF, 0xFFFF, 0x00400000, 0x00200000, 0x1B, &info, &state);
    assert_uint_equal(info.maximum_size, 4 * gib);
    assert_uint_equal(info.installed_size, 2 * gib);

    // Extended size fields are ignored unless legacy field is 0xFFFF
    decode_cache_size(context, 0x0100, 0xFFFF, 0x80010000, 0x80008000, 0x1B, &info, &state);
    assert_uint_equal(info.maximum_size, 256 * kib);
    assert_uint_equal(info.installed_size, 2 * gib);

    decode_cache_size(context, 0xFFFF, 0x0080, 0x80010000, 0x80008000, 0x1B, &info, &state);
    assert_uint_equal(info.maximum_size, 4 * gib);
    assert_uint_equal(info.installed_size, 128 * kib);

    assert_false(state & DMI_ENTITY_STATE_INCOMPLETE);

    // Only completely present extended fields are used, and the structure is
    // marked as incomplete
    decode_cache_size(context, 0xFFFF, 0xFFFF, 0x80010000, 0x80008000, 0x17, &info, &state);
    assert_uint_equal(info.maximum_size, 4 * gib);
    assert_uint_equal(info.installed_size, dmi_cache_size(0xFFFF));
    assert_true(state & DMI_ENTITY_STATE_INCOMPLETE);

    decode_cache_size(context, 0xFFFF, 0xFFFF, 0x80010000, 0x80008000, 0x15, &info, &state);
    assert_uint_equal(info.maximum_size, dmi_cache_size(0xFFFF));
    assert_uint_equal(info.installed_size, dmi_cache_size(0xFFFF));
    assert_true(state & DMI_ENTITY_STATE_INCOMPLETE);

    dmi_destroy(context);
}

static void test_cache_decode_v21(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    for (uint8_t length = 0x0F; length <= 0x13; length++) {
        // Cache information structure followed by a single string, which must
        // not be interpreted as SMBIOS 2.1 fields in shorter structures
        uint8_t data[0x13 + 4] = {
            7, length, 0x00, 0x10,          // Header
            0x01, 0x80, 0x01,               // Socket designation, configuration
            0x00, 0x01, 0x00, 0x01,         // Maximum and installed size
            0x20, 0x00, 0x20, 0x00,         // Supported and current SRAM type
            0x0A, 0x06, 0x05, 0x07          // Speed, ECC, type, associativity
        };

        memcpy(data + length, "L1\0\0", 4);

        dmi_entity_t *entity = dmi_entity_create(context, data, length + 4);
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));

        const dmi_cache_t *info = dmi_entity_info(entity, DMI_TYPE(CACHE));
        assert_non_null(info);
        assert_string_equal(info->socket_designator, "L1");

        // Incomplete SMBIOS 2.1 fields make the structure incomplete
        bool incomplete = (length > 0x0F) and (length < 0x13);
        assert_int_equal((entity->state & DMI_ENTITY_STATE_INCOMPLETE) != 0, incomplete);

        // Complete SMBIOS 2.0 and 2.1 structures do not contain SMBIOS 3.1 fields
        assert_int_equal((entity->state & DMI_ENTITY_STATE_PARTIAL) != 0, not incomplete);

        // Only completely present SMBIOS 2.1 fields are decoded
        size_t fields = length - 0x0F;

        assert_int_equal(entity->level, (fields > 0) ? DMI_VERSION(2, 1, 0) : DMI_VERSION(2, 0, 0));
        assert_int_equal(info->speed, (fields > 0) ? 0x0A : 0);
        assert_int_equal(info->error_correction, (fields > 1) ? 0x06 : 0);
        assert_int_equal(info->type, (fields > 2) ? 0x05 : 0);
        assert_int_equal(info->associativity, (fields > 3) ? 0x07 : 0);

        dmi_entity_destroy(entity);
    }

    dmi_destroy(context);
}
