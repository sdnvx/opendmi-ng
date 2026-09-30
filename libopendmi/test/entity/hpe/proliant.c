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
#include <opendmi/module/hpe.h>
#include <opendmi/platform.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/hpe/cru.h>
#include <opendmi/entity/hpe/dimm-location.h>
#include <opendmi/entity/hpe/microcode.h>
#include <opendmi/entity/hpe/nic.h>
#include <opendmi/entity/hpe/physical-attrs.h>
#include <opendmi/entity/hpe/power-supply.h>
#include <opendmi/entity/hpe/processor.h>
#include <opendmi/entity/hpe/proliant-info.h>
#include <opendmi/entity/hpe/rack-locator.h>
#include <opendmi/entity/hpe/reserved-memory.h>
#include <opendmi/entity/hpe/rom-info.h>
#include <opendmi/entity/hpe/system-id.h>
#include <opendmi/entity/hpe/tcontrol.h>
#include <opendmi/entity/hpe/trusted-module.h>

static int test_hpe_setup(void **pstate);
static int test_hpe_teardown(void **pstate);

static void test_hpe_blade_g1(void **pstate);
static void test_hpe_rack_g6(void **pstate);
static void test_hpe_generations(void **pstate);

static const void *test_hpe_info(dmi_context_t *context, dmi_type_t type, const dmi_entity_spec_t *spec);

static const char *test_bl460c_path = OPENDMI_TEST_DATA "/hp/proliant-bl460c-g1-416656-b21.bin";
static const char *test_dl360_path = OPENDMI_TEST_DATA "/hp/proliant-dl360-g6-484184-b21.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_hpe_blade_g1, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_rack_g6, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_generations, test_hpe_setup, test_hpe_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_hpe_setup(void **pstate)
{
    dmi_context_t *context = dmi_create(DMI_CONTEXT_FLAG_AUTO_MODULES);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_hpe_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_hpe_blade_g1(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_bl460c_path));
    assert_true(dmi_has_extension(context, &dmi_hpe_module));

    const dmi_hpe_rom_info_t *rom = test_hpe_info(context, DMI_TYPE(HPE_ROM_INFO), &dmi_hpe_rom_info_spec);
    assert_true(rom->is_redundant_rom);
    assert_string_equal(rom->redundant_rom_version, "11/13/2007");
    assert_string_equal(rom->bootblock_version, "09/18/2006");

    const dmi_hpe_system_id_t *system = test_hpe_info(context, DMI_TYPE(HPE_SYSTEM_ID), &dmi_hpe_system_id_spec);
    assert_string_equal(system->system_id, "$0E110761");

    const dmi_hpe_processor_t *processor = test_hpe_info(context, DMI_TYPE(HPE_PROCESSOR), &dmi_hpe_processor_spec);
    assert_int_equal(processor->processor_handle, 0x0400);
    assert_true(processor->is_bsp);
    assert_int_equal(processor->slot, UINT8_MAX);
    assert_int_equal(processor->socket, 1);

    const dmi_hpe_rack_locator_t *rack = test_hpe_info(context, DMI_TYPE(HPE_RACK_LOCATOR), &dmi_hpe_rack_locator_spec);
    assert_string_equal(rack->enclosure_model, "BladeSystem c7000 Enclosure");
    assert_int_equal(rack->bay_count, 16);

    // Ports of the network the firmware boots from, over PXE and over iSCSI
    const dmi_hpe_nic_info_t *pxe = test_hpe_info(context, DMI_TYPE(HPE_PXE_NIC), &dmi_hpe_pxe_nic_spec);
    assert_int_equal(pxe->port_count, 2);
    assert_int_equal(pxe->ports[0].bus, 0x03);
    assert_int_equal(pxe->ports[0].devfn, 0x00);
    assert_int_equal(pxe->ports[0].state, DMI_HPE_NIC_STATE_INSTALLED);
    assert_memory_equal(pxe->ports[0].mac_address.data, "\x00\x19\xBB\x36\x2F\xA2", 6);

    const dmi_hpe_nic_info_t *iscsi = test_hpe_info(context, DMI_TYPE(HPE_ISCSI_NIC), &dmi_hpe_iscsi_nic_spec);
    assert_int_equal(iscsi->port_count, 2);
    assert_memory_equal(iscsi->ports[0].mac_address.data, "\x00\x19\xBB\x36\x2F\xA3", 6);

    const dmi_hpe_tcontrol_t *tcontrol = test_hpe_info(context, DMI_TYPE(HPE_TCONTROL), &dmi_hpe_tcontrol_spec);
    assert_int_equal(tcontrol->tcontrol, 80);

    const dmi_hpe_cru_t *cru = test_hpe_info(context, DMI_TYPE(HPE_CRU), &dmi_hpe_cru_spec);
    assert_string_equal(cru->signature, "$CRU");
    assert_int_equal(cru->address, 0xFFF6F800);
    assert_int_equal(cru->length, 0x4000);
    assert_int_equal(cru->entry_point, 0xFFF6F800);

    // Microcode patch of Intel Penryn, released on September 12, 2007
    const dmi_hpe_microcode_t *microcode = test_hpe_info(context, DMI_TYPE(HPE_MICROCODE), &dmi_hpe_microcode_spec);
    assert_true(microcode->patch_count >= 3);
    assert_int_equal(microcode->patches[0].cpuid, 0x00010676);
    assert_int_equal(microcode->patches[0].patch_id, 0x0606);
    assert_int_equal(microcode->patches[0].date, dmi_date(2007, 9, 12));
}

static void test_hpe_rack_g6(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_dl360_path));

    const dmi_hpe_processor_t *processor = test_hpe_info(context, DMI_TYPE(HPE_PROCESSOR), &dmi_hpe_processor_spec);
    assert_int_equal(processor->maximum_power, 95);
    assert_int_equal(processor->x2apic_id, UINT32_MAX);
    assert_null(processor->qdf);

    // Sockets of the system board, which are shorter than the ones of Gen9
    const dmi_hpe_dimm_location_t *dimm = test_hpe_info(context, DMI_TYPE(HPE_DIMM_LOCATION), &dmi_hpe_dimm_location_spec);
    assert_int_equal(dimm->device_handle, 0x1100);
    assert_int_equal(dimm->board, UINT8_MAX);
    assert_int_equal(dimm->dimm, 1);
    assert_int_equal(dimm->processor, 1);
    assert_int_equal(dimm->channel_index, UINT8_MAX);

    const dmi_hpe_proliant_info_t *proliant = test_hpe_info(context, DMI_TYPE(HPE_PROLIANT_INFO), &dmi_hpe_proliant_info_spec);
    assert_int_equal(proliant->power_features, 0x0BFF);

    const dmi_hpe_trusted_module_t *tpm = test_hpe_info(context, DMI_TYPE(HPE_TRUSTED_MODULE), &dmi_hpe_trusted_module_spec);
    assert_int_equal(tpm->presence, DMI_HPE_TM_PRESENCE_ABSENT);
    assert_int_equal(tpm->version_handle, DMI_HANDLE_INVALID);

    // Product number and serial number stand for the UUID up to G7
    const dmi_hpe_physical_attrs_t *attrs = test_hpe_info(context, DMI_TYPE(HPE_PHYSICAL_ATTRS), &dmi_hpe_physical_attrs_legacy_spec);
    assert_string_equal(attrs->identifier, "484184GB894484YN");
    assert_string_equal(attrs->serial_number, "GB894484YN");

    const dmi_hpe_reserved_memory_t *memory = test_hpe_info(context, DMI_TYPE(HPE_RESERVED_MEMORY), &dmi_hpe_reserved_memory_spec);
    assert_int_equal(memory->entry_count, 2);
    assert_string_equal(memory->entries[0].signature, "$HDD");
    assert_int_equal(memory->entries[0].address, 0xDF63E000);
    assert_int_equal(memory->entries[0].size, 8192);
    assert_string_equal(memory->entries[1].signature, "$SRB");
    assert_int_equal(memory->entries[1].size, 65536);

    const dmi_hpe_power_supply_t *power = test_hpe_info(context, DMI_TYPE(HPE_POWER_SUPPLY), &dmi_hpe_power_supply_spec);
    assert_int_equal(power->power_supply_handle, 0x2700);
    assert_string_equal(power->manufacturer, "DELTA");
    assert_int_equal(power->fru_access, DMI_HPE_FRU_ACCESS_ILO);
    assert_int_equal(power->i2c_address, 0xA4);
}

static void test_hpe_generations(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_add_extension(context, &dmi_hpe_module));

    dmi_platform_t *platform = dmi_platform_create(context);
    assert_non_null(platform);
    platform->firmware_vendor = DMI_VENDOR_HPE;
    assert_true(dmi_platform_set_family(platform, DMI_HPE_FAMILY_SERVER));

    // iSCSI ports are listed up to G7, and physical attributes hold the
    // product and serial numbers
    platform->generation = DMI_HPE_GEN7;
    assert_true(dmi_set_platform(context, platform));
    assert_ptr_equal(dmi_type_spec(context, DMI_TYPE(HPE_ISCSI_NIC)), &dmi_hpe_iscsi_nic_spec);
    assert_ptr_equal(dmi_type_spec(context, DMI_TYPE(HPE_PHYSICAL_ATTRS)), &dmi_hpe_physical_attrs_legacy_spec);
    assert_ptr_equal(dmi_type_spec(context, DMI_TYPE(HPE_CRU)), &dmi_hpe_cru_spec);

    // Type 221 is deprecated from Gen8 onwards, and physical attributes hold
    // the UUID
    platform->generation = DMI_HPE_GEN10;
    assert_true(dmi_set_platform(context, platform));
    assert_null(dmi_type_spec(context, DMI_TYPE(HPE_ISCSI_NIC)));
    assert_ptr_equal(dmi_type_spec(context, DMI_TYPE(HPE_PHYSICAL_ATTRS)), &dmi_hpe_physical_attrs_spec);
    assert_null(dmi_type_spec(context, DMI_TYPE(HPE_CRU)));

    // Structures of unknown generation are decoded only by the layouts of
    // all generations
    platform->generation = 0;
    assert_true(dmi_set_platform(context, platform));
    assert_null(dmi_type_spec(context, DMI_TYPE(HPE_PHYSICAL_ATTRS)));
    assert_ptr_equal(dmi_type_spec(context, DMI_TYPE(HPE_PROLIANT_INFO)), &dmi_hpe_proliant_info_spec);

    dmi_platform_destroy(platform);
}

static const void *test_hpe_info(dmi_context_t *context, dmi_type_t type, const dmi_entity_spec_t *spec)
{
    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), type, false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, spec);

    const void *info = dmi_entity_info(entity, type);
    assert_non_null(info);

    return info;
}
