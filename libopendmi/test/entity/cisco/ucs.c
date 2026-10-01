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
#include <opendmi/internal.h>
#include <opendmi/module.h>
#include <opendmi/module/cisco.h>
#include <opendmi/platform.h>
#include <opendmi/registry.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/cisco/pci-adapter.h>
#include <opendmi/entity/cisco/slot-buses.h>

static int test_setup(void **pstate);
static int test_teardown(void **pstate);

static void test_cisco_platform(void **pstate);
static void test_cisco_slot_buses(void **pstate);
static void test_cisco_pci_adapter(void **pstate);
static void test_cisco_other_firmware(void **pstate);

static const void *test_info(dmi_context_t *context, dmi_handle_t handle, const dmi_entity_spec_t *spec);

static const char *test_ucs_path = OPENDMI_TEST_DATA "/cisco/ucsc-c220-m5sx.bin";
static const char *test_ids_path = OPENDMI_TEST_DATA "/cisco/ids-4235.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_cisco_platform, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_cisco_slot_buses, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_cisco_pci_adapter, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_cisco_other_firmware, test_setup, test_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_setup(void **pstate)
{
    dmi_context_t *context = dmi_create(DMI_CONTEXT_FLAG_AUTO_MODULES);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_cisco_platform(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_ucs_path));

    // Module is enabled by the vendor of the firmware of UCS servers
    const dmi_platform_t *platform = dmi_get_platform(context);
    assert_non_null(platform);
    assert_int_equal(platform->firmware_vendor, DMI_VENDOR_CISCO);
    assert_int_equal(platform->system_vendor, DMI_VENDOR_CISCO);
    assert_true(dmi_has_extension(context, &dmi_cisco_module));
    assert_string_equal(dmi_vendor_name(DMI_VENDOR_CISCO), "Cisco");
}

static void test_cisco_slot_buses(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_ucs_path));

    // Slot of the VIC 1455, whose PCI switch makes six buses
    const dmi_cisco_slot_buses_t *info = test_info(context, 0x0063, &dmi_cisco_slot_buses_spec);
    assert_string_equal(info->slot, "SlotID:1");
    assert_int_equal(info->bus_count, 6);
    assert_memory_equal(info->buses, "\x5E\x5F\x61\x62\x63\x64", 6);

    info = test_info(context, 0x0064, &dmi_cisco_slot_buses_spec);
    assert_string_equal(info->slot, "SlotID:MLOM");
    assert_int_equal(info->bus_count, 1);
    assert_int_equal(info->buses[0], 0x18);
}

static void test_cisco_pci_adapter(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_ucs_path));

    // Identifiers are held in the big-endian byte order, e.g. the ones of
    // the Cisco VIC 1455 PCIe management controller
    const dmi_cisco_pci_adapter_t *vic = test_info(context, 0x0067, &dmi_cisco_pci_adapter_spec);
    assert_int_equal(vic->version, 3);
    assert_string_equal(vic->model, "Cisco UCS VIC 1455");
    assert_string_equal(vic->slot, "SlotID:1");
    assert_int_equal(vic->vendor_id, 0x1137);
    assert_int_equal(vic->device_id, 0x0042);
    assert_int_equal(vic->subsystem_vendor_id, 0x1137);
    assert_int_equal(vic->subsystem_id, 0x0217);
    assert_int_equal(vic->raw_device_id, 0x4200);
    assert_int_equal(vic->revision, 0xA2);
    assert_string_equal(vic->firmware_version, "N/A");

    // Intel I350 network controller of class 0x02
    const dmi_cisco_pci_adapter_t *mlom = test_info(context, 0x0068, &dmi_cisco_pci_adapter_spec);
    assert_int_equal(mlom->vendor_id, 0x8086);
    assert_int_equal(mlom->device_id, 0x1521);
    assert_int_equal(mlom->pci_class, 0x02);
    assert_int_equal(mlom->subclass, 0x00);
    assert_int_equal(mlom->prog_if, 0x00);
    assert_int_equal(mlom->revision, 0x01);
    assert_string_equal(mlom->firmware_version, "0x80000E78-1.812.1");

    // Broadcom MegaRAID controller of the RAID subclass of storage
    const dmi_cisco_pci_adapter_t *raid = test_info(context, 0x0069, &dmi_cisco_pci_adapter_spec);
    assert_int_equal(raid->vendor_id, 0x1000);
    assert_int_equal(raid->device_id, 0x0014);
    assert_int_equal(raid->pci_class, 0x01);
    assert_int_equal(raid->subclass, 0x04);
    assert_int_equal(raid->subsystem_id, 0x020E);
}

//
// Other Cisco appliances carry the firmware of other vendors, whose modules
// decode their structures
//
static void test_cisco_other_firmware(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_ids_path));

    const dmi_platform_t *platform = dmi_get_platform(context);
    assert_non_null(platform);
    assert_int_equal(platform->system_vendor, DMI_VENDOR_CISCO);
    assert_int_equal(platform->firmware_vendor, DMI_VENDOR_DELL);
    assert_false(dmi_has_extension(context, &dmi_cisco_module));
}

static const void *test_info(dmi_context_t *context, dmi_handle_t handle, const dmi_entity_spec_t *spec)
{
    dmi_registry_t *registry = dmi_get_registry(context);
    const dmi_entity_t *entity = dmi_registry_lookup(registry, handle, spec->type, false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, spec);

    const void *info = dmi_entity_info(entity, spec->type);
    assert_non_null(info);

    return info;
}
