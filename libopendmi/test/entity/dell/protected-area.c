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
#include <opendmi/entity.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/module.h>
#include <opendmi/module/dell.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/dell/protected-area-1.h>
#include <opendmi/entity/dell/protected-area-2.h>

static int test_dell_protected_area_setup(void **pstate);
static int test_dell_protected_area_teardown(void **pstate);

static void test_dell_protected_area_1_decode(void **pstate);
static void test_dell_protected_area_2_decode(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_dell_protected_area_1_decode, test_dell_protected_area_setup, test_dell_protected_area_teardown),
        cmocka_unit_test_setup_teardown(test_dell_protected_area_2_decode, test_dell_protected_area_setup, test_dell_protected_area_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_dell_protected_area_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    if (not dmi_add_extension(context, dmi_module_find("dell"))) {
        dmi_destroy(context);
        return -1;
    }

    *pstate = context;

    return 0;
}

static int test_dell_protected_area_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_dell_protected_area_1_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const uint8_t data[] = {
        214, 0x11, 0x00, 0xD5,          // Header
        0x09, 0x00,                     // Token ID
        0x10, 0x01,                     // Value length and format
        0x34, 0x12,                     // Validation key
        0x72, 0x00, 0x73, 0x00,         // Index and data ports
        0x03, 0x40, 0x50,               // Checksum type, value and checksum indexes
        0x00, 0x00
    };

    dmi_buffer_t *entity_buffer = dmi_buffer_create(context);

    dmi_entity_t *entity = dmi_test_entity_create(entity_buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_dell_protected_area_1_t *info = dmi_entity_info(entity, DMI_TYPE(DELL_PROTECTED_AREA_1));
    assert_non_null(info);

    assert_int_equal(info->token_id, 0x0009);
    assert_int_equal(info->value_length, 0x10);
    assert_int_equal(info->value_format, DMI_DELL_VALUE_FORMAT_ASCII);
    assert_int_equal(info->validation_key, 0x1234);
    assert_int_equal(info->index_port, 0x72);
    assert_int_equal(info->data_port, 0x73);
    assert_int_equal(info->check_type, DMI_DELL_CHECK_TYPE_WORD_CHECKSUM_N);
    assert_int_equal(info->value_start, 0x40);
    assert_int_equal(info->check_index, 0x50);

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}

static void test_dell_protected_area_2_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const uint8_t data[] = {
        215, 0x15, 0x00, 0xD6,          // Header
        0x0A, 0x00,                     // Token ID
        0x20, 0x00,                     // Value length and format
        0x78, 0x56,                     // Validation key
        0x70, 0x00, 0x71, 0x00,         // Index and data ports
        0x01, 0x60, 0x80,               // Checksum type, value and checksum indexes
        0x02, 0x10, 0x7D, 0x7E,         // Range checksum type, range and index
        0x00, 0x00
    };

    dmi_buffer_t *entity_buffer = dmi_buffer_create(context);

    dmi_entity_t *entity = dmi_test_entity_create(entity_buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_dell_protected_area_2_t *info = dmi_entity_info(entity, DMI_TYPE(DELL_PROTECTED_AREA_2));
    assert_non_null(info);

    assert_int_equal(info->token_id, 0x000A);
    assert_int_equal(info->value_length, 0x20);
    assert_int_equal(info->value_format, DMI_DELL_VALUE_FORMAT_SCAN_CODE);
    assert_int_equal(info->validation_key, 0x5678);
    assert_int_equal(info->check_type, DMI_DELL_CHECK_TYPE_BYTE_CHECKSUM);
    assert_int_equal(info->value_start, 0x60);
    assert_int_equal(info->check_index, 0x80);
    assert_int_equal(info->range_check_type, DMI_DELL_CHECK_TYPE_WORD_CRC);
    assert_int_equal(info->range_check_start, 0x10);
    assert_int_equal(info->range_check_end, 0x7D);
    assert_int_equal(info->range_check_index, 0x7E);

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}
