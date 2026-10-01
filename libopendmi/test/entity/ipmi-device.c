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
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/ipmi-device.h>

static void test_ipmi_addr_type_name(void **pstate);
static void test_ipmi_device_decode_io(void **pstate);
static void test_ipmi_device_decode_memory(void **pstate);
static void test_ipmi_device_decode_ssif(void **pstate);
static void test_ipmi_device_decode_short(void **pstate);
static void test_ipmi_device_variants(void **pstate);

static const dmi_attribute_t *test_attribute(const char *code);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_ipmi_addr_type_name),
        cmocka_unit_test(test_ipmi_device_decode_io),
        cmocka_unit_test(test_ipmi_device_decode_memory),
        cmocka_unit_test(test_ipmi_device_decode_ssif),
        cmocka_unit_test(test_ipmi_device_decode_short),
        cmocka_unit_test(test_ipmi_device_variants)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void decode_ipmi_device_ex(
        size_t                length,
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
        38, (uint8_t)length, 0x00, 0x10,    // Header
        interface_type, 0x20, 0x20, 0xFF    // Interface, revision, I2C address, NV storage
    };

    for (size_t i = 0; i < 8; i++)
        data[0x08 + i] = (base_addr >> (8 * i)) & 0xFF;

    if (length > 0x10) {
        data[0x10] = modifier;
        data[0x11] = 0x00;                  // Interrupt number
    }

    dmi_buffer_t *entity_buffer = dmi_buffer_create(context);

    dmi_entity_t *entity = dmi_test_entity_create(entity_buffer, data, length + 2);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_false(entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    // Structure is written back as it has been read
    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_encoder_t encoder;

    assert_true(dmi_encoder_initialize(&encoder, buffer, entity, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE));
    assert_true(dmi_entity_encode(&encoder));
    assert_int_equal(buffer->length, entity->body_length);
    assert_memory_equal(buffer->data, data, buffer->length);
    dmi_encoder_finalize(&encoder);
    dmi_buffer_destroy(buffer);

    const dmi_ipmi_device_t *info = dmi_entity_info(entity, DMI_TYPE(ipmi_device));
    assert_non_null(info);

    *result = *info;

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
    dmi_destroy(context);
}

static void decode_ipmi_device(
        dmi_ipmi_interface_t  interface_type,
        uint64_t              base_addr,
        uint8_t               modifier,
        dmi_ipmi_device_t    *result)
{
    decode_ipmi_device_ex(0x12, interface_type, base_addr, modifier, result);
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

static void test_ipmi_device_decode_short(void **pstate)
{
    dmi_unused(pstate);

    dmi_ipmi_device_t info;

    // Structure of 16 bytes, which IPMI specification allows, carries no
    // base address modifier and no interrupt information
    decode_ipmi_device_ex(0x10, DMI_IPMI_INTERFACE_KCS, 0xCA3, 0x00, &info);
    assert_int_equal(info.base_addr_type, DMI_IPMI_ADDR_TYPE_IO);
    assert_uint_equal(info.base_addr, 0xCA2);
    assert_false(info.base_addr_lsb);
    assert_int_equal(info.intr_trigger, DMI_IPMI_INTR_TRIGGER_UNSPEC);
    assert_int_equal(info.intr_polarity, DMI_IPMI_INTR_POLARITY_UNSPEC);
    assert_int_equal(info.register_spacing, 0);
    assert_int_equal(info.intr_number, 0);

    decode_ipmi_device_ex(0x10, DMI_IPMI_INTERFACE_SSIF, 0x21, 0x00, &info);
    assert_int_equal(info.base_addr_type, DMI_IPMI_ADDR_TYPE_SMBUS);
    assert_uint_equal(info.base_addr, 0x10);
}

static void test_ipmi_device_variants(void **pstate)
{
    dmi_unused(pstate);

    dmi_ipmi_device_t kcs;
    dmi_ipmi_device_t ssif;

    decode_ipmi_device(DMI_IPMI_INTERFACE_KCS, 0xCA3, 0x00, &kcs);
    decode_ipmi_device(DMI_IPMI_INTERFACE_SSIF, 0x21, 0x00, &ssif);

    const dmi_attribute_t *base_addr = test_attribute("base-address");
    const dmi_attribute_t *base_addr_lsb = test_attribute("base-address-lsb");
    const dmi_attribute_t *register_spacing = test_attribute("register-spacing");

    assert_non_null(base_addr);
    assert_non_null(base_addr_lsb);
    assert_non_null(register_spacing);

    // Address in I/O space is shown as an address, with register details
    assert_int_equal(dmi_attribute_resolve(base_addr, &kcs)->type, DMI_ATTRIBUTE_TYPE_ADDRESS);
    assert_non_null(dmi_attribute_resolve(base_addr_lsb, &kcs));
    assert_non_null(dmi_attribute_resolve(register_spacing, &kcs));

    // SMBus target address is shown as a number, without register details
    assert_int_equal(dmi_attribute_resolve(base_addr, &ssif)->type, DMI_ATTRIBUTE_TYPE_INTEGER);
    assert_null(dmi_attribute_resolve(base_addr_lsb, &ssif));
    assert_null(dmi_attribute_resolve(register_spacing, &ssif));
}

static const dmi_attribute_t *test_attribute(const char *code)
{
    for (const dmi_attribute_t *attr = dmi_ipmi_device_spec.attributes; attr->params.name != nullptr; attr++) {
        if (strcmp(attr->params.code, code) == 0)
            return attr;
    }

    return nullptr;
}

