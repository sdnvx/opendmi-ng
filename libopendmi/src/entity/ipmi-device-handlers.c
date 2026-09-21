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
uintmax_t dmi_ipmi_device_convert_version(uintmax_t raw)
{
    return dmi_version((unsigned int)((raw & 0xF0u) >> 4), (unsigned int)(raw & 0x0Fu), 0);
}

//
// One byte holds the interrupt information and the least significant bit of
// the base address, whose parts mean nothing on their own.
//
bool dmi_ipmi_device_decode_details(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value)
{
    dmi_unused(field);

    dmi_ipmi_device_details_t details = {};

    if (not dmi_stream_decode(dmi_entity_stream(entity), dmi_byte_t, &details.__value))
        return false;

    dmi_ipmi_device_t *info = value;

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

    return true;
}

//
// Base address means what the interface type says it does, and its least
// significant bit is carried by the modifier rather than by the address.
//
bool dmi_ipmi_device_derive(dmi_entity_t *entity)
{
    dmi_ipmi_device_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(IPMI_DEVICE));
    if (info == nullptr)
        return false;

    dmi_qword_t base_addr = (dmi_qword_t)info->base_addr;

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

    return true;
}
