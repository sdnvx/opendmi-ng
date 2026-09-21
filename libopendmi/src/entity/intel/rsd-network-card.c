//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-network-card-internal.h>

const dmi_entity_spec_t dmi_intel_rsd_network_card_spec =
{
    .code = "intel-rsd-network-card",
    .name = "Intel RSD Network card information",
    .type = DMI_TYPE(INTEL_RSD_NETWORK_CARD),

    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x3A,
        .decoded_length  = sizeof(dmi_intel_rsd_network_card_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_intel_rsd_network_card_t, pci_class,        BYTE),
        DMI_FIELD(dmi_intel_rsd_network_card_t, pci_slot_id,      WORD),
        DMI_FIELD(dmi_intel_rsd_network_card_t, vendor_id,        WORD),
        DMI_FIELD(dmi_intel_rsd_network_card_t, device_id,        WORD),
        DMI_FIELD(dmi_intel_rsd_network_card_t, sub_vendor_id,    WORD),
        DMI_FIELD(dmi_intel_rsd_network_card_t, sub_device_id,    WORD),
        DMI_FIELD(dmi_intel_rsd_network_card_t, maximum_speed,    DWORD),
        DMI_FIELD(dmi_intel_rsd_network_card_t, current_speed,    DWORD),
        DMI_FIELD(dmi_intel_rsd_network_card_t, port_index,       WORD),
        DMI_FIELD_BINARY(dmi_intel_rsd_network_card_t, mac_address,
                         DMI_INTEL_RSD_MAC_ADDRESS_LENGTH),
        DMI_FIELD(dmi_intel_rsd_network_card_t, firmware_version, STRING),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_intel_rsd_network_card_t, pci_class, INTEGER, {
            .code = "pci-class",
            .name = "PCI class"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_network_card_t, pci_slot_id, INTEGER, {
            .code = "pci-slot-id",
            .name = "PCI slot ID"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_network_card_t, vendor_id, INTEGER, {
            .code  = "vendor-id",
            .name  = "Vendor ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_network_card_t, device_id, INTEGER, {
            .code  = "device-id",
            .name  = "Device ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_network_card_t, sub_vendor_id, INTEGER, {
            .code  = "sub-vendor-id",
            .name  = "Sub-vendor ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_network_card_t, sub_device_id, INTEGER, {
            .code  = "sub-device-id",
            .name  = "Sub-device ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_network_card_t, maximum_speed, INTEGER, {
            .code  = "maximum-speed",
            .name  = "Maximum speed",
            .unit  = DMI_UNIT_MEGABIT_SECOND
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_network_card_t, current_speed, INTEGER, {
            .code  = "current-speed",
            .name  = "Current speed",
            .unit  = DMI_UNIT_MEGABIT_SECOND
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_network_card_t, port_index, INTEGER, {
            .code  = "port-index",
            .name  = "Port index"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_network_card_t, mac_address, BINARY, {
            .code  = "mac-address",
            .name  = "MAC address",
            .flags = DMI_ATTRIBUTE_FLAG_MAC
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_network_card_t, firmware_version, STRING, {
            .code = "firmware-version",
            .name = "Firmware version"
        }),
        {}
    })
};
