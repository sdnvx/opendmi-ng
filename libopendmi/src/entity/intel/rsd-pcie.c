//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>
#include <opendmi/entity/intel/rsd-pcie.h>

static bool dmi_intel_rsd_pcie_decode(dmi_entity_t *entity);

const dmi_entity_spec_t dmi_intel_rsd_pcie_spec =
{
    .code            = "intel-rsd-pcie",
    .name            = "Intel RSD PCIe information",
    .type            = DMI_TYPE(INTEL_RSD_PCIE),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x17,
    .decoded_length  = sizeof(dmi_intel_rsd_pcie_t),
    .attributes      = (const dmi_attribute_t[]){
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
    },
    .handlers = {
        .decode = dmi_intel_rsd_pcie_decode
    }
};

static bool dmi_intel_rsd_pcie_decode(dmi_entity_t *entity)
{
    dmi_intel_rsd_pcie_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_PCIE));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    return
        dmi_stream_decode(stream, dmi_byte_t, &info->pci_class) and
        dmi_stream_decode(stream, dmi_word_t, &info->pci_slot_id) and
        dmi_stream_decode(stream, dmi_word_t, &info->vendor_id) and
        dmi_stream_decode(stream, dmi_word_t, &info->device_id) and
        dmi_stream_decode(stream, dmi_word_t, &info->sub_vendor_id) and
        dmi_stream_decode(stream, dmi_word_t, &info->sub_device_id) and
        dmi_stream_decode(stream, dmi_dword_t, &info->link_speed) and
        dmi_stream_decode(stream, dmi_dword_t, &info->link_width);
}
