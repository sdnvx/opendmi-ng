//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/log.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/memory-array.h>

static void test_memory_array_location_name(void **pstate);
static void test_memory_array_usage_name(void **pstate);
static void test_memory_array_decode_capacity(void **pstate);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_memory_array_location_name),
        cmocka_unit_test(test_memory_array_usage_name),
        cmocka_unit_test(test_memory_array_decode_capacity)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_memory_array_location_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_memory_array_location_name(__DMI_MEMORY_ARRAY_LOCATION_COUNT));

    for (int i = 0; i < __DMI_MEMORY_ARRAY_LOCATION_COUNT; i++) {
        if ((i >= __DMI_MEMORY_ARRAY_LOCATION_UNASSIGNED_START) and
            (i <= __DMI_MEMORY_ARRAY_LOCATION_UNASSIGNED_END))
            assert_null(dmi_memory_array_location_name(i));
        else
            assert_non_null(dmi_memory_array_location_name(i));
    }
}

static void test_memory_array_usage_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_memory_array_usage_name(__DMI_MEMORY_ARRAY_USAGE_COUNT));

    for (int i = 0; i < __DMI_MEMORY_ARRAY_USAGE_COUNT; i++) {
        assert_non_null(dmi_memory_array_usage_name(i));
    }
}

static dmi_size_t decode_memory_array_capacity(
        dmi_context_t *context,
        uint32_t       capacity,
        uint64_t       capacity_ex,
        uint8_t        length)
{
    // SMBIOS 2.7 physical memory array structure without strings
    uint8_t data[0x17 + 2] = {
        16, length, 0x00, 0x10,             // Header
        0x03, 0x03, 0x06,                   // Location, use, error correction
        capacity & 0xFF, (capacity >> 8) & 0xFF,
        (capacity >> 16) & 0xFF, capacity >> 24,
        0xFE, 0xFF, 0x04, 0x00              // Error info handle, device count
    };

    for (size_t i = 0; i < 8; i++)
        data[0x0F + i] = (capacity_ex >> (8 * i)) & 0xFF;

    // String set terminator right after the structure
    data[length]     = 0;
    data[length + 1] = 0;

    dmi_entity_t *entity = dmi_entity_create(context, data, length + 2);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_memory_array_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_ARRAY));
    assert_non_null(info);

    dmi_size_t result = info->maximum_capacity;
    dmi_entity_destroy(entity);

    return result;
}

static void test_memory_array_decode_capacity(void **pstate)
{
    dmi_unused(pstate);

    const dmi_size_t gib = 1073741824;

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    // Capacity in kilobytes, including values with bit 21 set
    assert_uint_equal(decode_memory_array_capacity(context, 0x00400000, 0, 0x17), 4 * gib);
    assert_uint_equal(decode_memory_array_capacity(context, 0x00200000, 0, 0x17), 2 * gib);
    assert_uint_equal(decode_memory_array_capacity(context, 0x00600000, 0, 0x17), 6 * gib);
    assert_uint_equal(decode_memory_array_capacity(context, 0x7FFFFFFF, 0, 0x17), 0x7FFFFFFFull << 10);

    // Extended capacity is used only if capacity field is 0x80000000
    assert_uint_equal(decode_memory_array_capacity(context, 0x80000000, 4096 * gib, 0x17), 4096 * gib);
    assert_uint_equal(decode_memory_array_capacity(context, 0x00400000, 4096 * gib, 0x17), 4 * gib);

    // Capacity is unknown if extended field is not present
    assert_uint_equal(decode_memory_array_capacity(context, 0x80000000, 4096 * gib, 0x0F), UINT64_MAX);
    assert_uint_equal(decode_memory_array_capacity(context, 0x80000000, 4096 * gib, 0x13), UINT64_MAX);
    assert_uint_equal(decode_memory_array_capacity(context, 0x00400000, 4096 * gib, 0x0F), 4 * gib);

    dmi_destroy(context);
}
