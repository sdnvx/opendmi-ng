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
#include <opendmi/module/intel.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/intel/rsd-fpga.h>

static int test_rsd_fpga_setup(void **pstate);
static int test_rsd_fpga_teardown(void **pstate);

static void test_rsd_fpga_decode(void **pstate);
static void test_rsd_fpga_decode_short(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

// Intel RSD FPGA information structure (0x24 bytes) followed by strings
static const uint8_t test_data[] = {
    198, 0x24, 0x00, 0x10,          // Header
    0x01, 0x01, 0x01, 0x00,         // Index, type, status, socket
    0x01, 0x02, 0x03, 0x04,         // Vendor, family, model, bit stream version
    0x04, 0xEE,                     // HPS cores, undefined byte (offset 0Dh)
    0x04, 0x01, 0x02, 0x10,         // HPS ISA, HSSI config, ports, speed
    0x05,                           // HSSI side band configuration
    0x02,                           // Reconfiguration slots
    0x03, 0x00,                     // PCIe slot
    0x3B, 0x00, 0x01,               // PCIe bus, device, function
    0x10, 0x27, 0x00, 0x00,         // TDP (10000 mW)
    0x03,                           // Memory technology (HBM2)
    0x00, 0x20, 0x00, 0x00,         // Memory capacity (8192 MiB)
    0x80, 0x0C,                     // Memory speed (3200)
    'I', 'n', 't', 'e', 'l', 0,
    'S', 't', 'r', 'a', 't', 'i', 'x', 0,
    '1', 'S', 'G', '2', '8', '0', 0,
    '1', '.', '0', 0,
    'S', 'M', 'B', 'u', 's', 0,
    0
};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_rsd_fpga_decode, test_rsd_fpga_setup, test_rsd_fpga_teardown),
        cmocka_unit_test_setup_teardown(test_rsd_fpga_decode_short, test_rsd_fpga_setup, test_rsd_fpga_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_rsd_fpga_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    if (not dmi_add_extension(context, dmi_module_find("intel"))) {
        dmi_destroy(context);
        return -1;
    }

    *pstate = context;

    return 0;
}

static int test_rsd_fpga_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_rsd_fpga_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    dmi_entity_t *entity = dmi_entity_create(context, test_data, sizeof(test_data));
    assert_non_null(entity);
    assert_int_equal(entity->body_length, 0x24);
    assert_true(dmi_entity_decode(entity));

    const dmi_intel_rsd_fpga_t *info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_FPGA));
    assert_non_null(info);

    assert_int_equal(info->index, 1);
    assert_int_equal(info->type, DMI_INTEL_RSD_FPGA_TYPE_DISCRETE);
    assert_int_equal(info->status, DMI_INTEL_RSD_FPGA_STATUS_ENABLED);
    assert_string_equal(info->vendor, "Intel");
    assert_string_equal(info->family, "Stratix");
    assert_string_equal(info->model, "1SG280");
    assert_string_equal(info->bit_stream_version, "1.0");
    assert_int_equal(info->hps_core_count, 4);
    assert_int_equal(info->hps_isa, DMI_INTEL_RSD_FPGA_HPS_ISA_ARM_A64);
    assert_int_equal(info->hssi_config, DMI_INTEL_RSD_FPGA_HSSI_CONFIG_PCIE);
    assert_int_equal(info->hssi_port_count, 2);
    assert_int_equal(info->hssi_port_speed, 16);
    assert_string_equal(info->hssi_side_band_config, "SMBus");
    assert_int_equal(info->reconfig_slots, 2);
    assert_int_equal(info->pci_slot_id, 3);
    assert_int_equal(info->pci_bus_number, 0x3B);
    assert_int_equal(info->pci_device_id, 0x00);
    assert_int_equal(info->pci_function_id, 0x01);
    assert_int_equal(info->tdp, 10000);
    assert_int_equal(info->memory_tech, DMI_INTEL_RSD_FPGA_MEMORY_TECH_HBM2);
    assert_int_equal(info->memory_capacity, 8192);
    assert_int_equal(info->memory_speed, 3200);

    dmi_entity_destroy(entity);
}

static void test_rsd_fpga_decode_short(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Structure without the last byte is rejected
    uint8_t data[sizeof(test_data)];
    memcpy(data, test_data, sizeof(data));
    data[1] = 0x23;
    memmove(data + 0x23, test_data + 0x24, sizeof(test_data) - 0x24);

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data) - 1);
    assert_non_null(entity);
    assert_false(dmi_entity_decode(entity));

    dmi_entity_destroy(entity);
}
