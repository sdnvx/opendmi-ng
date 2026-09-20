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

#include <opendmi/utils/uuid.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/error.h>
#include <opendmi/log.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/mgmt-controller.h>

static int test_mgmt_controller_setup(void **pstate);
static int test_mgmt_controller_teardown(void **pstate);

static void test_mgmt_controller_decode_v30(void **pstate);
static void test_mgmt_controller_decode_records(void **pstate);
static void test_mgmt_controller_decode_no_records(void **pstate);
static void test_mgmt_controller_decode_truncated(void **pstate);
static void test_mgmt_controller_decode_nhi_usb(void **pstate);
static void test_mgmt_controller_decode_nhi_usb_v2(void **pstate);
static void test_mgmt_controller_decode_nhi_pci_v2(void **pstate);
static void test_mgmt_controller_decode_nhi_oem(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_mgmt_controller_decode_v30, test_mgmt_controller_setup, test_mgmt_controller_teardown),
        cmocka_unit_test_setup_teardown(test_mgmt_controller_decode_records, test_mgmt_controller_setup, test_mgmt_controller_teardown),
        cmocka_unit_test_setup_teardown(test_mgmt_controller_decode_no_records, test_mgmt_controller_setup, test_mgmt_controller_teardown),
        cmocka_unit_test_setup_teardown(test_mgmt_controller_decode_truncated, test_mgmt_controller_setup, test_mgmt_controller_teardown),
        cmocka_unit_test_setup_teardown(test_mgmt_controller_decode_nhi_usb, test_mgmt_controller_setup, test_mgmt_controller_teardown),
        cmocka_unit_test_setup_teardown(test_mgmt_controller_decode_nhi_usb_v2, test_mgmt_controller_setup, test_mgmt_controller_teardown),
        cmocka_unit_test_setup_teardown(test_mgmt_controller_decode_nhi_pci_v2, test_mgmt_controller_setup, test_mgmt_controller_teardown),
        cmocka_unit_test_setup_teardown(test_mgmt_controller_decode_nhi_oem, test_mgmt_controller_setup, test_mgmt_controller_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_mgmt_controller_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_mgmt_controller_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

// Create entity from structure body, followed by empty string set
static dmi_entity_t *create_entity(dmi_context_t *context, const uint8_t *body, size_t length)
{
    static uint8_t data[256];

    assert_true(length + 4 + 2 <= sizeof(data));

    data[0] = 42;
    data[1] = (uint8_t)(length + 4);
    data[2] = 0x00;
    data[3] = 0x10;
    memcpy(data + 4, body, length);
    data[length + 4] = 0;
    data[length + 5] = 0;

    dmi_entity_t *entity = dmi_entity_create(context, data, length + 6);
    assert_non_null(entity);

    return entity;
}

static void test_mgmt_controller_decode_v30(void **pstate)
{
    dmi_context_t *context = *pstate;

    // SMBIOS 3.0 structure without protocol records
    static const uint8_t body[] = {
        0x40, 0x03, 0xAA, 0xBB, 0xCC
    };

    dmi_entity_t *entity = create_entity(context, body, sizeof(body));
    assert_true(dmi_entity_decode(entity));

    const dmi_mgmt_controller_t *info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);

    assert_int_equal(info->if_type, DMI_MGMT_IF_TYPE_NETWORK_HOST_IF);
    assert_int_equal(info->if_data.length, 3);
    assert_memory_equal(info->if_data.data, body + 2, 3);
    assert_int_equal(info->proto_records_count, 0);
    assert_null(info->proto_records);

    dmi_entity_destroy(entity);
}

static void test_mgmt_controller_decode_records(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const uint8_t body[] = {
        0x40, 0x02, 0x11, 0x22,         // Interface type and data
        0x02,                           // Number of protocol records
        0x04, 0x03, 0x01, 0x02, 0x03,   // Redfish over IP record
        0xF0, 0x00                      // OEM record without data
    };

    dmi_entity_t *entity = create_entity(context, body, sizeof(body));
    assert_true(dmi_entity_decode(entity));

    const dmi_mgmt_controller_t *info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);

    assert_int_equal(info->if_data.length, 2);
    assert_memory_equal(info->if_data.data, body + 2, 2);
    assert_int_equal(info->proto_records_count, 2);

    assert_int_equal(info->proto_records[0].type, DMI_MGMT_PROTO_REDFISH_OVER_IP);
    assert_int_equal(info->proto_records[0].data.length, 3);
    assert_memory_equal(info->proto_records[0].data.data, body + 7, 3);

    assert_int_equal(info->proto_records[1].type, DMI_MGMT_PROTO_OEM);
    assert_int_equal(info->proto_records[1].data.length, 0);

    dmi_entity_destroy(entity);
}

static void test_mgmt_controller_decode_no_records(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Zero interface data length and zero protocol records
    static const uint8_t body[] = { 0x40, 0x00, 0x00 };

    dmi_entity_t *entity = create_entity(context, body, sizeof(body));
    assert_true(dmi_entity_decode(entity));

    const dmi_mgmt_controller_t *info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);

    assert_int_equal(info->if_data.length, 0);
    assert_null(info->if_data.data);
    assert_int_equal(info->proto_records_count, 0);

    dmi_entity_destroy(entity);
}

static void test_mgmt_controller_decode_truncated(void **pstate)
{
    dmi_context_t *context = *pstate;
    const dmi_mgmt_controller_t *info;
    dmi_entity_t *entity;

    // Interface data length exceeds structure length (seen in real firmware)
    entity = create_entity(context, (const uint8_t[]){ 0x02, 0xFF, 0x01, 0x02, 0xFF, 0xFF, 0xFF, 0xFF }, 8);
    assert_true(dmi_entity_decode(entity));

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);
    assert_int_equal(info->if_type, DMI_MGMT_IF_TYPE_MCTP_KCS);
    assert_int_equal(info->if_data.length, 0);
    assert_null(info->if_data.data);
    assert_int_equal(info->proto_records_count, 0);

    dmi_entity_destroy(entity);

    // Number of protocol records exceeds structure length
    entity = create_entity(context, (const uint8_t[]){ 0x40, 0x00, 0xFF, 0x04, 0x00 }, 5);
    assert_true(dmi_entity_decode(entity));

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);
    assert_int_equal(info->proto_records_count, 1);
    assert_int_equal(info->proto_records[0].type, DMI_MGMT_PROTO_REDFISH_OVER_IP);
    assert_int_equal(info->proto_records[0].data.length, 0);

    dmi_entity_destroy(entity);

    // Protocol record header is truncated
    entity = create_entity(context, (const uint8_t[]){ 0x40, 0x00, 0x01, 0x04 }, 4);
    assert_true(dmi_entity_decode(entity));

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);
    assert_int_equal(info->proto_records_count, 0);

    dmi_entity_destroy(entity);

    // Protocol record data exceeds structure length
    entity = create_entity(context, (const uint8_t[]){
        0x40, 0x01, 0x11, 0x02, 0x04, 0x01, 0xAA, 0x03, 0xFF, 0xBB
    }, 10);
    assert_true(dmi_entity_decode(entity));

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);
    assert_int_equal(info->if_data.length, 1);
    assert_int_equal(info->proto_records_count, 1);
    assert_int_equal(info->proto_records[0].type, DMI_MGMT_PROTO_REDFISH_OVER_IP);
    assert_int_equal(info->proto_records[0].data.length, 1);
    assert_int_equal(info->proto_records[0].data.data[0], 0xAA);

    dmi_entity_destroy(entity);
}

static void test_mgmt_controller_decode_nhi_usb(void **pstate)
{
    dmi_context_t *context = *pstate;

    // USB network interface, the example from DSP0270
    static const uint8_t body[] = {
        0x40, 0x15,
        0x02, 0xBB, 0xAA, 0xDD, 0xCC, 0x10, 0x03,
        'S', 0x00, 'N', 0x00, '0', 0x00, '0', 0x00, '0', 0x00, '0', 0x00, '1', 0x00,
        0x00
    };

    dmi_entity_t *entity = create_entity(context, body, sizeof(body));
    assert_true(dmi_entity_decode(entity));

    const dmi_mgmt_controller_t *info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);

    assert_true(info->has_nhi);
    assert_int_equal(info->nhi.device_type, DMI_MGMT_NHI_DEVICE_TYPE_USB);
    assert_int_equal(info->nhi.format, DMI_MGMT_NHI_FORMAT_USB);
    assert_int_equal(info->nhi.descriptor.length, 0x14);
    assert_int_equal(info->nhi.usb.vendor_id, 0xAABB);
    assert_int_equal(info->nhi.usb.product_id, 0xCCDD);
    assert_non_null(info->nhi.usb.serial_number);
    assert_string_equal(info->nhi.usb.serial_number, "SN00001");
    assert_int_equal(info->proto_records_count, 0);

    dmi_entity_destroy(entity);

    // Serial number exceeds the descriptor
    static const uint8_t malformed[] = {
        0x40, 0x07, 0x02, 0xBB, 0xAA, 0xDD, 0xCC, 0x20, 0x03, 0x00
    };

    entity = create_entity(context, malformed, sizeof(malformed));
    assert_true(dmi_entity_decode(entity));

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);

    assert_true(info->has_nhi);
    assert_int_equal(info->nhi.format, DMI_MGMT_NHI_FORMAT_RAW);
    assert_int_equal(info->nhi.descriptor.length, 6);
    assert_null(info->nhi.usb.serial_number);

    dmi_entity_destroy(entity);
}

static void test_mgmt_controller_decode_nhi_usb_v2(void **pstate)
{
    dmi_context_t *context = *pstate;

    // USB network interface v2, DSP0270 1.2 layout without characteristics
    static const uint8_t body[] = {
        0x40, 0x0D,
        0x04, 0x0D, 0xBB, 0xAA, 0xDD, 0xCC, 0x00,
        0x02, 0x11, 0x22, 0x33, 0x44, 0x55,
        0x00
    };

    dmi_entity_t *entity = create_entity(context, body, sizeof(body));
    assert_true(dmi_entity_decode(entity));

    const dmi_mgmt_controller_t *info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);

    assert_true(info->has_nhi);
    assert_int_equal(info->nhi.format, DMI_MGMT_NHI_FORMAT_USB_V2);
    assert_int_equal(info->nhi.usb_v2.vendor_id, 0xAABB);
    assert_int_equal(info->nhi.usb_v2.product_id, 0xCCDD);
    assert_null(info->nhi.usb_v2.serial_number);
    assert_int_equal(info->nhi.usb_v2.mac_address.length, 6);
    assert_memory_equal(info->nhi.usb_v2.mac_address.data, body + 9, 6);
    assert_int_equal(info->nhi.usb_v2.characteristics, 0);
    assert_int_equal(info->nhi.usb_v2.credential_handle, DMI_HANDLE_INVALID);

    dmi_entity_destroy(entity);
}

static void test_mgmt_controller_decode_nhi_pci_v2(void **pstate)
{
    dmi_context_t *context = *pstate;

    // PCI/PCIe network interface v2 with Redfish over IP record
    static const uint8_t body[] = {
        0x40, 0x18,
        0x05, 0x18, 0x86, 0x80, 0x3B, 0x15, 0xD9, 0x15, 0x5B, 0x1F,
        0x00, 0x25, 0x90, 0xAA, 0xBB, 0xCC,
        0x00, 0x00, 0x3A, 0x19, 0x01, 0x00, 0x34, 0x12,
        0x01,
        0x04, 0x63,
        // Service UUID
        0x33, 0x22, 0x11, 0x00, 0x55, 0x44, 0x77, 0x66,
        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        // Host IP assignment type and address format
        0x02, 0x01,
        0x0A, 0x0C, 0x6E, 0x39, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        // Service IP discovery type and address format
        0x01, 0x02,
        0x20, 0x01, 0x0D, 0xB8, 0x63, 0xB3, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x90,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        // Port, VLAN ID and hostname padded with NULL characters
        0xBB, 0x01, 0x0A, 0x00, 0x00, 0x00,
        0x08, 'b', 'm', 'c', '.', 'l', 'a', 'n', 0x00
    };

    static const uint8_t uuid[] = {
        0x33, 0x22, 0x11, 0x00, 0x55, 0x44, 0x77, 0x66,
        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };

    dmi_entity_t *entity = create_entity(context, body, sizeof(body));
    assert_true(dmi_entity_decode(entity));

    const dmi_mgmt_controller_t *info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);

    assert_true(info->has_nhi);
    assert_int_equal(info->nhi.format, DMI_MGMT_NHI_FORMAT_PCI_V2);

    const dmi_mgmt_nhi_pci_v2_t *pci = &info->nhi.pci_v2;
    assert_int_equal(pci->vendor_id, 0x8086);
    assert_int_equal(pci->device_id, 0x153B);
    assert_int_equal(pci->subsys_vendor_id, 0x15D9);
    assert_int_equal(pci->subsys_id, 0x1F5B);
    assert_int_equal(pci->mac_address.length, 6);
    assert_memory_equal(pci->mac_address.data, body + 12, 6);
    assert_int_equal(pci->segment_group, 0);
    assert_int_equal(pci->bus_number, 0x3A);
    assert_int_equal(pci->device_number, 3);
    assert_int_equal(pci->function_number, 1);
    assert_int_equal(pci->characteristics, 1 << DMI_MGMT_NHI_CHAR_CREDENTIAL_BOOTSTRAPPING);
    assert_int_equal(pci->credential_handle, 0x1234);

    assert_int_equal(info->proto_records_count, 1);

    const dmi_mgmt_proto_record_t *record = &info->proto_records[0];
    assert_int_equal(record->type, DMI_MGMT_PROTO_REDFISH_OVER_IP);
    assert_int_equal(record->data.length, 0x63);
    assert_true(record->has_redfish);

    const dmi_mgmt_redfish_over_ip_t *redfish = &record->redfish;
    uint8_t service_uuid[16];

    dmi_uuid_encode(redfish->service_uuid, service_uuid);
    assert_memory_equal(service_uuid, uuid, sizeof(uuid));

    assert_int_equal(redfish->host_ip_assignment, DMI_MGMT_REDFISH_IP_ASSIGNMENT_DHCP);
    assert_int_equal(redfish->host_ip_format, DMI_MGMT_REDFISH_IP_FORMAT_IPV4);
    assert_int_equal(redfish->host_ip_address.length, 4);
    assert_memory_equal(redfish->host_ip_address.data, body + 47, 4);
    assert_int_equal(redfish->host_ip_mask.length, 4);
    assert_memory_equal(redfish->host_ip_mask.data, body + 63, 4);
    assert_int_equal(redfish->service_ip_discovery, DMI_MGMT_REDFISH_IP_ASSIGNMENT_STATIC);
    assert_int_equal(redfish->service_ip_format, DMI_MGMT_REDFISH_IP_FORMAT_IPV6);
    assert_int_equal(redfish->service_ip_address.length, 16);
    assert_memory_equal(redfish->service_ip_address.data, body + 81, 16);
    assert_int_equal(redfish->service_ip_mask.length, 16);
    assert_int_equal(redfish->service_ip_port, 443);
    assert_int_equal(redfish->service_vlan_id, 10);
    assert_non_null(redfish->service_hostname);
    assert_string_equal(redfish->service_hostname, "bmc.lan");

    dmi_entity_destroy(entity);
}

static void test_mgmt_controller_decode_nhi_oem(void **pstate)
{
    dmi_context_t *context = *pstate;

    // OEM device with truncated Redfish over IP record and OEM record
    static const uint8_t body[] = {
        0x40, 0x07, 0x85, 0x00, 0x00, 0x01, 0x57, 0xDE, 0xAD,
        0x02,
        0x04, 0x03, 0xAA, 0xBB, 0xCC,
        0xF0, 0x01, 0xFF
    };

    dmi_entity_t *entity = create_entity(context, body, sizeof(body));
    assert_true(dmi_entity_decode(entity));

    const dmi_mgmt_controller_t *info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);

    assert_true(info->has_nhi);
    assert_int_equal(info->nhi.device_type, 0x85);
    assert_int_equal(info->nhi.format, DMI_MGMT_NHI_FORMAT_OEM);
    assert_int_equal(info->nhi.oem.vendor_iana, 343);
    assert_int_equal(info->nhi.oem.vendor_data.length, 2);
    assert_memory_equal(info->nhi.oem.vendor_data.data, body + 7, 2);

    // Truncated record is left undecoded, and the following one is decoded
    assert_int_equal(info->proto_records_count, 2);
    assert_int_equal(info->proto_records[0].type, DMI_MGMT_PROTO_REDFISH_OVER_IP);
    assert_false(info->proto_records[0].has_redfish);
    assert_int_equal(info->proto_records[0].data.length, 3);
    assert_int_equal(info->proto_records[1].type, DMI_MGMT_PROTO_OEM);
    assert_int_equal(info->proto_records[1].data.length, 1);

    dmi_entity_destroy(entity);
}
