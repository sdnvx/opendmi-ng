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
#include <opendmi/test/logger.h>

#include <opendmi/entity/tpm-device.h>

static void test_tpm_device_firmware_version(void **pstate);
static void test_tpm_device_vendor(void **pstate);

static dmi_entity_t *test_create(dmi_context_t *context, uint8_t *data, uint8_t major, uint8_t minor);
static dmi_entity_t *test_create_vendor(dmi_context_t *context, uint8_t *data, const char vendor[4]);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

// TPM device structure (0x1F bytes) followed by strings
#define TEST_TPM_DEVICE_SIZE (0x1F + 5)

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_tpm_device_firmware_version),
        cmocka_unit_test(test_tpm_device_vendor)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_tpm_device_firmware_version(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    uint8_t data[TEST_TPM_DEVICE_SIZE];

    // TPM 1.2: revision of TCPA_VERSION structure
    dmi_entity_t *entity = test_create(context, data, 1, 2);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_tpm_device_t *info = dmi_entity_info(entity, DMI_TYPE(TPM_DEVICE));
    assert_non_null(info);
    assert_int_equal(info->firmware_version_format, DMI_TPM_FIRMWARE_VERSION_FORMAT_TPM_1);
    assert_int_equal(info->firmware_revision, dmi_version(0x03, 0x14, 0));
    assert_string_equal(info->vendor_id, "IFX");

    dmi_entity_destroy(entity);

    // TPM 2.0: major and minor version in the first double word
    entity = test_create(context, data, 2, 0);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    info = dmi_entity_info(entity, DMI_TYPE(TPM_DEVICE));
    assert_non_null(info);
    assert_int_equal(info->firmware_version_format, DMI_TPM_FIRMWARE_VERSION_FORMAT_TPM_2);
    assert_int_equal(info->firmware_version_2.major, 0x1403);
    assert_int_equal(info->firmware_version_2.minor, 0x0201);
    assert_int_equal(info->firmware_version_2.vendor_specific, 0x00402E00);

    dmi_entity_destroy(entity);

    // Unknown TPM version: raw value
    entity = test_create(context, data, 3, 0);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    info = dmi_entity_info(entity, DMI_TYPE(TPM_DEVICE));
    assert_non_null(info);
    assert_int_equal(info->firmware_version_format, DMI_TPM_FIRMWARE_VERSION_FORMAT_RAW);
    assert_int_equal(info->firmware_version, 0x1403020100402E00);

    dmi_entity_destroy(entity);
    dmi_destroy(context);
}

static void test_tpm_device_vendor(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    static const struct {
        char        vendor[4];
        const char *expected;
    } test_cases[] = {
        { { 'I', 'N', 'T', 'C' }, "INTC"  },
        { { 'I', 'F', 'X', 0   }, "IFX"   },
        // Little-endian double word
        { { 0,   'X', 'F', 'I' }, "IFX"   },
        // Non-printable characters end the identifier
        { { 'A', 0x01, 'B', 'C' }, "A"    },
        { { 0,   0,   0,   0   }, nullptr }
    };

    uint8_t data[TEST_TPM_DEVICE_SIZE];

    for (size_t i = 0; i < countof(test_cases); i++) {
        dmi_entity_t *entity = test_create_vendor(context, data, test_cases[i].vendor);
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));

        const dmi_tpm_device_t *info = dmi_entity_info(entity, DMI_TYPE(TPM_DEVICE));
        assert_non_null(info);

        if (test_cases[i].expected != nullptr)
            assert_string_equal(info->vendor, test_cases[i].expected);
        else
            assert_null(info->vendor);

        dmi_entity_destroy(entity);
    }

    dmi_destroy(context);
}

static dmi_entity_t *test_create_vendor(dmi_context_t *context, uint8_t *data, const char vendor[4])
{
    dmi_entity_t *entity = test_create(context, data, 2, 0);
    if (entity == nullptr)
        return nullptr;

    dmi_entity_destroy(entity);

    // Structure data is referenced by entities, so it is changed before
    // creating the entity
    memcpy(data + 4, vendor, 4);

    return dmi_entity_create(context, data, TEST_TPM_DEVICE_SIZE);
}

static dmi_entity_t *test_create(dmi_context_t *context, uint8_t *data, uint8_t major, uint8_t minor)
{
    const uint8_t header[] = {
        43, 0x1F, 0x00, 0x30,               // Header
        'I', 'F', 'X', 0x00,                // Vendor ID
        major, minor,                       // Specification version
        0x01, 0x02, 0x03, 0x14,             // Firmware version 1
        0x00, 0x2E, 0x40, 0x00,             // Firmware version 2
        0x01,                               // Description
        0x10, 0x00, 0x00, 0x00,             // Characteristics
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00              // OEM-defined
    };

    memcpy(data, header, sizeof(header));
    memcpy(data + sizeof(header), "TPM\0\0", 5);

    return dmi_entity_create(context, data, TEST_TPM_DEVICE_SIZE);
}
