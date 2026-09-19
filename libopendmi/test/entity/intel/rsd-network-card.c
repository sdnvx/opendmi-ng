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
#include <opendmi/utils.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/intel/rsd-network-card.h>

static int test_rsd_network_card_setup(void **pstate);
static int test_rsd_network_card_teardown(void **pstate);

static void test_rsd_network_card_decode(void **pstate);
static void test_rsd_network_card_decode_short(void **pstate);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

// Offset of MAC address field
#define TEST_MAC_ADDRESS_OFFSET 0x19

// Intel RSD network card information structure (0x3A bytes) followed by
// strings
static const uint8_t test_data[] = {
    190, 0x3A, 0x00, 0x20,          // Header
    0x02,                           // PCI class
    0x03, 0x00,                     // PCI slot ID
    0x86, 0x80, 0x93, 0x15,         // Vendor ID, device ID
    0x86, 0x80, 0x01, 0x00,         // Sub-vendor ID, sub-device ID
    0x10, 0x27, 0x00, 0x00,         // Maximum speed (10000 Mbps)
    0xE8, 0x03, 0x00, 0x00,         // Current speed (1000 Mbps)
    0x01, 0x00,                     // Port index
    0x00, 0x1B, 0x21, 0x3C, 0x4D, 0x5E, 0x00, 0x00, // MAC address (32 bytes)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01,                           // Firmware version
    '1', '.', '2', '.', '3', 0,
    0
};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_rsd_network_card_decode,
                                        test_rsd_network_card_setup, test_rsd_network_card_teardown),
        cmocka_unit_test_setup_teardown(test_rsd_network_card_decode_short,
                                        test_rsd_network_card_setup, test_rsd_network_card_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_rsd_network_card_setup(void **pstate)
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

static int test_rsd_network_card_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_rsd_network_card_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    dmi_entity_t *entity = dmi_entity_create(context, test_data, sizeof(test_data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_intel_rsd_network_card_t *info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_NETWORK_CARD));
    assert_non_null(info);

    assert_int_equal(info->pci_class, 0x02);
    assert_int_equal(info->vendor_id, 0x8086);
    assert_int_equal(info->device_id, 0x1593);
    assert_int_equal(info->maximum_speed, 10000);
    assert_int_equal(info->current_speed, 1000);
    assert_int_equal(info->port_index, 1);
    assert_string_equal(info->firmware_version, "1.2.3");

    // MAC address is referenced in the structure data
    assert_int_equal(info->mac_address.length, DMI_INTEL_RSD_MAC_ADDRESS_LENGTH);
    assert_ptr_equal(info->mac_address.data, entity->data + TEST_MAC_ADDRESS_OFFSET);
    assert_int_equal(info->mac_address.data[1], 0x1B);

    // MAC address is formatted without padding
    const dmi_attribute_t *attr = entity->spec->attributes;
    while ((attr->params.code != nullptr) and (strcmp(attr->params.code, "mac-address") != 0))
        attr++;

    assert_non_null(attr->params.code);

    char *mac = dmi_attribute_format(context, attr, &info->mac_address, true);
    bool valid = (mac != nullptr) and (strcmp(mac, "00:1B:21:3C:4D:5E") == 0);
    dmi_free(mac);

    assert_true(valid);

    dmi_entity_destroy(entity);
}

static void test_rsd_network_card_decode_short(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Structure ends in the middle of MAC address, which is shorter than the
    // minimum length
    uint8_t data[sizeof(test_data)];
    memcpy(data, test_data, sizeof(data));
    data[1] = TEST_MAC_ADDRESS_OFFSET + 6;

    // String set follows the shortened structure
    data[TEST_MAC_ADDRESS_OFFSET + 6]     = 0;
    data[TEST_MAC_ADDRESS_OFFSET + 6 + 1] = 0;

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);
    assert_false(dmi_entity_decode(entity));

    dmi_entity_destroy(entity);
}
