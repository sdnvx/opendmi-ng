//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/ipmi-device-internal.h>

//
// Revision is one nibble of major and one of minor.
//
bool dmi_ipmi_device_decode_version(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    unsigned int major = (unsigned int)((data->number & 0xF0u) >> 4);
    unsigned int minor = (unsigned int)(data->number & 0x0Fu);

    return dmi_field_set(field, value, dmi_version(major, minor, 0));
}

bool dmi_ipmi_device_encode_version(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    dmi_version_t version = (dmi_version_t)dmi_field_get(field, value);

    data->number = ((dmi_version_major(version) & 0x0Fu) << 4) | (dmi_version_minor(version) & 0x0Fu);

    return true;
}

//
// Base address means what the interface type says it does: an SMBus target
// address shifted left by one bit for the SSIF interface, and a memory or an
// I/O address for the rest, whose least significant bit tells the two apart
// and is carried by the modifier instead.
//
bool dmi_ipmi_device_decode_address(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    dmi_unused(field);

    dmi_ipmi_device_t *info  = value;
    const dmi_byte_t  *bytes = data->binary.data;

    dmi_qword_t base_addr = 0;

    for (size_t i = 0; i < sizeof(dmi_qword_t); i++)
        base_addr |= (dmi_qword_t)bytes[i] << (i * CHAR_BIT);

    dmi_ipmi_device_details_t details = {
        .__value = bytes[sizeof(dmi_qword_t)]
    };

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
    default:                           info->register_spacing = 0;  break;
    }

    info->base_addr_lsb = details.base_addr_lsb;

    if (info->interface_type == DMI_IPMI_INTERFACE_SSIF) {
        info->base_addr      = (base_addr & 0xFFu) >> 1;
        info->base_addr_type = DMI_IPMI_ADDR_TYPE_SMBUS;
    } else {
        info->base_addr      = (base_addr & ~(dmi_qword_t)1u) | info->base_addr_lsb;
        info->base_addr_type = (base_addr & 1u)
                             ? DMI_IPMI_ADDR_TYPE_IO
                             : DMI_IPMI_ADDR_TYPE_MEMORY;
    }

    return true;
}

bool dmi_ipmi_device_encode_address(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    dmi_unused(field);

    const dmi_ipmi_device_t *info = value;

    dmi_qword_t base_addr;

    if (info->interface_type == DMI_IPMI_INTERFACE_SSIF) {
        base_addr = ((dmi_qword_t)info->base_addr << 1) & 0xFFu;
    } else {
        base_addr = ((dmi_qword_t)info->base_addr & ~(dmi_qword_t)1u) |
                    ((info->base_addr_type == DMI_IPMI_ADDR_TYPE_IO) ? 1u : 0u);
    }

    dmi_ipmi_device_details_t details = {};

    details.is_intr_info_specified  = (info->intr_trigger != DMI_IPMI_INTR_TRIGGER_UNSPEC);
    details.is_intr_level_triggered = (info->intr_trigger == DMI_IPMI_INTR_TRIGGER_LEVEL);
    details.is_intr_active_high     = (info->intr_polarity == DMI_IPMI_INTR_POLARITY_HIGH);
    details.base_addr_lsb           = info->base_addr_lsb & 1u;

    switch (info->register_spacing) {
    case 1:  details.register_spacing = DMI_IPMI_REGISTER_SPACING_1;        break;
    case 4:  details.register_spacing = DMI_IPMI_REGISTER_SPACING_4;        break;
    case 16: details.register_spacing = DMI_IPMI_REGISTER_SPACING_16;       break;
    default: details.register_spacing = DMI_IPMI_REGISTER_SPACING_RESERVED; break;
    }

    for (size_t i = 0; i < sizeof(dmi_qword_t); i++)
        data->buffer[i] = (dmi_byte_t)((base_addr >> (i * CHAR_BIT)) & 0xFFu);

    data->buffer[sizeof(dmi_qword_t)] = details.__value;

    data->binary = (dmi_binary_t){
        .data   = data->buffer,
        .length = sizeof(dmi_qword_t) + sizeof(dmi_byte_t)
    };

    return true;
}
