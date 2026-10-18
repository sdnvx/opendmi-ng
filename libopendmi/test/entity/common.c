//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/internal.h>
#include <opendmi/entity/common.h>

#include "entity/common-internal.h"

static void test_status_name(void **pstate);
static void test_error_correct_type_name(void **pstate);
static void test_pci_addr(void **pstate);
static void test_pci_addr_arguments(void **pstate);

// Structure holding a PCI address, and the field carrying it, which is
// declared the way the specifications declare theirs
typedef struct test_device
{
    dmi_pci_addr_t address;
} test_device_t;

static const dmi_field_t test_address_field = DMI_FIELD(test_device_t, address, dmi_dword_t,
                                                        .decode = dmi_pci_addr_decode,
                                                        .encode = dmi_pci_addr_encode);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_status_name),
        cmocka_unit_test(test_error_correct_type_name),
        cmocka_unit_test(test_pci_addr),
        cmocka_unit_test(test_pci_addr_arguments)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_status_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_status_name(__DMI_STATUS_COUNT));

    for (int i = 0; i < __DMI_STATUS_COUNT; i++) {
        assert_non_null(dmi_status_name(i));
    }
}

static void test_error_correct_type_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_error_correct_type_name(__DMI_ERROR_CORRECT_TYPE_COUNT));

    for (int i = 0; i < __DMI_ERROR_CORRECT_TYPE_COUNT; i++) {
        assert_non_null(dmi_error_correct_type_name(i));
    }
}

static void test_pci_addr(void **pstate)
{
    dmi_unused(pstate);

    // Address is carried as a double word of the segment group, the bus, and
    // the device and function packed into one byte: function 2 of device 29
    // on bus 0x12 of segment group 1 here
    dmi_field_data_t data = { .number = 0xEA120001 };
    test_device_t device = {};

    assert_true(dmi_pci_addr_decode(&test_address_field, &data, &device.address));

    assert_int_equal(device.address.segment_group, 0x0001);
    assert_int_equal(device.address.bus_number, 0x12);
    assert_int_equal(device.address.device_number, 29);
    assert_int_equal(device.address.function_number, 2);

    // Encoding undoes the decoding
    data.number = 0;

    assert_true(dmi_pci_addr_encode(&test_address_field, &device.address, &data));
    assert_int_equal(data.number, 0xEA120001);

    // Address of no bus carries no device and function either, whose bits
    // would spell device 31 and function 7 otherwise
    data.number = 0xFFFFFFFF;

    assert_true(dmi_pci_addr_decode(&test_address_field, &data, &device.address));

    assert_int_equal(device.address.segment_group, UINT16_MAX);
    assert_int_equal(device.address.bus_number, UINT8_MAX);
    assert_int_equal(device.address.device_number, UINT8_MAX);
    assert_int_equal(device.address.function_number, UINT8_MAX);

    data.number = 0;

    assert_true(dmi_pci_addr_encode(&test_address_field, &device.address, &data));
    assert_int_equal(data.number, 0xFFFFFFFF);
}

static void test_pci_addr_arguments(void **pstate)
{
    dmi_unused(pstate);

    dmi_field_data_t data = { .number = 0xEA120001 };
    test_device_t device = {};

    // Data to decode and the address to decode it into are both required
    assert_false(dmi_pci_addr_decode(&test_address_field, nullptr, &device.address));
    assert_false(dmi_pci_addr_decode(&test_address_field, &data, nullptr));
    assert_int_equal(device.address.bus_number, 0);

    // So are the address to encode and the data to encode it into
    assert_false(dmi_pci_addr_encode(&test_address_field, nullptr, &data));
    assert_false(dmi_pci_addr_encode(&test_address_field, &device.address, nullptr));
    assert_int_equal(data.number, 0xEA120001);
}
