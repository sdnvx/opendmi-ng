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
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/firmware.h>

static int test_firmware_setup(void **pstate);
static int test_firmware_teardown(void **pstate);

static void test_firmware_decode_v20(void **pstate);
static void test_firmware_decode_v21(void **pstate);
static void test_firmware_decode_v23(void **pstate);

static dmi_entity_t *test_firmware_create(dmi_buffer_t *buffer, uint8_t *data, uint8_t length);
static const dmi_attribute_t *test_firmware_attribute(const char *code);

static dmi_log_t test_logger = { dmi_test_log_handler };

// Firmware information structure. Structure length is set by the tests, and
// extension bytes beyond it are overwritten by strings.
static const uint8_t test_firmware_data[] = {
    0, 0x14, 0x00, 0x00,                            // Header
    0x01, 0x02, 0x00, 0xE0, 0x00, 0x00,             // Vendor, version, segment, date, ROM size
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Features: PCI support
    0x01, 0x08                                      // Extension bytes: ACPI and UEFI support
};

static const char test_firmware_strings[] = "Vendor\0" "1.0\0";

// Size of buffer for structure data and strings
#define TEST_FIRMWARE_SIZE (sizeof(test_firmware_data) + sizeof(test_firmware_strings))

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_firmware_decode_v20),
        cmocka_unit_test(test_firmware_decode_v21),
        cmocka_unit_test(test_firmware_decode_v23)
    };

    return cmocka_run_group_tests(tests, test_firmware_setup, test_firmware_teardown);
}

static int test_firmware_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_firmware_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_firmware_decode_v20(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);

    uint8_t data[TEST_FIRMWARE_SIZE];
    dmi_entity_t *entity = test_firmware_create(entity_buffer, data, 0x12);
    assert_non_null(entity);
    assert_int_equal(entity->level, DMI_VERSION(2, 0, 0));

    const dmi_firmware_t *info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE));
    assert_non_null(info);
    assert_true(info->features.pci_support);
    assert_false(info->features_ex.acpi_support);

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);

    // Features are available since SMBIOS 2.0, extended features are not
    const dmi_attribute_t *features    = test_firmware_attribute("features");
    const dmi_attribute_t *features_ex = test_firmware_attribute("features-ex");

    assert_true(features->params.level <= DMI_VERSION(2, 0, 0));
    assert_true(features_ex->params.level > DMI_VERSION(2, 0, 0));
}

static void test_firmware_decode_v21(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);

    uint8_t data[TEST_FIRMWARE_SIZE];
    dmi_entity_t *entity = test_firmware_create(entity_buffer, data, 0x13);
    assert_non_null(entity);
    assert_int_equal(entity->level, DMI_VERSION(2, 1, 0));

    const dmi_firmware_t *info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE));
    assert_non_null(info);
    assert_true(info->features.pci_support);
    assert_true(info->features_ex.acpi_support);
    assert_false(info->features_ex.uefi_spec);

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);

    // Extension byte 1 is available since SMBIOS 2.1
    const dmi_attribute_t *features_ex = test_firmware_attribute("features-ex");
    assert_true(features_ex->params.level <= DMI_VERSION(2, 1, 0));
}

static void test_firmware_decode_v23(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);

    // Extension byte 2 is available since SMBIOS 2.3
    uint8_t data[TEST_FIRMWARE_SIZE];
    dmi_entity_t *entity = test_firmware_create(entity_buffer, data, 0x14);
    assert_non_null(entity);
    assert_int_equal(entity->level, DMI_VERSION(2, 3, 0));

    const dmi_firmware_t *info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE));
    assert_non_null(info);
    assert_true(info->features_ex.acpi_support);
    assert_true(info->features_ex.uefi_spec);

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}

static dmi_entity_t *test_firmware_create(dmi_buffer_t *buffer, uint8_t *data, uint8_t length)
{
    memcpy(data, test_firmware_data, sizeof(test_firmware_data));
    data[1] = length;

    // Strings immediately follow the structure
    memcpy(data + length, test_firmware_strings, sizeof(test_firmware_strings));

    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, length + sizeof(test_firmware_strings));
    if (entity == nullptr)
        return nullptr;

    if (not dmi_entity_decode(entity)) {
        dmi_entity_destroy(entity);
        return nullptr;
    }

    return entity;
}

static const dmi_attribute_t *test_firmware_attribute(const char *code)
{
    for (const dmi_attribute_t *attr = dmi_firmware_spec.attributes; attr->params.name != nullptr; attr++) {
        if (strcmp(attr->params.code, code) == 0)
            return attr;
    }

    fail_msg("Attribute %s not found", code);

    return nullptr;
}
