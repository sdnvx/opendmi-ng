//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-pcie-internal.h>

const dmi_entity_spec_t dmi_intel_rsd_pcie_spec =
{
    .code            = "intel-rsd-pcie",
    .name            = "Intel RSD PCIe information",
    .type            = DMI_TYPE(INTEL_RSD_PCIE),
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x17,
        .decoded_length  = sizeof(dmi_intel_rsd_pcie_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_intel_rsd_pcie_t, pci_class,     dmi_byte_t),
        DMI_FIELD(dmi_intel_rsd_pcie_t, pci_slot_id,   dmi_word_t),
        DMI_FIELD(dmi_intel_rsd_pcie_t, vendor_id,     dmi_word_t),
        DMI_FIELD(dmi_intel_rsd_pcie_t, device_id,     dmi_word_t),
        DMI_FIELD(dmi_intel_rsd_pcie_t, sub_vendor_id, dmi_word_t),
        DMI_FIELD(dmi_intel_rsd_pcie_t, sub_device_id, dmi_word_t),
        DMI_FIELD(dmi_intel_rsd_pcie_t, link_speed,    dmi_dword_t),
        DMI_FIELD(dmi_intel_rsd_pcie_t, link_width,    dmi_dword_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_intel_rsd_pcie_t, pci_class, INTEGER, {
            .code = "pci-class",
            .name = "PCI class"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_pcie_t, pci_slot_id, INTEGER, {
            .code = "pci-slot-id",
            .name = "PCI slot ID"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_pcie_t, vendor_id, INTEGER, {
            .code  = "vendor-id",
            .name  = "Vendor ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_pcie_t, device_id, INTEGER, {
            .code  = "device-id",
            .name  = "Device ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_pcie_t, sub_vendor_id, INTEGER, {
            .code  = "sub-vendor-id",
            .name  = "Sub-vendor ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_pcie_t, sub_device_id, INTEGER, {
            .code  = "sub-device-id",
            .name  = "Sub-device ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_pcie_t, link_speed, INTEGER, {
            .code  = "link-speed",
            .name  = "Link speed"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_pcie_t, link_width, INTEGER, {
            .code  = "link-width",
            .name  = "Link width"
        }),
        {}
    }),
};
