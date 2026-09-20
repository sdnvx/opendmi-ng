//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/battery.h>

static int test_battery_setup(void **pstate);
static int test_battery_teardown(void **pstate);

static void test_battery_chemistry_name(void **pstate);
static void test_battery_decode_v21(void **pstate);
static void test_battery_decode_v22(void **pstate);
static void test_battery_decode_short(void **pstate);
static void test_battery_decode_incomplete(void **pstate);

static dmi_entity_t *test_battery_create(dmi_context_t *context, uint8_t *data, uint8_t length);

static dmi_log_t test_logger = { dmi_test_log_handler };

// Portable battery structure (SMBIOS 2.2). Structure length is set by the
// tests, and fields beyond it are overwritten by strings.
static const uint8_t test_battery_data[] = {
    22, 0x1A, 0x00, 0x20,                   // Header
    0x01, 0x02, 0x00, 0x03, 0x04,           // Location, vendor, date, serial number, name
    0x03, 0x64, 0x00, 0xE0, 0x2E,           // Chemistry, capacity, voltage
    0x00, 0xFF,                             // SBDS version, maximum error
    0x34, 0x12, 0xB1, 0x50,                 // SBDS serial number and manufacture date
    0x00, 0x0A,                             // SBDS chemistry, capacity factor
    0xEF, 0xBE, 0xAD, 0xDE                  // OEM-specific
};

static const char test_battery_strings[] = "Rear\0" "ACME\0" "SN1\0" "Battery\0";

// Size of buffer for structure data and strings
#define TEST_BATTERY_SIZE (sizeof(test_battery_data) + sizeof(test_battery_strings))

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_battery_chemistry_name),
        cmocka_unit_test(test_battery_decode_v21),
        cmocka_unit_test(test_battery_decode_v22),
        cmocka_unit_test(test_battery_decode_short),
        cmocka_unit_test(test_battery_decode_incomplete)
    };

    return cmocka_run_group_tests(tests, test_battery_setup, test_battery_teardown);
}

static int test_battery_setup(void **pstate)
{
    dmi_context_t *context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_battery_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_battery_chemistry_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_battery_chemistry_name(__DMI_BATTERY_CHEMISTRY_COUNT));

    for (int i = 0; i < __DMI_BATTERY_CHEMISTRY_COUNT; i++) {
        assert_non_null(dmi_battery_chemistry_name(i));
    }
}

static void test_battery_decode_v21(void **pstate)
{
    dmi_context_t *context = *pstate;

    uint8_t data[TEST_BATTERY_SIZE];
    dmi_entity_t *entity = test_battery_create(context, data, 0x10);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_int_equal(entity->level, DMI_VERSION(2, 1, 0));
    assert_false(entity->state & DMI_ENTITY_STATE_INCOMPLETE);
    assert_true(entity->state & DMI_ENTITY_STATE_PARTIAL);

    const dmi_battery_t *info = dmi_entity_info(entity, DMI_TYPE(PORTABLE_BATTERY));
    assert_non_null(info);
    assert_string_equal(info->location, "Rear");
    assert_string_equal(info->vendor, "ACME");
    assert_string_equal(info->serial_number, "SN1");
    assert_string_equal(info->name, "Battery");
    assert_int_equal(info->chemistry, 0x03);
    assert_int_equal(info->capacity, 100);
    assert_int_equal(info->voltage, 12000);
    assert_int_equal(info->maximum_error, USHRT_MAX);
    assert_int_equal(info->oem_defined, 0);

    dmi_entity_destroy(entity);
}

static void test_battery_decode_v22(void **pstate)
{
    dmi_context_t *context = *pstate;

    uint8_t data[TEST_BATTERY_SIZE];
    dmi_entity_t *entity = test_battery_create(context, data, 0x1A);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_int_equal(entity->level, DMI_VERSION(2, 2, 0));
    assert_false(entity->state & DMI_ENTITY_STATE_INCOMPLETE);
    assert_false(entity->state & DMI_ENTITY_STATE_PARTIAL);

    const dmi_battery_t *info = dmi_entity_info(entity, DMI_TYPE(PORTABLE_BATTERY));
    assert_non_null(info);

    // Capacity is multiplied, and manufacture date is taken from SBDS field
    assert_int_equal(info->capacity, 1000);
    assert_int_equal(info->sbds_serial_number, 0x1234);
    assert_int_equal(info->manufacture_date, dmi_date(2020, 5, 17));
    assert_int_equal(info->oem_defined, 0xDEADBEEF);

    dmi_entity_destroy(entity);
}

static void test_battery_decode_short(void **pstate)
{
    dmi_context_t *context = *pstate;

    // SMBIOS 2.1 fields are mandatory
    uint8_t data[TEST_BATTERY_SIZE];
    dmi_entity_t *entity = test_battery_create(context, data, 0x0F);
    assert_non_null(entity);
    assert_false(dmi_entity_decode(entity));

    dmi_entity_destroy(entity);
}

static void test_battery_decode_incomplete(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Only completely present SMBIOS 2.2 fields are decoded
    uint8_t data[TEST_BATTERY_SIZE];
    dmi_entity_t *entity = test_battery_create(context, data, 0x15);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_int_equal(entity->level, DMI_VERSION(2, 2, 0));
    assert_true(entity->state & DMI_ENTITY_STATE_INCOMPLETE);
    assert_false(entity->state & DMI_ENTITY_STATE_PARTIAL);

    const dmi_battery_t *info = dmi_entity_info(entity, DMI_TYPE(PORTABLE_BATTERY));
    assert_non_null(info);
    assert_int_equal(info->sbds_serial_number, 0x1234);
    assert_int_equal(info->manufacture_date, dmi_date(2020, 5, 17));

    // Capacity factor is not present
    assert_int_equal(info->capacity, 100);

    dmi_entity_destroy(entity);
}

static dmi_entity_t *test_battery_create(dmi_context_t *context, uint8_t *data, uint8_t length)
{
    memcpy(data, test_battery_data, sizeof(test_battery_data));
    data[1] = length;

    // Strings immediately follow the structure
    memcpy(data + length, test_battery_strings, sizeof(test_battery_strings));

    return dmi_entity_create(context, data, length + sizeof(test_battery_strings));
}
