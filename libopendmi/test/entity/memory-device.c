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
#include <opendmi/test/logger.h>

#include <opendmi/entity/memory-device.h>

static void test_memory_device_type_name(void **pstate);
static void test_memory_device_form_factor_name(void **pstate);
static void test_memory_device_tech_name(void **pstate);
static void test_memory_device_size(void **pstate);
static void test_memory_device_size_ex(void **pstate);
static void test_memory_device_decode_size(void **pstate);
static void test_memory_device_decode_rank(void **pstate);
static void test_memory_device_decode_speed(void **pstate);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_memory_device_type_name),
        cmocka_unit_test(test_memory_device_form_factor_name),
        cmocka_unit_test(test_memory_device_tech_name),
        cmocka_unit_test(test_memory_device_size),
        cmocka_unit_test(test_memory_device_size_ex),
        cmocka_unit_test(test_memory_device_decode_size),
        cmocka_unit_test(test_memory_device_decode_rank),
        cmocka_unit_test(test_memory_device_decode_speed)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_memory_device_type_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_memory_device_type_name(__DMI_MEMORY_DEVICE_TYPE_COUNT));

    for (int i = 1; i < __DMI_MEMORY_DEVICE_TYPE_COUNT; i++) {
        if ((i >= __DMI_MEMORY_DEVICE_TYPE_UNASSIGNED_START) and
            (i <= __DMI_MEMORY_DEVICE_TYPE_UNASSIGNED_END))
            assert_null(dmi_memory_device_type_name(i));
        else
            assert_non_null(dmi_memory_device_type_name(i));
    }
}

static void test_memory_device_form_factor_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_memory_device_form_factor_name(__DMI_MEMORY_DEVICE_FORM_FACTOR_COUNT));

    for (int i = 1; i < __DMI_MEMORY_DEVICE_FORM_FACTOR_COUNT; i++) {
        assert_non_null(dmi_memory_device_form_factor_name(i));
    }
}

static void test_memory_device_tech_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_memory_device_tech_name(__DMI_MEMORY_DEVICE_TECH_COUNT));

    for (int i = 1; i < __DMI_MEMORY_DEVICE_TECH_COUNT; i++) {
        assert_non_null(dmi_memory_device_tech_name(i));
    }
}

static void test_memory_device_size(void **pstate)
{
    dmi_unused(pstate);

    for (size_t i = 0; i < 15; i++) {
        dmi_size_t base_size = (dmi_size_t)1u << i;

        assert_uint_equal(dmi_memory_device_size(1u << i), base_size * 1048576);
        assert_uint_equal(dmi_memory_device_size(0x8000u | (1u << i)), base_size * 1024);
    }
}

static void test_memory_device_size_ex(void **pstate)
{
    dmi_unused(pstate);

    for (size_t i = 0; i < 31; i++) {
        dmi_size_t base_size = (dmi_size_t)1u << i;

        assert_uint_equal(dmi_memory_device_size_ex(1u << i), base_size * 1048576);
        assert_uint_equal(dmi_memory_device_size_ex(0x80000000u | (1u << i)), DMI_SIZE_MAX);
    }
}

static void decode_memory_device(
        dmi_context_t       *context,
        uint16_t             size,
        uint32_t             size_ex,
        uint8_t              rank,
        uint8_t              length,
        dmi_memory_device_t *result)
{
    // SMBIOS 2.7 memory device structure, followed by a single string
    uint8_t data[] = {
        17, length, 0x00, 0x10,             // Header
        0x00, 0x01, 0xFE, 0xFF,             // Array handle, error info handle
        0x40, 0x00, 0x40, 0x00,             // Total width, data width
        size & 0xFF, size >> 8,             // Size
        0x09, 0x00, 0x01, 0x00,             // Form factor, device set, locators
        0x1A, 0x80, 0x00, 0x40, 0x06,       // Memory type, type detail, speed
        0x00, 0x00, 0x00, 0x00, rank,       // Strings, rank
        size_ex & 0xFF, (size_ex >> 8) & 0xFF,
        (size_ex >> 16) & 0xFF, size_ex >> 24,
        'D', 'I', 'M', 'M', 0, 0
    };

    // Move string set right after the structure
    memmove(data + length, data + 0x20, 6);

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_memory_device_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_DEVICE));
    assert_non_null(info);

    // Copy numeric fields only, string pointers are owned by the entity
    *result = *info;
    dmi_entity_destroy(entity);
}

static dmi_size_t decode_memory_device_size(dmi_context_t *context, uint16_t size, uint32_t size_ex, uint8_t length)
{
    dmi_memory_device_t info;

    decode_memory_device(context, size, size_ex, 0x02, length, &info);

    return info.size;
}

static void test_memory_device_decode_size(void **pstate)
{
    dmi_unused(pstate);

    const dmi_size_t mib = 1048576;

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    // Size field only
    assert_uint_equal(decode_memory_device_size(context, 0x4000, 0, 0x20), 16384 * mib);
    assert_uint_equal(decode_memory_device_size(context, 0x8100, 0, 0x20), 256 * 1024);
    assert_uint_equal(decode_memory_device_size(context, 0xFFFF, 0, 0x20), DMI_SIZE_MAX);

    // Extended size field is used for 32 GiB and larger devices
    assert_uint_equal(decode_memory_device_size(context, 0x7FFF, 0x8000, 0x20), 32768 * mib);
    assert_uint_equal(decode_memory_device_size(context, 0x7FFF, 0x10000, 0x20), 65536 * mib);
    assert_uint_equal(decode_memory_device_size(context, 0x7FFF, 0x100000, 0x20), 1048576 * mib);

    // Invalid extended size is treated as unknown
    assert_uint_equal(decode_memory_device_size(context, 0x7FFF, 0x80010000, 0x20), DMI_SIZE_MAX);

    // Extended size field is ignored if size field is not 0x7FFF
    assert_uint_equal(decode_memory_device_size(context, 0x2000, 0x10000, 0x20), 8192 * mib);

    // Extended size field is not present in shorter structures
    assert_uint_equal(decode_memory_device_size(context, 0x7FFF, 0x10000, 0x1C), 32767 * mib);
    assert_uint_equal(decode_memory_device_size(context, 0x7FFF, 0x10000, 0x1E), 32767 * mib);

    dmi_destroy(context);
}

static unsigned short decode_memory_device_rank(dmi_context_t *context, uint8_t rank)
{
    dmi_memory_device_t info;

    decode_memory_device(context, 0x4000, 0, rank, 0x20, &info);

    return info.rank;
}

static void test_memory_device_decode_rank(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    assert_int_equal(decode_memory_device_rank(context, 0x00), 0);
    assert_int_equal(decode_memory_device_rank(context, 0x01), 1);
    assert_int_equal(decode_memory_device_rank(context, 0x04), 4);
    assert_int_equal(decode_memory_device_rank(context, 0x08), 8);
    assert_int_equal(decode_memory_device_rank(context, 0x0F), 15);

    // Reserved bits are ignored
    assert_int_equal(decode_memory_device_rank(context, 0xF2), 2);
    assert_int_equal(decode_memory_device_rank(context, 0x18), 8);

    dmi_destroy(context);
}

static void test_memory_device_decode_speed(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    // Speed field is present without the following SMBIOS 2.3 strings, as
    // many SMBIOS 2.3 implementations do
    for (uint8_t length = 0x15; length <= 0x17; length++) {
        uint8_t data[] = {
            17, length, 0x00, 0x10,
            0x00, 0x01, 0xFE, 0xFF, 0x40, 0x00, 0x40, 0x00,
            0x00, 0x40, 0x09, 0x00, 0x00, 0x00, 0x1A, 0x80, 0x00,
            0x15, 0x02,
            0x00, 0x00
        };

        // String set terminator right after the structure
        data[length]     = 0;
        data[length + 1] = 0;

        dmi_entity_t *entity = dmi_entity_create(context, data, length + 2);
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));

        const dmi_memory_device_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_DEVICE));
        assert_non_null(info);

        assert_int_equal(entity->level, (length > 0x15) ? DMI_VERSION(2, 3, 0) : DMI_VERSION(2, 1, 0));
        assert_int_equal(info->maximum_speed, (length == 0x17) ? 533 : 0);

        // Only partially present speed field makes the structure incomplete
        assert_int_equal((entity->state & DMI_ENTITY_STATE_INCOMPLETE) != 0, length == 0x16);
        assert_int_equal((entity->state & DMI_ENTITY_STATE_PARTIAL) != 0, length != 0x16);

        dmi_entity_destroy(entity);
    }

    dmi_destroy(context);
}
