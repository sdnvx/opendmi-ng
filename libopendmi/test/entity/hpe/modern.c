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
#include <opendmi/encoder.h>
#include <opendmi/entity.h>
#include <opendmi/internal.h>
#include <opendmi/module.h>
#include <opendmi/module/hpe.h>
#include <opendmi/platform.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/hpe/backplane.h>
#include <opendmi/entity/hpe/device-correlation.h>
#include <opendmi/entity/hpe/dimm-attrs.h>
#include <opendmi/entity/hpe/dimm-config.h>
#include <opendmi/entity/hpe/dimm-location.h>
#include <opendmi/entity/hpe/dimm-vendor.h>
#include <opendmi/entity/hpe/drive.h>
#include <opendmi/entity/hpe/extension-board.h>
#include <opendmi/entity/hpe/inventory.h>
#include <opendmi/entity/hpe/nic-mac.h>
#include <opendmi/entity/hpe/processor.h>
#include <opendmi/entity/hpe/rom-info.h>
#include <opendmi/entity/hpe/trusted-module.h>
#include <opendmi/entity/hpe/usb-device.h>
#include <opendmi/entity/hpe/usb-port.h>
#include <opendmi/entity/hpe/version.h>

typedef struct test_state
{
    dmi_context_t *context;
    dmi_buffer_t  *buffer;
    dmi_entity_t  *entity;
} test_state_t;

static int test_hpe_setup(void **pstate);
static int test_hpe_teardown(void **pstate);

static void test_hpe_device_correlation(void **pstate);
static void test_hpe_version(void **pstate);
static void test_hpe_version_formats(void **pstate);
static void test_hpe_dimm_attrs(void **pstate);
static void test_hpe_nic_mac(void **pstate);
static void test_hpe_backplane(void **pstate);
static void test_hpe_dimm_vendor(void **pstate);
static void test_hpe_usb(void **pstate);
static void test_hpe_inventory(void **pstate);
static void test_hpe_drive(void **pstate);
static void test_hpe_dimm_config(void **pstate);
static void test_hpe_extension_board(void **pstate);
static void test_hpe_trusted_module(void **pstate);
static void test_hpe_rom_info(void **pstate);
static void test_hpe_processor(void **pstate);
static void test_hpe_dimm_location(void **pstate);

static void test_hpe_generation(test_state_t *state, unsigned generation);
static const void *test_hpe_decode(test_state_t *state, const void *data, size_t size,
                                   const dmi_entity_spec_t *spec);
static const dmi_attribute_t *test_hpe_attribute(test_state_t *state, const char *code);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_hpe_device_correlation, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_version, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_version_formats, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_dimm_attrs, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_nic_mac, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_backplane, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_dimm_vendor, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_usb, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_inventory, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_drive, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_dimm_config, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_extension_board, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_trusted_module, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_rom_info, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_processor, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_dimm_location, test_hpe_setup, test_hpe_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_hpe_setup(void **pstate)
{
    test_state_t *state = calloc(1, sizeof(*state));
    if (state == nullptr)
        return -1;

    state->context = dmi_create(0);
    if (state->context == nullptr) {
        free(state);
        return -1;
    }

    dmi_set_logger(state->context, &test_logger);

    state->buffer = dmi_buffer_create(state->context);
    if ((state->buffer == nullptr) or not dmi_add_extension(state->context, &dmi_hpe_module)) {
        dmi_buffer_destroy(state->buffer);
        dmi_destroy(state->context);
        free(state);
        return -1;
    }

    test_hpe_generation(state, DMI_HPE_GEN10);

    *pstate = state;

    return 0;
}

static int test_hpe_teardown(void **pstate)
{
    test_state_t *state = *pstate;

    dmi_entity_destroy(state->entity);
    dmi_buffer_destroy(state->buffer);
    dmi_destroy(state->context);
    free(state);

    *pstate = nullptr;

    return 0;
}

static void test_hpe_device_correlation(void **pstate)
{
    test_state_t *state = *pstate;

    // Port of an HPE 562SFP+ (Intel X710) in slot 1, bifurcated from the slot
    // of handle 0x0901
    static const uint8_t data[] = {
        203, 0x28, 0x00, 0xCB,
        0x00, 0x09, 0xFE, 0xFF,
        0x86, 0x80, 0x72, 0x15, 0x3C, 0x10, 0xFD, 0x22,
        0x02, 0x00, 0xFE, 0xFF,
        0x01, 0x00,
        0x05, 0x0A, 0x01, 0x02, 0xFF, 0x00,
        0x01, 0x02, 0x03, 0x04,
        0x01, 0x09,
        0x05, 0x06,
        0x00, 0x00, 0x5C, 0x08,
        'P', 'c', 'i', 'R', 'o', 'o', 't', '(', '0', ')', 0x00,
        'N', 'I', 'C', '.', 'S', 'l', 'o', 't', '.', '1', '.', '1', 0x00,
        'S', 'l', 'o', 't', ' ', '1', ' ', 'P', 'o', 'r', 't', ' ', '1', 0x00,
        'S', 'l', 'o', 't', ' ', '1', 0x00,
        'P', '2', '1', '9', '3', '3', '-', 'B', '2', '1', 0x00,
        'S', 'N', '1', '2', '3', 0x00,
        0x00
    };

    const dmi_hpe_device_correlation_t *info =
        test_hpe_decode(state, data, sizeof(data), &dmi_hpe_device_correlation_spec);

    assert_int_equal(info->device_handle, 0x0900);
    assert_int_equal(info->smbus_handle, 0xFFFE);
    assert_int_equal(info->pci_vendor_id, 0x8086);
    assert_int_equal(info->pci_device_id, 0x1572);
    assert_int_equal(info->pci_subvendor_id, 0x103C);
    assert_int_equal(info->pci_subdevice_id, 0x22FD);
    assert_int_equal(info->pci_class, 0x02);
    assert_true(info->is_peer_bifurcated);
    assert_false(info->is_upstream);
    assert_int_equal(info->device_type, DMI_HPE_DEVICE_TYPE_SLOT_NIC);
    assert_int_equal(info->device_location, DMI_HPE_DEVICE_LOCATION_PCI_SLOT);
    assert_int_equal(info->sub_instance, 2);
    assert_int_equal(info->bay, UINT8_MAX);
    assert_string_equal(info->uefi_device_name, "NIC.Slot.1.1");
    assert_string_equal(info->uefi_location, "Slot 1");
    assert_int_equal(info->physical_handle, 0x0901);
    assert_string_equal(info->part_number, "P21933-B21");
    assert_int_equal(info->bus, 0x5C);
    assert_int_equal(info->devfn, 0x08);
    assert_non_null(dmi_attribute_resolve(test_hpe_attribute(state, "physical-handle"), info));

    // Slot handle is given for the peer bifurcated devices only
    uint8_t other[sizeof(data)];
    memcpy(other, data, sizeof(other));
    other[0x14] = 0x00;

    info = test_hpe_decode(state, other, sizeof(other), &dmi_hpe_device_correlation_spec);
    assert_false(info->is_peer_bifurcated);
    assert_null(dmi_attribute_resolve(test_hpe_attribute(state, "physical-handle"), info));

    // Structures shorter than 40 bytes end before the PCI location, which is
    // not shown
    assert_true(info->has_pci_location);

    uint8_t shorter[sizeof(data) - 4];
    memcpy(shorter, data, 0x24);
    memcpy(shorter + 0x24, data + 0x28, sizeof(data) - 0x28);
    shorter[0x01] = 0x24;

    info = test_hpe_decode(state, shorter, sizeof(shorter), &dmi_hpe_device_correlation_spec);
    assert_false(info->has_pci_location);
    assert_string_equal(info->part_number, "P21933-B21");
    assert_null(dmi_attribute_resolve(test_hpe_attribute(state, "segment"), info));
    assert_null(dmi_attribute_resolve(test_hpe_attribute(state, "bus"), info));
    assert_null(dmi_attribute_resolve(test_hpe_attribute(state, "devfn"), info));

    // Structures are decoded from Gen9 onwards only
    test_hpe_generation(state, DMI_HPE_GEN8);
    assert_null(dmi_type_spec(state->context, DMI_TYPE_ID(HPE_DEVICE_CORRELATION)));
}

static void test_hpe_version(void **pstate)
{
    test_state_t *state = *pstate;

    // Format 5 of Gen10 takes the low nibbles of four bytes
    static const uint8_t rom[] = {
        216, 0x17, 0x00, 0xD8,
        0x01, 0x00, 0x01, 0x02, 0x05,
        0x00, 0x02, 0x00, 0x07, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,
        'S', 'y', 's', 't', 'e', 'm', ' ', 'R', 'O', 'M', 0x00,
        'U', '3', '0', ' ', 'v', '2', '.', '7', '2', 0x00,
        0x00
    };

    const dmi_hpe_version_t *info = test_hpe_decode(state, rom, sizeof(rom), &dmi_hpe_version_spec);
    assert_int_equal(info->firmware_type, DMI_HPE_FIRMWARE_TYPE_SYSTEM_ROM);
    assert_string_equal(info->firmware_name, "System ROM");
    assert_string_equal(info->version_string, "U30 v2.72");
    assert_string_equal(info->version, "2.7.2.0");

    // Format 5 of Gen9 takes the nibbles of the first byte instead
    static const uint8_t gen9[] = {
        216, 0x17, 0x01, 0xD8,
        0x08, 0x00, 0x01, 0x00, 0x05,
        0x27, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,
        'S', 'y', 's', 't', 'e', 'm', ' ', 'P', 'r', 'o', 'g', 'r', 'a', 'm', 'm', 'a',
        'b', 'l', 'e', ' ', 'L', 'o', 'g', 'i', 'c', ' ', 'D', 'e', 'v', 'i', 'c', 'e', 0x00,
        0x00
    };

    test_hpe_generation(state, DMI_HPE_GEN9);
    info = test_hpe_decode(state, gen9, sizeof(gen9), &dmi_hpe_version_spec);
    assert_int_equal(info->firmware_type, DMI_HPE_FIRMWARE_TYPE_CPLD);
    assert_null(info->version_string);
    assert_string_equal(info->version, "2.7.3");

    // Format 7 holds a version and a date
    static const uint8_t dated[] = {
        216, 0x17, 0x02, 0xD8,
        0x04, 0x00, 0x00, 0x00, 0x07,
        0x02, 0x48, 0x09, 0x0D, 0xE6, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x34, 0x12,
        0x00, 0x00
    };

    info = test_hpe_decode(state, dated, sizeof(dated), &dmi_hpe_version_spec);
    assert_string_equal(info->version, "v2.72 (09/13/2022)");
    assert_int_equal(info->unique_id, 0x1234);

    // Format 3 is reserved
    static const uint8_t reserved[] = {
        216, 0x17, 0x03, 0xD8,
        0x04, 0x00, 0x00, 0x00, 0x03,
        0x02, 0x48, 0x09, 0x0D, 0xE6, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00
    };

    info = test_hpe_decode(state, reserved, sizeof(reserved), &dmi_hpe_version_spec);
    assert_null(info->version);
}

static void test_hpe_version_formats(void **pstate)
{
    test_state_t *state = *pstate;

    // Words are held low byte first, and the bank of format 1 is told by the
    // high bit of the first byte
    static const uint8_t numeric[] = { 0x27, 0x83, 0x05, 0x0D, 0xE6, 0x07, 0x41, 0x92, 0x09, 0x0A, 0x0B, 0x0C };
    static const uint8_t banked[]  = { 0xA7, 0x03, 0x05, 0x0D, 0xE6, 0x07, 0x41, 0x92, 0x09, 0x0A, 0x0B, 0x0C };

    // Format 16 starts with four printable characters
    static const uint8_t text[]    = { 'U', '3', '0', 'A', 0x02, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static const uint8_t control[] = { 'U', 0x01, '0', 'A', 0x02, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    static const struct
    {
        unsigned       generation;
        uint8_t        format;
        const uint8_t *data;
        const char    *version;
    } cases[] = {
        { DMI_HPE_GEN10,      0,  numeric, nullptr },
        { DMI_HPE_GEN10,      1,  numeric, "0x03" },
        { DMI_HPE_GEN10,      2,  numeric, "2.7" },
        { DMI_HPE_GEN10,      3,  numeric, nullptr },
        { DMI_HPE_GEN10,      4,  numeric, "2.7.3" },
        { DMI_HPE_GEN10,      5,  numeric, "3.13.7.1" },
        { DMI_HPE_GEN10,      6,  numeric, "131.39" },
        { DMI_HPE_GEN10,      7,  numeric, "v39.131 (05/13/2022)" },
        { DMI_HPE_GEN10,      8,  numeric, "2022.33575" },
        { DMI_HPE_GEN10,      9,  numeric, "39.131.3333" },
        { DMI_HPE_GEN10,      10, numeric, "39.131.5 Build 13" },
        { DMI_HPE_GEN10,      11, numeric, "3333.33575 2453735398" },
        { DMI_HPE_GEN10,      12, numeric, "33575.3333.2022.37441" },
        { DMI_HPE_GEN10,      13, numeric, "39" },
        { DMI_HPE_GEN10,      14, numeric, "39.131.5.58893" },
        { DMI_HPE_GEN10,      15, numeric, "33575.3333.2022.37441 (09/10/3083)" },
        { DMI_HPE_GEN10,      16, numeric, nullptr },
        { DMI_HPE_GEN10,      17, numeric, "0D058327" },
        { DMI_HPE_GEN10,      18, numeric, "39.131" },
        { DMI_HPE_GEN10,      19, numeric, "0x27.0x83.0x05" },
        { DMI_HPE_GEN10,      20, numeric, "39.131.5.13" },
        { DMI_HPE_GEN10,      21, numeric, nullptr },
        { DMI_HPE_GEN10,      1,  banked,  "0x03 B.0x27" },
        { DMI_HPE_GEN9,       5,  numeric, "2.7.3" },
        { DMI_HPE_GEN8,       5,  numeric, nullptr },
        { DMI_HPE_GEN10_PLUS, 5,  numeric, "3.13.7.1" },
        { DMI_HPE_GEN10,      16, text,    "U30A.27" },
        { DMI_HPE_GEN10,      16, control, nullptr },
    };

    for (size_t i = 0; i < countof(cases); i++) {
        uint8_t data[] = {
            216, 0x17, 0x00, 0xD8,
            0x04, 0x00, 0x00, 0x00, cases[i].format,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00
        };

        memcpy(data + 9, cases[i].data, 12);

        test_hpe_generation(state, cases[i].generation);

        const dmi_hpe_version_t *info = test_hpe_decode(state, data, sizeof(data), &dmi_hpe_version_spec);
        if (cases[i].version == nullptr)
            assert_null(info->version);
        else
            assert_string_equal(info->version, cases[i].version);
    }
}

static void test_hpe_dimm_attrs(void **pstate)
{
    test_state_t *state = *pstate;

    // SmartMemory of standard memory, which is not load reduced
    static const uint8_t legacy[] = {
        232, 0x0E, 0x00, 0xE8,
        0x00, 0x11,
        0x35, 0x00, 0x00, 0x00,
        0xB0, 0x04, 0xB0, 0x04,
        0x00, 0x00
    };

    const dmi_hpe_dimm_attrs_t *info = test_hpe_decode(state, legacy, sizeof(legacy), &dmi_hpe_dimm_attrs_spec);
    assert_int_equal(info->device_handle, 0x1100);
    assert_int_equal(info->smart_memory, DMI_HPE_FLAG_YES);
    assert_int_equal(info->load_reduced, DMI_HPE_FLAG_NO);
    assert_int_equal(info->standard_memory, DMI_HPE_FLAG_YES);
    assert_int_equal(info->minimum_voltage, 1200);
    assert_false(info->is_training_error);
    assert_int_equal(info->encryption, UINT8_MAX);

    // Mapped out module of unknown SmartMemory, whose flags are undefined
    static const uint8_t mapped[] = {
        232, 0x24, 0x01, 0xE8,
        0x01, 0x11,
        0x02, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x4C, 0x04,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x01,
        0x00, 0x00
    };

    info = test_hpe_decode(state, mapped, sizeof(mapped), &dmi_hpe_dimm_attrs_spec);
    assert_int_equal(info->smart_memory, DMI_HPE_FLAG_UNSPEC);
    assert_int_equal(info->load_reduced, DMI_HPE_FLAG_UNSPEC);
    assert_int_equal(info->minimum_voltage, 0);
    assert_int_equal(info->configured_voltage, 1100);
    assert_false(info->is_config_error);
    assert_true(info->is_training_error);
    assert_int_equal(info->encryption, DMI_HPE_ENCRYPTION_ENCRYPTED);
}

static void test_hpe_nic_mac(void **pstate)
{
    test_state_t *state = *pstate;

    // Shortest structure, which holds no port number
    static const uint8_t data[] = {
        233, 0x0E, 0x00, 0xE9,
        0x00, 0x00, 0x37, 0x00,
        0x94, 0x40, 0xC9, 0x3A, 0x5B, 0x10,
        0x00, 0x00
    };

    const dmi_hpe_nic_mac_t *info = test_hpe_decode(state, data, sizeof(data), &dmi_hpe_nic_mac_spec);
    assert_int_equal(info->bus, 0x37);
    assert_int_equal(info->state, DMI_HPE_NIC_STATE_INSTALLED);
    assert_memory_equal(info->mac_address.data, "\x94\x40\xC9\x3A\x5B\x10", 6);
    assert_int_equal(info->port, UINT8_MAX);
    assert_null(info->uefi_device_path);
}

static void test_hpe_backplane(void **pstate)
{
    test_state_t *state = *pstate;

    static const uint8_t data[] = {
        236, 0x15, 0x00, 0xEC,
        0xA0, 0x01, 0x00, 0x20, 0x01,
        0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
        0x08, 0x04, 0x04, 0x01,
        '8', 'S', 'F', 'F', 0x00,
        0x00
    };

    const dmi_hpe_backplane_t *info = test_hpe_decode(state, data, sizeof(data), &dmi_hpe_backplane_spec);
    assert_int_equal(info->i2c_address, 0xA0);
    assert_int_equal(info->box_number, 1);
    assert_int_equal(info->nvram_id, 0x0120);
    assert_int_equal(info->wwid, 0x0102030405060708);
    assert_int_equal(info->bay_count, 8);
    assert_string_equal(info->name, "8SFF");
    assert_true(info->has_legacy_details);
    assert_non_null(dmi_attribute_resolve(test_hpe_attribute(state, "a0-bay-count"), info));

    // Bays of the ports and the name are deprecated from Gen10 Plus onwards
    test_hpe_generation(state, DMI_HPE_GEN10_PLUS);
    info = test_hpe_decode(state, data, sizeof(data), &dmi_hpe_backplane_spec);
    assert_false(info->has_legacy_details);

    static const char *deprecated[] = { "a0-bay-count", "a2-bay-count", "name" };
    for (size_t i = 0; i < countof(deprecated); i++)
        assert_null(dmi_attribute_resolve(test_hpe_attribute(state, deprecated[i]), info));

    // Structures are not known from Gen11 onwards
    test_hpe_generation(state, DMI_HPE_GEN11);
    assert_null(dmi_type_spec(state->context, DMI_TYPE_ID(HPE_BACKPLANE)));
}

static void test_hpe_dimm_vendor(void **pstate)
{
    test_state_t *state = *pstate;

    // Manufacture date in binary-coded decimal: week 35 of 2021
    static const uint8_t data[] = {
        237, 0x0B, 0x00, 0xED,
        0x00, 0x11, 0x01, 0x02, 0x03, 0x21, 0x35,
        'S', 'a', 'm', 's', 'u', 'n', 'g', 0x00,
        'M', '3', '9', '3', 'A', '4', 'K', '4', '0', 'D', 'B', '3', 0x00,
        '1', '2', '3', '4', '5', '6', '7', '8', 0x00,
        0x00
    };

    const dmi_hpe_dimm_vendor_t *info = test_hpe_decode(state, data, sizeof(data), &dmi_hpe_dimm_vendor_spec);
    assert_string_equal(info->manufacturer, "Samsung");
    assert_string_equal(info->part_number, "M393A4K40DB3");
    assert_string_equal(info->serial_number, "12345678");
    assert_int_equal(info->manufacture_year, 21);
    assert_int_equal(info->manufacture_week, 35);
}

static void test_hpe_usb(void **pstate)
{
    test_state_t *state = *pstate;

    // Front port of the controller at 0000:00:14.0, not behind a hub
    static const uint8_t port[] = {
        238, 0x11, 0x00, 0xEE,
        0x00, 0x08, 0x00, 0xA0, 0x01, 0x00, 0x00, 0x01, 0xFE, 0x03, 0x01,
        0x00, 0x00,
        'P', 'c', 'i', 'R', 'o', 'o', 't', '(', '0', ')', 0x00,
        0x00
    };

    const dmi_hpe_usb_port_t *info = test_hpe_decode(state, port, sizeof(port), &dmi_hpe_usb_port_spec);
    assert_int_equal(info->port_handle, 0x0800);
    assert_int_equal(info->devfn, 0xA0);
    assert_int_equal(info->location, DMI_HPE_USB_LOCATION_FRONT);
    assert_int_equal(info->sharing, DMI_HPE_USB_SHARING_NONE);
    assert_int_equal(info->hub_instance, 0xFE);
    assert_int_equal(info->speed, DMI_HPE_USB_SPEED_SUPER);
    assert_string_equal(info->uefi_device_path, "PciRoot(0)");

    // Flash drive of 30000 megabytes plugged into it
    static const uint8_t device[] = {
        239, 0x17, 0x00, 0xEF,
        0x00, 0xEE, 0x81, 0x07, 0x00, 0x00, 0x08, 0x06, 0x50, 0x81, 0x55,
        0x30, 0x75, 0x00, 0x00,
        0x01, 0x02, 0x03, 0x04,
        'U', 'S', 'B', '(', '0', ')', 0x00,
        'U', 'S', 'B', '.', 'F', 'r', 'o', 'n', 't', '.', '1', 0x00,
        'F', 'l', 'a', 's', 'h', 0x00,
        'F', 'r', 'o', 'n', 't', 0x00,
        0x00
    };

    const dmi_hpe_usb_device_t *usb = test_hpe_decode(state, device, sizeof(device), &dmi_hpe_usb_device_spec);
    assert_int_equal(usb->port_handle, 0xEE00);
    assert_int_equal(usb->vendor_id, 0x0781);
    assert_false(usb->is_sd_card_present);
    assert_int_equal(usb->usb_class, 0x08);
    assert_int_equal(usb->product_id, 0x5581);

    // Subclass and protocol of mass storage devices are named
    const dmi_attribute_t *subclass = dmi_attribute_resolve(test_hpe_attribute(state, "usb-subclass"), usb);
    assert_non_null(subclass);
    assert_int_equal(subclass->type, DMI_ATTRIBUTE_TYPE_ENUM);
    assert_string_equal(dmi_hpe_usb_storage_subclass_name(usb->usb_subclass), "SCSI transparent command set");
    assert_string_equal(dmi_hpe_usb_storage_proto_name(usb->usb_protocol), "Bulk-only transport");
    assert_string_equal(dmi_hpe_usb_hub_proto_name(DMI_HPE_USB_HUB_PROTO_MULTI_TT),
                        "Hi-speed with multiple transaction translators");
    assert_string_equal(dmi_hpe_usb_storage_subclass_name((dmi_hpe_usb_storage_subclass_t)0x10), "Reserved");

    // Codes of the other classes are shown as they are
    uint8_t other[sizeof(device)];
    memcpy(other, device, sizeof(other));
    other[0x0A] = 0x03;

    usb = test_hpe_decode(state, other, sizeof(other), &dmi_hpe_usb_device_spec);
    subclass = dmi_attribute_resolve(test_hpe_attribute(state, "usb-subclass"), usb);
    assert_non_null(subclass);
    assert_int_equal(subclass->type, DMI_ATTRIBUTE_TYPE_INTEGER);
    assert_int_equal(usb->capacity, UINT64_C(30000) * 1024 * 1024);
    assert_string_equal(usb->location, "Front");
}

static void test_hpe_inventory(void **pstate)
{
    test_state_t *state = *pstate;

    // Firmware in use, which may be updated
    static const uint8_t data[] = {
        240, 0x27, 0x00, 0xF0,
        0x00, 0xCB, 0x04, 0x03, 0x02, 0x01, 0x01,
        0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        '1', '.', '2', '.', '3', 0x00,
        0x00
    };

    const dmi_hpe_inventory_t *info = test_hpe_decode(state, data, sizeof(data), &dmi_hpe_inventory_spec);
    assert_int_equal(info->correlation_handle, 0xCB00);
    assert_int_equal(info->package_version, 0x01020304);
    assert_string_equal(info->version_string, "1.2.3");
    assert_int_equal(info->image_size, 0x100000);
    assert_int_equal(info->is_updatable, DMI_HPE_FLAG_YES);
    assert_int_equal(info->is_reset_required, DMI_HPE_FLAG_NO);
    assert_int_equal(info->is_auth_required, DMI_HPE_FLAG_UNSPEC);
    assert_int_equal(info->is_in_use, DMI_HPE_FLAG_YES);
    assert_int_equal(info->is_uefi_image, DMI_HPE_FLAG_NO);
    assert_int_equal(info->lowest_version, 0);
}

static void test_hpe_drive(void **pstate)
{
    test_state_t *state = *pstate;

    // NVMe drive of 1.6 TB, whose structure holds every field
    static const uint8_t data[] = {
        242, 0x3E, 0x00, 0xF2,
        0x00, 0xCB, 0x01,
        0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11,
        0x00, 0x6A, 0x18, 0x00,
        0xD2, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x19, 0x03, 0x00,
        0x01, 0x02, 0x03, 0x04, 0x03,
        0x00, 0x80, 0x6E, 0x87, 0x74, 0x01, 0x00, 0x00,
        0x00, 0x02, 0x00, 0x00,
        0x01, 0x00, 0x10, 0x00, 0x10, 0x00,
        'S', '1', 0x00,
        'M', 'Z', 'X', 'L', 0x00,
        'H', 'P', 'K', '1', 0x00,
        'B', 'o', 'x', ' ', '3', 0x00,
        0x00
    };

    const dmi_hpe_drive_t *info = test_hpe_decode(state, data, sizeof(data), &dmi_hpe_drive_spec);
    assert_int_equal(info->drive_type, DMI_HPE_DRIVE_TYPE_NVME_SSD);
    assert_int_equal(info->unique_id, 0x1122334455667788);
    assert_int_equal(info->legacy_capacity, 1600000);
    assert_int_equal(info->power_on_hours, 1234);
    assert_int_equal(info->power, 25);
    assert_int_equal(info->form_factor, DMI_HPE_DRIVE_FORM_FACTOR_2_5);
    assert_int_equal(info->health, DMI_HPE_DRIVE_HEALTH_OK);
    assert_string_equal(info->model_number, "MZXL");
    assert_string_equal(info->location, "Box 3");
    assert_int_equal(info->encryption, DMI_HPE_ENCRYPTION_UNSUPPORTED);
    assert_int_equal(info->capacity, 1600000000000);
    assert_int_equal(info->block_size, 512);
    assert_int_equal(info->rotation_speed, 1);
    assert_int_equal(info->capable_speed, 16);

    // Structures are decoded from Gen10 onwards only
    test_hpe_generation(state, DMI_HPE_GEN9);
    assert_null(dmi_type_spec(state->context, DMI_TYPE_ID(HPE_DRIVE)));
}

static void test_hpe_dimm_config(void **pstate)
{
    test_state_t *state = *pstate;

    // Byte accessible region of 16 GiB, interleaved over six modules
    static const uint8_t data[] = {
        244, 0x16, 0x00, 0xF4,
        0x00, 0x11, 0x01, 0x02, 0x00,
        0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x03, 0x00, 0x06, 0x00,
        0x00, 0x00
    };

    const dmi_hpe_dimm_config_t *info = test_hpe_decode(state, data, sizeof(data), &dmi_hpe_dimm_config_spec);
    assert_int_equal(info->region_id, 1);
    assert_false(info->is_volatile);
    assert_true(info->is_byte_accessible);
    assert_int_equal(info->size, UINT64_C(16) << 30);
    assert_true(info->is_passphrase_enabled);
    assert_int_equal(info->interleave_set, 3);
    assert_int_equal(info->interleave_dimm_count, 6);
    assert_int_equal(info->interleave_health, DMI_HPE_INTERLEAVE_HEALTH_HEALTHY);

    // Passphrase is enabled for any state other than zero, and health 0xFF
    // is a reserved value rather than a missing one
    uint8_t other[sizeof(data)];
    memcpy(other, data, sizeof(other));
    other[0x11] = 0x02;
    other[0x15] = 0xFF;

    info = test_hpe_decode(state, other, sizeof(other), &dmi_hpe_dimm_config_spec);
    assert_int_equal(info->passphrase_state, 0x02);
    assert_true(info->is_passphrase_enabled);
    assert_int_equal(info->interleave_health, 0xFF);
    assert_string_equal(dmi_hpe_interleave_health_name(info->interleave_health), "Reserved");

    // Health of the structures ending before it is told apart from the values
    static const uint8_t shortest[] = {
        244, 0x14, 0x01, 0xF4,
        0x00, 0x11, 0x01, 0x02, 0x00,
        0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x03, 0x00,
        0x00, 0x00
    };

    info = test_hpe_decode(state, shortest, sizeof(shortest), &dmi_hpe_dimm_config_spec);
    assert_false(info->is_passphrase_enabled);
    assert_int_equal(info->interleave_health, DMI_HPE_INTERLEAVE_HEALTH_ABSENT);
    assert_null(dmi_hpe_interleave_health_name(info->interleave_health));
}

static void test_hpe_extension_board(void **pstate)
{
    test_state_t *state = *pstate;

    // Board type tells the layout
    static const uint8_t riser[] = {
        245, 0x09, 0x00, 0xF5,
        0x00, 0x01, 0x05, 0x83, 0x01,
        'P', 'r', 'i', 'm', 'a', 'r', 'y', 0x00,
        0x00
    };

    const dmi_hpe_riser_t *info = test_hpe_decode(state, riser, sizeof(riser), &dmi_hpe_riser_spec);
    assert_int_equal(info->position, DMI_HPE_RISER_POSITION_PRIMARY);
    assert_int_equal(info->riser_id, 5);
    assert_int_equal(info->cpld_version, 0x03);
    assert_true(info->is_cpld_b_release);
    assert_string_equal(info->name, "Primary");

    static const uint8_t mhs[] = {
        245, 0x0E, 0x01, 0xF5,
        0x01, 0x07, 0x01, 0x02, 0x01, 0x01, 0x03, 0x11, 0x12, 0x13,
        'M', 'H', 'S', 0x00,
        0x00
    };

    const dmi_hpe_mhs_riser_t *board = test_hpe_decode(state, mhs, sizeof(mhs), &dmi_hpe_mhs_riser_spec);
    assert_int_equal(board->riser_id, 7);
    assert_int_equal(board->firmware_major, 1);
    assert_int_equal(board->firmware_minor, 2);
    assert_true(board->is_downgradable);
    assert_string_equal(board->name, "MHS");
    assert_int_equal(board->slot_total, 3);
    assert_int_equal(board->slot_count, 3);
    assert_int_equal(board->slot_ids[2], 0x13);

    // Board types of no known layout are not decoded
    static const uint8_t other[] = {
        245, 0x09, 0x02, 0xF5,
        0x02, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00
    };

    dmi_entity_destroy(state->entity);
    state->entity = dmi_test_entity_create(state->buffer, other, sizeof(other));
    assert_non_null(state->entity);
    dmi_entity_decode(state->entity);
    assert_null(state->entity->spec);
}

static void test_hpe_trusted_module(void **pstate)
{
    test_state_t *state = *pstate;

    // TPM 2.0 soldered down and FIPS certified, disabled for a self-test
    // failure, whose chip is told by the low byte of the identifier
    static const uint8_t full[] = {
        224, 0x0C, 0x00, 0xE0,
        0x02, 0x06, 0x02, 0x0B, 0x00, 0xD8, 0x05, 0x80,
        0x00, 0x00
    };

    const dmi_hpe_trusted_module_t *info = test_hpe_decode(state, full, sizeof(full), &dmi_hpe_trusted_module_spec);
    assert_int_equal(info->presence, DMI_HPE_TM_PRESENCE_DISABLED);
    assert_int_equal(info->disable_reason, DMI_HPE_TM_DISABLE_REASON_ERROR);
    assert_int_equal(info->error_condition, DMI_HPE_TM_ERROR_SELF_TEST);
    assert_int_equal(info->type, DMI_HPE_TM_TYPE_TPM_2_0);
    assert_int_equal(info->mounting, DMI_HPE_TM_MOUNTING_SOLDERED);
    assert_int_equal(info->fips, DMI_HPE_TM_FIPS_CERTIFIED);
    assert_int_equal(info->version_handle, 0xD800);
    assert_int_equal(info->chip, DMI_HPE_TM_CHIP_STM_GEN11);
    assert_true(info->has_extended_status);
    assert_true(info->has_chip);
    assert_string_equal(dmi_hpe_tm_error_name(info->error_condition), "Self-test failure");

    static const char *shown[] = { "disable-reason", "error-condition", "type", "chip" };
    for (size_t i = 0; i < countof(shown); i++)
        assert_non_null(dmi_attribute_resolve(test_hpe_attribute(state, shown[i]), info));

    // Module disabled by the user has no error condition, and an enabled one
    // no disable reason
    uint8_t other[sizeof(full)];
    memcpy(other, full, sizeof(other));
    other[0x05] = 0x01;

    info = test_hpe_decode(state, other, sizeof(other), &dmi_hpe_trusted_module_spec);
    assert_non_null(dmi_attribute_resolve(test_hpe_attribute(state, "disable-reason"), info));
    assert_null(dmi_attribute_resolve(test_hpe_attribute(state, "error-condition"), info));

    other[0x04] = 0x01;

    info = test_hpe_decode(state, other, sizeof(other), &dmi_hpe_trusted_module_spec);
    assert_null(dmi_attribute_resolve(test_hpe_attribute(state, "disable-reason"), info));

    // Structures of 5 bytes hold the status only
    static const uint8_t status[] = {
        224, 0x05, 0x01, 0xE0,
        0x00,
        0x00, 0x00
    };

    info = test_hpe_decode(state, status, sizeof(status), &dmi_hpe_trusted_module_spec);
    assert_false(info->has_extended_status);
    assert_false(info->has_chip);

    static const char *hidden[] = {
        "type", "is-standard-algorithm", "is-chinese-algorithm", "mounting", "fips",
        "version-handle", "chip"
    };
    for (size_t i = 0; i < countof(hidden); i++)
        assert_null(dmi_attribute_resolve(test_hpe_attribute(state, hidden[i]), info));
}

static void test_hpe_rom_info(void **pstate)
{
    test_state_t *state = *pstate;

    // Redundant ROM installed, along with an image of the OEM ROM
    static const uint8_t data[] = {
        193, 0x0A, 0x00, 0xC1,
        0x01, 0x01, 0x02, 0x03, 0x04, 0x00,
        'U', '3', '2', 0x00,
        '0', '1', '/', '1', '0', '/', '2', '0', '2', '4', 0x00,
        'O', 'E', 'M', '.', 'B', 'I', 'N', 0x00,
        '0', '2', '/', '2', '0', '/', '2', '0', '2', '4', 0x00,
        0x00
    };

    const dmi_hpe_rom_info_t *info = test_hpe_decode(state, data, sizeof(data), &dmi_hpe_rom_info_spec);
    assert_true(info->is_redundant_rom);
    assert_true(info->has_redundant_rom_version);
    assert_true(info->has_oem_rom);
    assert_string_equal(info->oem_rom_filename, "OEM.BIN");
    assert_string_equal(info->oem_rom_date, "02/20/2024");
    assert_non_null(dmi_attribute_resolve(test_hpe_attribute(state, "redundant-rom-version"), info));
    assert_non_null(dmi_attribute_resolve(test_hpe_attribute(state, "oem-rom-filename"), info));
    assert_non_null(dmi_attribute_resolve(test_hpe_attribute(state, "oem-rom-date"), info));

    // Name of the image beginning with blanks tells there is none
    uint8_t blank[sizeof(data)];
    memcpy(blank, data, sizeof(blank));
    memcpy(blank + 0x19, "  ", 2);

    info = test_hpe_decode(state, blank, sizeof(blank), &dmi_hpe_rom_info_spec);
    assert_false(info->has_oem_rom);
    assert_null(dmi_attribute_resolve(test_hpe_attribute(state, "oem-rom-filename"), info));
    assert_null(dmi_attribute_resolve(test_hpe_attribute(state, "oem-rom-date"), info));

    // Version of the redundant ROM is not shown when there is none
    uint8_t single[sizeof(data)];
    memcpy(single, data, sizeof(single));
    single[0x04] = 0x00;

    info = test_hpe_decode(state, single, sizeof(single), &dmi_hpe_rom_info_spec);
    assert_false(info->is_redundant_rom);
    assert_false(info->has_redundant_rom_version);
    assert_null(dmi_attribute_resolve(test_hpe_attribute(state, "redundant-rom-version"), info));

    // Version of the redundant ROM is reserved from Gen12 onwards
    test_hpe_generation(state, DMI_HPE_GEN12);

    info = test_hpe_decode(state, data, sizeof(data), &dmi_hpe_rom_info_spec);
    assert_true(info->is_redundant_rom);
    assert_false(info->has_redundant_rom_version);
    assert_null(dmi_attribute_resolve(test_hpe_attribute(state, "redundant-rom-version"), info));
}

static void test_hpe_processor(void **pstate)
{
    test_state_t *state = *pstate;

    // Bootstrap processor in the x2APIC mode
    static const uint8_t data[] = {
        197, 0x10, 0x00, 0xC5,
        0x00, 0x04, 0x02, 0x03, 0x00, 0x01,
        0x5F, 0x00,
        0x20, 0x00, 0x00, 0x00,
        0x00, 0x00
    };

    const dmi_hpe_processor_t *info = test_hpe_decode(state, data, sizeof(data), &dmi_hpe_processor_spec);
    assert_true(info->is_x2apic);
    assert_int_equal(info->x2apic_id, 0x20);
    assert_non_null(dmi_attribute_resolve(test_hpe_attribute(state, "x2apic-id"), info));

    // x2APIC ID is not shown outside the x2APIC mode
    uint8_t xapic[sizeof(data)];
    memcpy(xapic, data, sizeof(xapic));
    xapic[0x07] = 0x01;

    info = test_hpe_decode(state, xapic, sizeof(xapic), &dmi_hpe_processor_spec);
    assert_false(info->is_x2apic);
    assert_null(dmi_attribute_resolve(test_hpe_attribute(state, "x2apic-id"), info));
}

static void test_hpe_dimm_location(void **pstate)
{
    test_state_t *state = *pstate;

    // Second DIMM of channel 1 on memory board 1, on an NVDIMM controller
    // of Micron
    static const uint8_t data[] = {
        202, 0x1C, 0x00, 0xCA,
        0x00, 0x11, 0x01, 0x03, 0x02, 0x05,
        0x00, 0x00, 0x00,
        0x00, 0x01, 0x04, 0x07,
        0x86, 0x80, 0x34, 0x12, 0x00, 0x2C, 0x78, 0x56,
        0x01, 0x00, 0x01,
        0x00, 0x00
    };

    const dmi_hpe_dimm_location_t *info = test_hpe_decode(state, data, sizeof(data), &dmi_hpe_dimm_location_spec);
    assert_int_equal(info->board, 1);
    assert_false(info->is_system_board);
    assert_true(info->has_ie);
    assert_int_equal(info->ie_dimm, 4);
    assert_int_equal(info->ie_pldm_id, 7);
    assert_int_equal(info->controller_vendor_id, 0x2C00);
    assert_true(info->has_channel_index);
    assert_int_equal(info->channel_index, 1);
    assert_non_null(dmi_attribute_resolve(test_hpe_attribute(state, "board"), info));
    assert_non_null(dmi_attribute_resolve(test_hpe_attribute(state, "ie-dimm"), info));
    assert_non_null(dmi_attribute_resolve(test_hpe_attribute(state, "channel-index"), info));
    assert_true(test_hpe_attribute(state, "controller-vendor-id")->params.flags & DMI_ATTRIBUTE_FLAG_JEP106);

    // Board number of 0xFF stands for the system board
    uint8_t system[sizeof(data)];
    memcpy(system, data, sizeof(system));
    system[0x06] = 0xFF;

    info = test_hpe_decode(state, system, sizeof(system), &dmi_hpe_dimm_location_spec);
    assert_true(info->is_system_board);
    assert_null(dmi_attribute_resolve(test_hpe_attribute(state, "board"), info));

    // Structures shorter than 28 bytes end before the index of the DIMM
    uint8_t shorter[sizeof(data) - 1];
    memcpy(shorter, data, 0x1B);
    memcpy(shorter + 0x1B, data + 0x1C, 2);
    shorter[0x01] = 0x1B;

    info = test_hpe_decode(state, shorter, sizeof(shorter), &dmi_hpe_dimm_location_spec);
    assert_false(info->has_channel_index);
    assert_null(dmi_attribute_resolve(test_hpe_attribute(state, "channel-index"), info));

    // Fields of the Innovation Engine are reserved from Gen12 onwards
    test_hpe_generation(state, DMI_HPE_GEN12);

    info = test_hpe_decode(state, data, sizeof(data), &dmi_hpe_dimm_location_spec);
    assert_false(info->has_ie);
    assert_null(dmi_attribute_resolve(test_hpe_attribute(state, "ie-dimm"), info));
    assert_null(dmi_attribute_resolve(test_hpe_attribute(state, "ie-pldm-id"), info));
}

static void test_hpe_generation(test_state_t *state, unsigned generation)
{
    dmi_platform_t *platform = dmi_platform_create(state->context);
    assert_non_null(platform);

    platform->firmware_vendor = DMI_VENDOR_HPE;
    platform->generation      = generation;
    assert_true(dmi_platform_set_family(platform, DMI_HPE_FAMILY_SERVER));
    assert_true(dmi_set_platform(state->context, platform));

    dmi_platform_destroy(platform);
}

static const void *test_hpe_decode(test_state_t *state, const void *data, size_t size,
                                   const dmi_entity_spec_t *spec)
{
    dmi_entity_destroy(state->entity);

    state->entity = dmi_test_entity_create(state->buffer, data, size);
    assert_non_null(state->entity);
    assert_true(dmi_entity_decode(state->entity));
    assert_ptr_equal(state->entity->spec, spec);
    assert_false(dmi_entity_is_incomplete(state->entity));

    // Layout covers every byte of the structure
    dmi_buffer_t *buffer = dmi_buffer_create(state->context);
    dmi_encoder_t encoder;

    assert_true(dmi_encoder_initialize(&encoder, buffer, state->entity, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE));
    assert_true(dmi_entity_encode(&encoder));
    assert_int_equal(buffer->length, state->entity->body_length);
    assert_memory_equal(buffer->data, data, buffer->length);
    dmi_encoder_finalize(&encoder);
    dmi_buffer_destroy(buffer);

    const void *info = dmi_entity_info(state->entity, spec->type);
    assert_non_null(info);

    return info;
}

static const dmi_attribute_t *test_hpe_attribute(test_state_t *state, const char *code)
{
    for (const dmi_attribute_t *attr = state->entity->spec->attributes; attr->params.name != nullptr; attr++) {
        if (strcmp(attr->params.code, code) == 0)
            return attr;
    }

    fail_msg("No attribute %s", code);
}
