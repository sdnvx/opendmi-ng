//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/utils/endian.h>
#include <opendmi/internal.h>
#include <opendmi/module/cisco.h>

#include "pci-adapter-internal.h"

const dmi_entity_spec_t dmi_cisco_pci_adapter_spec =
{
    .type        = DMI_TYPE(cisco_pci_adapter),
    .code        = "cisco-pci-adapter",
    .name        = "Cisco PCI adapter information",
    .description = (const char *[]){
        "Describes an adapter: its PCI identifiers and class, its model, the "
        "slot it is installed in, and the version of its firmware.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x14,
        .decoded_length = sizeof(dmi_cisco_pci_adapter_t)
    },

    // Identifiers are held in the big-endian byte order, and are decoded as
    // they are, to be swapped by the handler
    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_cisco_pci_adapter_t, version,       dmi_byte_t),
        DMI_FIELD(dmi_cisco_pci_adapter_t, raw_vendor_id, dmi_word_t),
        DMI_FIELD(dmi_cisco_pci_adapter_t, raw_device_id, dmi_word_t),

        // Class code is laid out as the configuration space of the function
        // holds it, from the programming interface up
        DMI_FIELD(dmi_cisco_pci_adapter_t, prog_if,   dmi_byte_t),
        DMI_FIELD(dmi_cisco_pci_adapter_t, subclass,  dmi_byte_t),
        DMI_FIELD(dmi_cisco_pci_adapter_t, pci_class, dmi_byte_t),
        DMI_FIELD(dmi_cisco_pci_adapter_t, revision,  dmi_byte_t),

        DMI_FIELD_STRING(dmi_cisco_pci_adapter_t, model),
        DMI_FIELD_STRING(dmi_cisco_pci_adapter_t, slot),
        DMI_FIELD(dmi_cisco_pci_adapter_t, raw_subsystem_vendor_id, dmi_word_t),
        DMI_FIELD(dmi_cisco_pci_adapter_t, raw_subsystem_id,        dmi_word_t),
        DMI_FIELD_STRING(dmi_cisco_pci_adapter_t, firmware_version),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_cisco_pci_adapter_t, version, INTEGER, {
            .code = "version",
            .name = "Version"
        }),
        DMI_ATTRIBUTE(dmi_cisco_pci_adapter_t, model, STRING, {
            .code = "model",
            .name = "Model"
        }),
        DMI_ATTRIBUTE(dmi_cisco_pci_adapter_t, slot, STRING, {
            .code = "slot",
            .name = "Slot"
        }),
        DMI_ATTRIBUTE(dmi_cisco_pci_adapter_t, vendor_id, INTEGER, {
            .code  = "vendor-id",
            .name  = "Vendor ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_cisco_pci_adapter_t, device_id, INTEGER, {
            .code  = "device-id",
            .name  = "Device ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_cisco_pci_adapter_t, subsystem_vendor_id, INTEGER, {
            .code  = "subsystem-vendor-id",
            .name  = "Subsystem vendor ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_cisco_pci_adapter_t, subsystem_id, INTEGER, {
            .code  = "subsystem-id",
            .name  = "Subsystem ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_cisco_pci_adapter_t, pci_class, INTEGER, {
            .code  = "pci-class",
            .name  = "PCI class",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_cisco_pci_adapter_t, subclass, INTEGER, {
            .code  = "subclass",
            .name  = "PCI subclass",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_cisco_pci_adapter_t, prog_if, INTEGER, {
            .code  = "prog-if",
            .name  = "Programming interface",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_cisco_pci_adapter_t, revision, INTEGER, {
            .code  = "revision",
            .name  = "Revision ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_cisco_pci_adapter_t, firmware_version, STRING, {
            .code = "firmware-version",
            .name = "Firmware version"
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_cisco_pci_adapter_derive
    }
};

bool dmi_cisco_pci_adapter_derive(dmi_entity_t *entity)
{
    dmi_cisco_pci_adapter_t *info = dmi_entity_info(entity, DMI_TYPE(cisco_pci_adapter));
    if (info == nullptr)
        return false;

    // Words are read in the little-endian byte order, while the structure
    // holds them in the big-endian one
    info->vendor_id           = dmi_bswap16(info->raw_vendor_id);
    info->device_id           = dmi_bswap16(info->raw_device_id);
    info->subsystem_vendor_id = dmi_bswap16(info->raw_subsystem_vendor_id);
    info->subsystem_id        = dmi_bswap16(info->raw_subsystem_id);

    return true;
}
