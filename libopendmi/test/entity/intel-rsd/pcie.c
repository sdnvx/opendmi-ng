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

#include <opendmi/entity/intel-rsd/pcie.h>

static int test_rsd_pcie_setup(void **pstate);
static int test_rsd_pcie_teardown(void **pstate);

static void test_rsd_pcie_decode(void **pstate);
static void test_rsd_pcie_decode_short(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

// Intel RSD PCIe information structure (0x17 bytes) with no strings
static const uint8_t test_data[] = {
    192, 0x17, 0x00, 0x30,          // Header
    0x01,                           // PCI class
    0x05, 0x00,                     // Slot number
    0x86, 0x80, 0x53, 0x09,         // Vendor ID, device ID (Intel P3700)
    0x86, 0x80, 0x02, 0x37,         // Sub-vendor ID, sub-device ID
    0x03, 0x00, 0x00, 0x00,         // Link speed
    0x04, 0x00, 0x00, 0x00,         // Link width
    0, 0
};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_rsd_pcie_decode,
                                        test_rsd_pcie_setup, test_rsd_pcie_teardown),
        cmocka_unit_test_setup_teardown(test_rsd_pcie_decode_short,
                                        test_rsd_pcie_setup, test_rsd_pcie_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_rsd_pcie_setup(void **pstate)
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

static int test_rsd_pcie_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_rsd_pcie_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, test_data, sizeof(test_data));

    assert_non_null(entity);
    assert_int_equal(entity->body_length, 0x17);
    assert_true(dmi_entity_decode(entity));

    const dmi_intel_rsd_pcie_t *info = dmi_entity_info(entity, DMI_TYPE(intel_rsd_pcie));
    assert_non_null(info);

    assert_int_equal(info->pci_class, 0x01);
    assert_int_equal(info->pci_slot_id, 5);
    assert_int_equal(info->vendor_id, 0x8086);
    assert_int_equal(info->device_id, 0x0953);
    assert_int_equal(info->sub_vendor_id, 0x8086);
    assert_int_equal(info->sub_device_id, 0x3702);
    assert_int_equal(info->link_speed, 3);
    assert_int_equal(info->link_width, 4);

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
}

//
// Structure one byte shorter than its minimum length is rejected.
//
static void test_rsd_pcie_decode_short(void **pstate)
{
    dmi_context_t *context = *pstate;

    uint8_t data[0x16 + 2] = {};
    memcpy(data, test_data, 0x16);
    data[1] = 0x16;

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));

    assert_non_null(entity);
    assert_false(dmi_entity_decode(entity));

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
}
