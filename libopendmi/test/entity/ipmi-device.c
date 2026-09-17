//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/ipmi-device.h>

static void test_ipmi_addr_type_name(void **pstate);
static void test_ipmi_device_decode_io(void **pstate);
static void test_ipmi_device_decode_memory(void **pstate);
static void test_ipmi_device_decode_ssif(void **pstate);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_ipmi_addr_type_name),
        cmocka_unit_test(test_ipmi_device_decode_io),
        cmocka_unit_test(test_ipmi_device_decode_memory),
        cmocka_unit_test(test_ipmi_device_decode_ssif)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void decode_ipmi_device(
        dmi_ipmi_interface_t  interface_type,
        uint64_t              base_addr,
        uint8_t               modifier,
        dmi_ipmi_device_t    *result)
{
    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    // IPMI device information structure without strings
    uint8_t data[0x12 + 2] = {
        38, 0x12, 0x00, 0x10,               // Header
        interface_type, 0x20, 0x20, 0xFF    // Interface, revision, I2C address, NV storage
    };

    for (size_t i = 0; i < 8; i++)
        data[0x08 + i] = (base_addr >> (8 * i)) & 0xFF;

    data[0x10] = modifier;
    data[0x11] = 0x00;                      // Interrupt number

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_ipmi_device_t *info = dmi_entity_info(entity, DMI_TYPE(IPMI_DEVICE));
    assert_non_null(info);

    *result = *info;

    dmi_entity_destroy(entity);
    dmi_destroy(context);
}

static void test_ipmi_addr_type_name(void **pstate)
{
    dmi_unused(pstate);

    assert_non_null(dmi_ipmi_addr_type_name(DMI_IPMI_ADDR_TYPE_MEMORY));
    assert_non_null(dmi_ipmi_addr_type_name(DMI_IPMI_ADDR_TYPE_IO));
    assert_non_null(dmi_ipmi_addr_type_name(DMI_IPMI_ADDR_TYPE_SMBUS));
}

static void test_ipmi_device_decode_io(void **pstate)
{
    dmi_unused(pstate);

    dmi_ipmi_device_t info;

    // Typical KCS interface at I/O port 0xCA2
    decode_ipmi_device(DMI_IPMI_INTERFACE_KCS, 0xCA3, 0x00, &info);
    assert_int_equal(info.base_addr_type, DMI_IPMI_ADDR_TYPE_IO);
    assert_uint_equal(info.base_addr, 0xCA2);
    assert_false(info.base_addr_lsb);
    assert_int_equal(info.register_spacing, 1);

    // Address LSB is taken from base address modifier, 4-byte register spacing
    decode_ipmi_device(DMI_IPMI_INTERFACE_KCS, 0xCA9, 0x50, &info);
    assert_int_equal(info.base_addr_type, DMI_IPMI_ADDR_TYPE_IO);
    assert_uint_equal(info.base_addr, 0xCA9);
    assert_true(info.base_addr_lsb);
    assert_int_equal(info.register_spacing, 4);

    decode_ipmi_device(DMI_IPMI_INTERFACE_KCS, 0xCA9, 0x40, &info);
    assert_int_equal(info.base_addr_type, DMI_IPMI_ADDR_TYPE_IO);
    assert_uint_equal(info.base_addr, 0xCA8);
}

static void test_ipmi_device_decode_memory(void **pstate)
{
    dmi_unused(pstate);

    dmi_ipmi_device_t info;

    decode_ipmi_device(DMI_IPMI_INTERFACE_BT, 0xFED40000, 0x00, &info);
    assert_int_equal(info.base_addr_type, DMI_IPMI_ADDR_TYPE_MEMORY);
    assert_uint_equal(info.base_addr, 0xFED40000);

    // Bit 63 is a regular address bit
    decode_ipmi_device(DMI_IPMI_INTERFACE_BT, 0x8000000000001000u, 0x10, &info);
    assert_int_equal(info.base_addr_type, DMI_IPMI_ADDR_TYPE_MEMORY);
    assert_uint_equal(info.base_addr, 0x8000000000001001u);
}

static void test_ipmi_device_decode_ssif(void **pstate)
{
    dmi_unused(pstate);

    dmi_ipmi_device_t info;

    // SSIF interface uses SMBus target address, shifted left by one bit
    decode_ipmi_device(DMI_IPMI_INTERFACE_SSIF, 0x21, 0x00, &info);
    assert_int_equal(info.base_addr_type, DMI_IPMI_ADDR_TYPE_SMBUS);
    assert_uint_equal(info.base_addr, 0x10);
}
