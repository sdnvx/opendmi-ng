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
#include <opendmi/entity/hpe/dimm-vendor.h>
#include <opendmi/entity/hpe/drive.h>
#include <opendmi/entity/hpe/extension-board.h>
#include <opendmi/entity/hpe/inventory.h>
#include <opendmi/entity/hpe/nic-mac.h>
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
static void test_hpe_dimm_attrs(void **pstate);
static void test_hpe_nic_mac(void **pstate);
static void test_hpe_backplane(void **pstate);
static void test_hpe_dimm_vendor(void **pstate);
static void test_hpe_usb(void **pstate);
static void test_hpe_inventory(void **pstate);
static void test_hpe_drive(void **pstate);
static void test_hpe_dimm_config(void **pstate);
static void test_hpe_extension_board(void **pstate);

static void test_hpe_generation(test_state_t *state, unsigned generation);
static const void *test_hpe_decode(test_state_t *state, const void *data, size_t size,
                                   const dmi_entity_spec_t *spec);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_hpe_device_correlation, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_version, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_dimm_attrs, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_nic_mac, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_backplane, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_dimm_vendor, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_usb, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_inventory, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_drive, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_dimm_config, test_hpe_setup, test_hpe_teardown),
        cmocka_unit_test_setup_teardown(test_hpe_extension_board, test_hpe_setup, test_hpe_teardown)
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

    // Port of a NIC in slot 1, bifurcated from the slot of handle 0x0901
    static const uint8_t data[] = {
        203, 0x28, 0x00, 0xCB,
        0x00, 0x09, 0xFE, 0xFF,
        0x86, 0x80, 0x72, 0x15, 0x3C, 0x10, 0xFC, 0x22,
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
    assert_int_equal(info->pci_subdevice_id, 0x22FC);
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
    assert_int_equal(info->form_factor, DMI_HPE_DRIVE_FORM_2_5);
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
    assert_int_equal(info->cpld_version, 0x83);
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
    assert_false(state->entity->state & DMI_ENTITY_STATE_INCOMPLETE);

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
