//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/ipmi-device.h>

static bool dmi_ipmi_device_decode(dmi_entity_t *entity);

static const dmi_name_set_t dmi_ipmi_interface_names =
{
    .code  = "ipmi-interface",
    .names = (dmi_name_t[]){
        DMI_NAME_UNKNOWN(DMI_IPMI_INTERFACE_UNKNOWN),
        {
            .id   = DMI_IPMI_INTERFACE_KCS,
            .code = "kcs",
            .name = "KCS: Keyboard Controller Style"
        },
        {
            .id   = DMI_IPMI_INTERFACE_SMIC,
            .code = "smic",
            .name = "SMIC: Server Management Interface Chip"
        },
        {
            .id   = DMI_IPMI_INTERFACE_BT,
            .code = "bt",
            .name = "BT: Block Transfer"
        },
        {
            .id   = DMI_IPMI_INTERFACE_SSIF,
            .code = "ssif",
            .name = "SSIF: SMBus System Interface"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_ipmi_addr_type_names =
{
    .code  = "ipmi-addr-type",
    .names = (dmi_name_t[]){
        {
            .id   = DMI_IPMI_ADDR_TYPE_MEMORY,
            .code = "memory",
            .name = "Memory"

        },
        {
            .id   = DMI_IPMI_ADDR_TYPE_IO,
            .code = "io",
            .name = "I/O"
        },
        {
            .id   = DMI_IPMI_ADDR_TYPE_SMBUS,
            .code = "smbus",
            .name = "SMBus"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_ipmi_intr_trigger_names =
{
    .code  = "ipmi-intr-trigger",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_IPMI_INTR_TRIGGER_UNSPEC),
        {
            .id   = DMI_IPMI_INTR_TRIGGER_EDGE,
            .code = "edge",
            .name = "Edge"
        },
        {
            .id   = DMI_IPMI_INTR_TRIGGER_LEVEL,
            .code = "level",
            .name = "Level"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_ipmi_intr_polarity_names =
{
    .code  = "ipmi-intr-polarity",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_IPMI_INTR_POLARITY_UNSPEC),
        {
            .id   = DMI_IPMI_INTR_POLARITY_LOW,
            .code = "active-low",
            .name = "Active low"
        },
        {
            .id   = DMI_IPMI_INTR_POLARITY_HIGH,
            .code = "active-high",
            .name = "Active high"
        },
        DMI_NAME_NULL
    }
};

//
// Register-related fields are defined only for interfaces in I/O or memory
// space, and are not shown for SSIF interface
//
#define dmi_ipmi_register_variants(__member, ...)                                                    \
    (const dmi_attribute_variant_t[]){                                                              \
        DMI_VARIANT(DMI_IPMI_ADDR_TYPE_IO, dmi_ipmi_device_t, __member, INTEGER, __VA_ARGS__),       \
        DMI_VARIANT(DMI_IPMI_ADDR_TYPE_MEMORY, dmi_ipmi_device_t, __member, INTEGER, __VA_ARGS__),   \
        DMI_VARIANT_NULL                                                                            \
    }

const dmi_entity_spec_t dmi_ipmi_device_spec =
{
    .code            = "ipmi-device",
    .name            = "IPMI device information",
    .description     = (const char *[]){
        "The information in this structure defines the attributes of an "
        "Intelligent Platform Management Interface (IPMI) Baseboard "
        "Management Controller (BMC). See the Intelligent Platform Management "
        "Interface (IPMI) Interface Specification for full documentation of "
        "IPMI and additional information on the use of this structure.",
        //
        nullptr
    },
    .type            = DMI_TYPE(IPMI_DEVICE),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x12,
    .decoded_length  = sizeof(dmi_ipmi_device_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_ipmi_device_t, interface_type, ENUM, {
            .code   = "interface-type",
            .name   = "Interface type",
            .values = &dmi_ipmi_interface_names
        }),
        DMI_ATTRIBUTE(dmi_ipmi_device_t, spec_version, VERSION, {
            .code   = "specification-version",
            .name   = "Specification version",
            .scale  = 2
        }),
        DMI_ATTRIBUTE(dmi_ipmi_device_t, i2c_target_addr, INTEGER, {
            .code   = "i2c-target-address",
            .name   = "I2C target address",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_ipmi_device_t, nv_storage_addr, INTEGER, {
            .code   = "nv-storage-address",
            .name   = "NV storage address",
            .unspec = dmi_value_ptr((uint8_t)UINT8_MAX),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        // SMBus target address of SSIF interface is shown as a short number
        DMI_ATTRIBUTE_VARIANT(dmi_ipmi_device_t, base_addr_type, {
            .code     = "base-address",
            .name     = "Base address",
            .variants = (const dmi_attribute_variant_t[]){
                DMI_VARIANT(DMI_IPMI_ADDR_TYPE_SMBUS, dmi_ipmi_device_t, base_addr, INTEGER, {
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_VARIANT_DEFAULT(dmi_ipmi_device_t, base_addr, ADDRESS, {}),
                DMI_VARIANT_NULL
            }
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_ipmi_device_t, base_addr_type, {
            .code     = "base-address-lsb",
            .name     = "Base address LSB",
            .variants = dmi_ipmi_register_variants(base_addr_lsb, {})
        }),
        DMI_ATTRIBUTE(dmi_ipmi_device_t, base_addr_type, ENUM, {
            .code   = "base-address-type",
            .name   = "Base address type",
            .values = &dmi_ipmi_addr_type_names
        }),
        DMI_ATTRIBUTE(dmi_ipmi_device_t, intr_trigger, ENUM, {
            .code   = "interrupt-trigger-mode",
            .name   = "Interrupt trigger mode",
            .unspec = dmi_value_ptr(DMI_IPMI_INTR_TRIGGER_UNSPEC),
            .values = &dmi_ipmi_intr_trigger_names
        }),
        DMI_ATTRIBUTE(dmi_ipmi_device_t, intr_polarity, ENUM, {
            .code   = "interrupt-polarity",
            .name   = "Interrupt polarity",
            .unspec = dmi_value_ptr(DMI_IPMI_INTR_POLARITY_UNSPEC),
            .values = &dmi_ipmi_intr_polarity_names
        }),
        DMI_ATTRIBUTE(dmi_ipmi_device_t, intr_number, INTEGER, {
            .code   = "interrupt-number",
            .name   = "Interrupt number",
            .unspec = dmi_value_ptr((unsigned short)0)
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_ipmi_device_t, base_addr_type, {
            .code     = "register-spacing",
            .name     = "Register spacing",
            .variants = dmi_ipmi_register_variants(register_spacing, {
                .unit   = DMI_UNIT_BYTE,
                .unspec = dmi_value_ptr((unsigned short)0)
            })
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_ipmi_device_decode,
    }
};

const char *dmi_ipmi_interface_name(dmi_ipmi_interface_t value)
{
    return dmi_name_lookup(&dmi_ipmi_interface_names, (int)value);
}

const char *dmi_ipmi_addr_type_name(dmi_ipmi_addr_type_t value)
{
    return dmi_name_lookup(&dmi_ipmi_addr_type_names, (int)value);
}

const char *dmi_ipmi_intr_trigger_name(dmi_ipmi_intr_trigger_t value)
{
    return dmi_name_lookup(&dmi_ipmi_intr_trigger_names, (int)value);
}

const char *dmi_ipmi_intr_polarity_name(dmi_ipmi_intr_polarity_t value)
{
    return dmi_name_lookup(&dmi_ipmi_intr_polarity_names, (int)value);
}

static bool dmi_ipmi_device_decode(dmi_entity_t *entity)
{
    dmi_ipmi_device_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(IPMI_DEVICE));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    if (not dmi_stream_decode(stream, dmi_byte_t, &info->interface_type))
        return false;

    dmi_byte_t spec_version = 0;
    if (not dmi_stream_decode(stream, dmi_byte_t, &spec_version))
        return false;

    info->spec_version = dmi_version((spec_version & 0xF0) >> 4, spec_version & 0x0F, 0);

    bool status =
        dmi_stream_decode(stream, dmi_byte_t, &info->i2c_target_addr) and
        dmi_stream_decode(stream, dmi_byte_t, &info->nv_storage_addr);
    if (not status)
        return false;

    dmi_qword_t base_addr = 0;
    if (not dmi_stream_decode(stream, dmi_qword_t, &base_addr))
        return false;

    dmi_ipmi_device_details_t details;
    if (not dmi_stream_decode(stream, dmi_byte_t, &details.__value))
        return false;

    if (details.is_intr_info_specified) {
        info->intr_trigger  = details.is_intr_level_triggered
                            ? DMI_IPMI_INTR_TRIGGER_LEVEL
                            : DMI_IPMI_INTR_TRIGGER_EDGE;
        info->intr_polarity = details.is_intr_active_high
                            ? DMI_IPMI_INTR_POLARITY_HIGH
                            : DMI_IPMI_INTR_POLARITY_LOW;
    } else {
        info->intr_trigger  = DMI_IPMI_INTR_TRIGGER_UNSPEC;
        info->intr_polarity = DMI_IPMI_INTR_POLARITY_UNSPEC;
    }

    switch (details.register_spacing) {
    case DMI_IPMI_REGISTER_SPACING_1:  info->register_spacing = 1;  break;
    case DMI_IPMI_REGISTER_SPACING_4:  info->register_spacing = 4;  break;
    case DMI_IPMI_REGISTER_SPACING_16: info->register_spacing = 16; break;
    default: // fallthrough
    }

    info->base_addr_lsb = details.base_addr_lsb;

    if (info->interface_type == DMI_IPMI_INTERFACE_SSIF) {
        // SSIF interface uses SMBus target address, shifted left by one bit
        info->base_addr      = (base_addr & 0xFFu) >> 1;
        info->base_addr_type = DMI_IPMI_ADDR_TYPE_SMBUS;
    } else {
        // Least-significant bit indicates I/O space, and the actual
        // least-significant bit of the address is stored in the modifier
        info->base_addr      = (base_addr & ~(dmi_qword_t)1u) | info->base_addr_lsb;
        info->base_addr_type = (base_addr & 1u)
                             ? DMI_IPMI_ADDR_TYPE_IO
                             : DMI_IPMI_ADDR_TYPE_MEMORY;
    }

    return dmi_stream_decode(stream, dmi_byte_t, &info->intr_number);
}
