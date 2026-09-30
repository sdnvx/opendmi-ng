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
#include <opendmi/module/intel-rsd.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/intel-rsd/storage-device.h>

static int test_rsd_storage_device_setup(void **pstate);
static int test_rsd_storage_device_teardown(void **pstate);

static void test_rsd_storage_device_decode(void **pstate);
static void test_rsd_storage_device_decode_short(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

// Intel RSD storage device information structure (0x1B bytes) followed by strings
static const uint8_t test_data[] = {
    194, 0x1B, 0x00, 0x31,          // Header
    0x01,                           // Port designation
    0x02,                           // Device index
    0x03, 0x03, 0x02,               // Connector (PCIe), protocol (NVMe), type (SSD)
    0xE8, 0x03, 0x00, 0x00,         // Capacity (1000 GB)
    0x00, 0x00,                     // RPM
    0x02, 0x03,                     // Model, serial number
    0x01,                           // PCI class
    0x86, 0x80, 0x53, 0x09,         // Vendor ID, device ID
    0x86, 0x80, 0x70, 0x37,         // Sub-vendor ID, sub-device ID
    0x04,                           // Firmware version
    'P', 'C', 'I', 'e', ' ', 'P', 'o', 'r', 't', ' ', '1', 0,
    'S', 'S', 'D', 'P', 'E', '2', 'K', 'X', '0', '1', '0', 'T', '8', 0,
    'P', 'H', 'L', 'J', '0', '0', '0', '1', 0,
    'V', 'D', 'V', '1', '0', '1', '3', '1', 0,
    0
};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_rsd_storage_device_decode,
                                        test_rsd_storage_device_setup, test_rsd_storage_device_teardown),
        cmocka_unit_test_setup_teardown(test_rsd_storage_device_decode_short,
                                        test_rsd_storage_device_setup, test_rsd_storage_device_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_rsd_storage_device_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    if (not dmi_add_extension(context, dmi_module_find("intel-rsd"))) {
        dmi_destroy(context);
        return -1;
    }

    *pstate = context;

    return 0;
}

static int test_rsd_storage_device_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_rsd_storage_device_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    dmi_buffer_t *entity_buffer = dmi_buffer_create(context);

    dmi_entity_t *entity = dmi_test_entity_create(entity_buffer, test_data, sizeof(test_data));
    assert_non_null(entity);
    assert_int_equal(entity->body_length, 0x1B);
    assert_true(dmi_entity_decode(entity));

    const dmi_intel_rsd_storage_device_t *info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_STORAGE_DEVICE));
    assert_non_null(info);

    assert_string_equal(info->port, "PCIe Port 1");
    assert_int_equal(info->index, 2);
    assert_int_equal(info->connector, DMI_INTEL_RSD_STORAGE_CONNECTOR_PCIE);
    assert_int_equal(info->protocol, DMI_INTEL_RSD_STORAGE_PROTO_NVME);
    assert_int_equal(info->type, DMI_INTEL_RSD_STORAGE_DEVICE_TYPE_SSD);
    assert_int_equal(info->capacity, 1000);
    assert_int_equal(info->rpm, 0);
    assert_string_equal(info->model, "SSDPE2KX010T8");
    assert_string_equal(info->serial_number, "PHLJ0001");
    assert_int_equal(info->pci_class, 0x01);
    assert_int_equal(info->vendor_id, 0x8086);
    assert_int_equal(info->device_id, 0x0953);
    assert_int_equal(info->sub_vendor_id, 0x8086);
    assert_int_equal(info->sub_device_id, 0x3770);
    assert_string_equal(info->firmware_version, "VDV10131");

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}

//
// Structure one byte shorter than its minimum length is rejected.
//
static void test_rsd_storage_device_decode_short(void **pstate)
{
    dmi_context_t *context = *pstate;

    uint8_t data[0x1A + 2] = {};
    memcpy(data, test_data, 0x1A);
    data[1] = 0x1A;

    dmi_buffer_t *entity_buffer = dmi_buffer_create(context);

    dmi_entity_t *entity = dmi_test_entity_create(entity_buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_false(dmi_entity_decode(entity));

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}
