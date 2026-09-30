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
#include <opendmi/registry.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>
#include <opendmi/module/dell.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/memory-device.h>
#include <opendmi/entity/processor.h>
#include <opendmi/entity/dell/calling-iface.h>
#include <opendmi/entity/dell/device-bay.h>
#include <opendmi/entity/dell/device-names.h>
#include <opendmi/entity/dell/indexed-io.h>
#include <opendmi/entity/dell/infrared-port.h>
#include <opendmi/entity/dell/memory-ids.h>
#include <opendmi/entity/dell/revisions.h>
#include <opendmi/entity/dell/system-id.h>
#include <opendmi/entity/dell/token-refs.h>
#include <opendmi/entity/dell/video-rom.h>
#include <opendmi/entity/intel/fvi.h>
#include <opendmi/entity/intel/mei.h>

static int test_setup(void **pstate);
static int test_teardown(void **pstate);
static const void *test_info(dmi_context_t *context, dmi_handle_t handle, const dmi_entity_spec_t *spec);

static void test_dell_video_rom(void **pstate);
static void test_dell_token_refs(void **pstate);
static void test_dell_system_id(void **pstate);
static void test_dell_device_bay(void **pstate);
static void test_dell_memory_ids(void **pstate);
static void test_dell_device_names(void **pstate);
static void test_dell_platforms(void **pstate);
static void test_dell_relocations(void **pstate);
static void test_dell_intel_native(void **pstate);
static void test_dell_infrared_port(void **pstate);

static const char *test_g15_path = OPENDMI_TEST_DATA "/dell/g15-5510.bin";
static const char *test_poweredge_path = OPENDMI_TEST_DATA "/dell/poweredge-1800.bin";
static const char *test_precision_path = OPENDMI_TEST_DATA "/dell/precision-490.bin";
static const char *test_latitude_path = OPENDMI_TEST_DATA "/dell/latitude-d610.bin";
static const char *test_poweredge_400sc_path = OPENDMI_TEST_DATA "/dell/poweredge-400sc.bin";
static const char *test_poweredge_r640_path = OPENDMI_TEST_DATA "/dell/poweredge-r640.bin";
static const char *test_poweredge_8450_path = OPENDMI_TEST_DATA "/dell/poweredge-8450.bin";
static const char *test_precision_3620_path = OPENDMI_TEST_DATA "/dell/precision-tower-3620.bin";
static const char *test_unisys_path = OPENDMI_TEST_DATA "/unisys/es3020.bin";
static const char *test_t7600_path = OPENDMI_TEST_DATA "/dell/precision-t7600.bin";
static const char *test_xps_9350_path = OPENDMI_TEST_DATA "/dell/xps-13-9350.bin";
static const char *test_m3800_path = OPENDMI_TEST_DATA "/dell/precision-m3800.bin";
static const char *test_studio_path = OPENDMI_TEST_DATA "/dell/studio-1555.bin";
static const char *test_e6230_path = OPENDMI_TEST_DATA "/dell/latitude-e6230-1.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_dell_video_rom, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_dell_token_refs, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_dell_system_id, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_dell_device_bay, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_dell_memory_ids, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_dell_device_names, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_dell_platforms, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_dell_relocations, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_dell_intel_native, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_dell_infrared_port, test_setup, test_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_dell_video_rom(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_g15_path));

    const dmi_dell_video_rom_t *info = test_info(context, 0xD800, &dmi_dell_video_rom_spec);
    assert_string_equal(info->vendor, "\"Intel Corp.\"");
    assert_string_equal(info->version, "\"2089\"");

    dmi_close(context);

    assert_true(dmi_load(context, test_poweredge_path));

    info = test_info(context, 0xD800, &dmi_dell_video_rom_spec);
    assert_string_equal(info->vendor, "ATI");
    assert_string_equal(info->version, "RADEON 7000 V6.11");
}

static void test_dell_token_refs(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_precision_path));

    const dmi_dell_token_refs_1_t *refs = test_info(context, 0xDC00, &dmi_dell_token_refs_1_spec);
    assert_int_equal(refs->token_count, 9);
    assert_int_equal(refs->tokens[0], 0xF420);
    assert_int_equal(refs->tokens[2], 0xF410);
    assert_int_equal(refs->tokens[5], 0xF430);
    assert_int_equal(refs->tokens[6], 0xF440);

    const dmi_dell_token_refs_2_t *pairs = test_info(context, 0xDD00, &dmi_dell_token_refs_2_spec);
    assert_int_equal(pairs->unknown_2, 1);
    assert_int_equal(pairs->tokens[1], 0xF510);

    // Tokens are the ones the calling interface defines
    dmi_entity_t *entity = dmi_registry_lookup(dmi_get_registry(context), 0xDA02,
                                               DMI_TYPE(dell_calling_iface), false);
    assert_non_null(entity);

    const dmi_dell_calling_iface_t *iface = dmi_entity_info(entity, DMI_TYPE(dell_calling_iface));
    assert_non_null(iface);

    bool found = false;
    for (size_t i = 0; i < iface->token_count; i++) {
        if (iface->tokens[i].id == 0xF420)
            found = true;
    }
    assert_true(found);
}

static void test_dell_system_id(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_g15_path));

    // System ID is the one of the revisions and IDs
    const dmi_dell_system_id_t *info = test_info(context, 0x0000, &dmi_dell_system_id_spec);
    assert_string_equal(info->system_id, "0A64");
    assert_memory_equal(info->identifier, "_SID", 4);

    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(dell_revisions), false);
    assert_non_null(entity);

    const dmi_dell_revisions_t *revisions = dmi_entity_info(entity, DMI_TYPE(dell_revisions));
    assert_non_null(revisions);
    assert_int_equal(revisions->system_id, strtol(info->system_id, nullptr, 16));
}

static void test_dell_device_bay(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_latitude_path));

    const dmi_dell_device_bay_t *bay = test_info(context, 0xDB00, &dmi_dell_device_bay_spec);
    assert_string_equal(bay->name, "System Device Bay");
    assert_non_null(strstr(bay->supported_devices, "DVD+RW"));
    assert_string_equal(bay->installed_device, "Battery");

    const dmi_dell_device_bay_t *dock = test_info(context, 0xDB81, &dmi_dell_device_bay_spec);
    assert_string_equal(dock->name, "Dock DBay");
    assert_string_equal(dock->installed_device, "EMPTY");
    assert_false(dock->has_unknown_strings);

    // Structures of 11 bytes refer to two more strings, which Latitude E6230
    // leaves out
    dmi_close(context);
    assert_true(dmi_load(context, test_e6230_path));

    bay = test_info(context, 0xDB00, &dmi_dell_device_bay_spec);
    assert_string_equal(bay->name, "System Device Bay");
    assert_string_equal(bay->installed_device, "EMPTY");
    assert_true(bay->has_unknown_strings);
    assert_null(bay->unknown_string_1);
    assert_null(bay->unknown_string_2);
}

static void test_dell_memory_ids(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_poweredge_400sc_path));

    const dmi_dell_memory_ids_t *ids = test_info(context, 0xDF00, &dmi_dell_memory_ids_spec);
    assert_int_equal(ids->id_length, 12);
    assert_int_equal(ids->module_count, 4);

    // Module of Samsung, whose memory device carries no identifiers of its own
    const dmi_dell_memory_id_t *module = &ids->modules[0];
    assert_int_equal(module->handle, 0x1100);
    assert_int_equal(module->manufacturer.length, 8);
    assert_int_equal(module->manufacturer.data[0], 0xCE);
    assert_int_equal(module->serial_number.length, 4);
    assert_memory_equal(module->serial_number.data, "\x42\x01\xED\x1F", 4);

    dmi_entity_t *entity = dmi_registry_lookup(dmi_get_registry(context), module->handle,
                                               DMI_TYPE(memory_device), false);
    assert_non_null(entity);

    // Empty socket
    assert_int_equal(ids->modules[2].handle, 0x1102);
    assert_memory_equal(ids->modules[2].serial_number.data, "\xFF\xFF\xFF\xFF", 4);
}

static void test_dell_device_names(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_poweredge_r640_path));

    const dmi_dell_device_names_t *cpus = test_info(context, 0xE100, &dmi_dell_device_names_spec);
    assert_int_equal(cpus->device_count, 2);
    assert_string_equal(cpus->devices[1].fqdd, "CPU.Socket.2");
    assert_int_equal(cpus->devices[1].handle, 0x0401);

    dmi_entity_t *entity = dmi_registry_lookup(dmi_get_registry(context), cpus->devices[1].handle,
                                               DMI_TYPE(processor), false);
    assert_non_null(entity);

    const dmi_dell_device_names_t *dimms = test_info(context, 0xE101, &dmi_dell_device_names_spec);
    assert_int_equal(dimms->device_count, 24);
    assert_string_equal(dimms->devices[12].fqdd, "DIMM.Socket.B1");
    assert_int_equal(dimms->devices[12].handle, 0x110C);

    entity = dmi_registry_lookup(dmi_get_registry(context), dimms->devices[12].handle,
                                 DMI_TYPE(memory_device), false);
    assert_non_null(entity);

    // Handles refer to processors and memory devices only
    const dmi_attribute_t *devices = dmi_dell_device_names_spec.attributes;
    while ((devices->params.code != nullptr) and (strcmp(devices->params.code, "devices") != 0))
        devices++;
    assert_non_null(devices->params.code);

    const dmi_attribute_t *handle = devices->params.attrs;
    while ((handle->params.code != nullptr) and (strcmp(handle->params.code, "handle") != 0))
        handle++;
    assert_non_null(handle->params.targets);
    assert_ptr_equal(handle->params.targets[0], DMI_TYPE(processor));
    assert_ptr_equal(handle->params.targets[1], DMI_TYPE(memory_device));
    assert_null(handle->params.targets[2]);
}

static void test_dell_platforms(void **pstate)
{
    dmi_context_t *context = *pstate;
    const dmi_module_t *dell = dmi_module_find("dell");

    // Servers Dell makes for Unisys carry the firmware of Dell under the name
    // of Unisys
    assert_true(dmi_load(context, test_unisys_path));
    assert_true(dmi_has_extension(context, dell));

    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(dell_revisions), false);
    assert_non_null(entity);

    dmi_close(context);

    // Systems of Dell with the firmware of other vendors
    assert_true(dmi_load(context, test_poweredge_8450_path));
    assert_true(dmi_has_extension(context, dell));

    const dmi_dell_indexed_io_t *io = test_info(context, 0x007A, &dmi_dell_indexed_io_spec);
    assert_int_equal(io->index_port, 0x72);
    assert_int_equal(io->data_port, 0x73);
}

static void test_dell_relocations(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Structures of the Intel reference code 16 types up rather than down
    assert_true(dmi_load(context, test_precision_3620_path));

    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(intel_fvi), false);
    assert_non_null(entity);
    assert_int_equal(dmi_entity_type_id(entity), 237);

    const dmi_intel_fvi_t *fvi = dmi_entity_info(entity, DMI_TYPE(intel_fvi));
    assert_non_null(fvi);
    assert_string_equal(fvi->items[0].component, "Reference Code - CPU");

    entity = dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(intel_mei), false);
    assert_non_null(entity);
    assert_int_equal(dmi_entity_type_id(entity), 235);
    assert_non_null(dmi_entity_info(entity, DMI_TYPE(intel_mei)));
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

static const void *test_info(dmi_context_t *context, dmi_handle_t handle, const dmi_entity_spec_t *spec)
{
    dmi_entity_t *entity = dmi_registry_lookup(dmi_get_registry(context), handle, DMI_TYPE_ANY, false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, spec);

    const void *info = dmi_entity_info(entity, spec->type);
    assert_non_null(info);

    return info;
}

//
// Some systems keep the structures of the Intel reference code at their own
// types, where the structures of Dell are told apart by their signatures, and
// the structures of type 220 hold 8 tokens on some systems
//
static void test_dell_intel_native(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_xps_9350_path));

    dmi_registry_t *registry = dmi_get_registry(context);

    dmi_entity_t *entity = dmi_registry_lookup(registry, 0xF02A, DMI_TYPE_ANY, false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, &dmi_intel_fvi_spec);

    entity = dmi_registry_lookup(registry, 0xF03C, DMI_TYPE_ANY, false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, &dmi_intel_mei_spec);

    entity = dmi_registry_lookup(registry, 0xDD00, DMI_TYPE_ANY, false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, &dmi_dell_token_refs_2_spec);

    // Firmware version information of two items is as long as the structure
    // of Dell, and is told from it by the strings it refers to
    static const uint8_t fvi[] = {
        221, 0x13, 0x00, 0xF1,
        0x02,
        0x01, 0x02, 0x01, 0x02, 0x03, 0x04, 0x00,
        0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        'M', 'E', 0x00, '1', '.', '2', 0x00, 'B', 'I', 'O', 'S', 0x00,
        0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    entity = dmi_test_entity_create(buffer, fvi, sizeof(fvi));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_ptr_equal(entity->spec, &dmi_intel_fvi_spec);
    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);

    // Device bay of blank strings of a system which has none is not taken for
    // the interface information of the Management Engine
    dmi_close(context);
    assert_true(dmi_load(context, test_m3800_path));

    entity = dmi_registry_lookup_first_id(dmi_get_registry(context), 219, false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, &dmi_dell_device_bay_spec);

    dmi_close(context);
    assert_true(dmi_load(context, test_t7600_path));

    const dmi_dell_token_refs_1_t *refs = test_info(context, 0xDC00, &dmi_dell_token_refs_1_spec);
    assert_int_equal(refs->token_count, 8);
    assert_int_equal(refs->tokens[0], 0xF000);
}

//
// Infrared ports are told by their length, since other structures of type
// 211 are found, e.g. on Studio 1555
//
static void test_dell_infrared_port(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_latitude_path));

    const dmi_dell_infrared_port_t *info = test_info(context, 0xD300, &dmi_dell_infrared_port_spec);
    assert_string_equal(info->location, "Back of System");

    dmi_close(context);

    assert_true(dmi_load(context, test_studio_path));

    dmi_entity_t *entity = dmi_registry_lookup(dmi_get_registry(context), 0xD300, DMI_TYPE_ANY, false);
    assert_non_null(entity);
    assert_int_equal(entity->body_length, 0x11);
    assert_null(entity->spec);
}
