//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include <opendmi/entity/hpe/nic-internal.h>
#include <opendmi/entity/hpe/nic-mac-internal.h>

const dmi_entity_spec_t dmi_hpe_nic_mac_spec =
{
    .type        = DMI_TYPE(HPE_NIC),
    .code        = "hpe-nic-mac",
    .name        = "HP/HPE NIC PCI and MAC information",
    .description = (const char *[]){
        "Describes a network port the firmware boots from over PXE, one per "
        "structure.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x0E,
        .decoded_length = sizeof(dmi_hpe_nic_mac_t)
    },

    // The MAC address is padded with zeroes to 32 bytes
    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_nic_mac_t, segment, dmi_word_t),
        DMI_FIELD(dmi_hpe_nic_mac_t, bus,     dmi_byte_t),
        DMI_FIELD(dmi_hpe_nic_mac_t, devfn,   dmi_byte_t),
        DMI_FIELD_BINARY(dmi_hpe_nic_mac_t, mac_address, 6),

        DMI_FIELD_GROUP(),
        DMI_FIELD_SKIP(26),
        DMI_FIELD(dmi_hpe_nic_mac_t, port, dmi_byte_t,
                  .absent = dmi_value_ptr((uint8_t)UINT8_MAX)),

        DMI_FIELD_GROUP(),
        DMI_FIELD_STRING(dmi_hpe_nic_mac_t, uefi_device_path),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_nic_mac_t, port, INTEGER, {
            .code   = "port",
            .name   = "Port",
            .unspec = dmi_value_ptr((uint8_t)UINT8_MAX)
        }),
        DMI_ATTRIBUTE(dmi_hpe_nic_mac_t, state, ENUM, {
            .code   = "state",
            .name   = "State",
            .values = &dmi_hpe_nic_state_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_nic_mac_t, segment, INTEGER, {
            .code  = "segment",
            .name  = "PCI segment group",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_nic_mac_t, bus, INTEGER, {
            .code  = "bus",
            .name  = "PCI bus",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_nic_mac_t, devfn, INTEGER, {
            .code  = "devfn",
            .name  = "PCI device and function",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_nic_mac_t, mac_address, BINARY, {
            .code  = "mac-address",
            .name  = "MAC address",
            .flags = DMI_ATTRIBUTE_FLAG_MAC | DMI_ATTRIBUTE_FLAG_PRIVATE
        }),
        DMI_ATTRIBUTE(dmi_hpe_nic_mac_t, uefi_device_path, STRING, {
            .code = "uefi-device-path",
            .name = "UEFI device path"
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_hpe_nic_mac_derive
    }
};

bool dmi_hpe_nic_mac_derive(dmi_entity_t *entity)
{
    dmi_hpe_nic_mac_t *info = dmi_entity_info(entity, DMI_TYPE(HPE_NIC));
    if (info == nullptr)
        return false;

    if ((info->bus == 0x00) and (info->devfn == 0x00))
        info->state = DMI_HPE_NIC_STATE_DISABLED;
    else if ((info->bus == 0xFF) and (info->devfn == 0xFF))
        info->state = DMI_HPE_NIC_STATE_NOT_INSTALLED;
    else
        info->state = DMI_HPE_NIC_STATE_INSTALLED;

    return true;
}
