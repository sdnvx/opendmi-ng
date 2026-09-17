//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>
#include <opendmi/entity/intel/rsd-network-card.h>

static bool dmi_intel_rsd_network_card_decode(dmi_entity_t *entity);

const dmi_entity_spec_t dmi_intel_rsd_network_card_spec =
{
    .code            = "intel-rsd-network-card",
    .name            = "Intel RSD Network card information",
    .type            = DMI_TYPE(INTEL_RSD_NETWORK_CARD),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x3A,
    .decoded_length  = sizeof(dmi_intel_rsd_network_card_t),
    .attributes      = (const dmi_attribute_t[]){
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
        // TODO: Add MAC address attribute after supporting binary attrs
        DMI_ATTRIBUTE(dmi_intel_rsd_network_card_t, firmware_version, STRING, {
            .code = "firmware-version",
            .name = "Firmware version"
        }),
        {}
    },
    .handlers = {
        .decode = dmi_intel_rsd_network_card_decode
    }
};

static bool dmi_intel_rsd_network_card_decode(dmi_entity_t *entity)
{
    dmi_intel_rsd_network_card_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_NETWORK_CARD));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    return
        dmi_stream_decode(stream, dmi_byte_t, &info->pci_class) and
        dmi_stream_decode(stream, dmi_word_t, &info->pci_slot_id) and
        dmi_stream_decode(stream, dmi_word_t, &info->vendor_id) and
        dmi_stream_decode(stream, dmi_word_t, &info->device_id) and
        dmi_stream_decode(stream, dmi_word_t, &info->sub_vendor_id) and
        dmi_stream_decode(stream, dmi_word_t, &info->sub_device_id) and
        dmi_stream_decode(stream, dmi_dword_t, &info->maximum_speed) and
        dmi_stream_decode(stream, dmi_dword_t, &info->current_speed) and
        dmi_stream_decode(stream, dmi_word_t, &info->port_index) and
        dmi_stream_read_data(stream, info->mac_address, sizeof(info->mac_address)) and
        dmi_stream_decode_str(stream, &info->firmware_version);
}
