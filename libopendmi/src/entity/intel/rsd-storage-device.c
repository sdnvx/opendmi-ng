//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-storage-device-internal.h>

const dmi_entity_spec_t dmi_intel_rsd_storage_device_spec =
{
    .code            = "intel-rsd-storage-device",
    .name            = "Intel RSD storage device information",
    .type            = DMI_TYPE(INTEL_RSD_STORAGE_DEVICE),
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x1B,
        .decoded_length  = sizeof(dmi_intel_rsd_storage_device_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_intel_rsd_storage_device_t, port,             STRING),
        DMI_FIELD(dmi_intel_rsd_storage_device_t, index,            BYTE),
        DMI_FIELD(dmi_intel_rsd_storage_device_t, connector,        BYTE),
        DMI_FIELD(dmi_intel_rsd_storage_device_t, protocol,         BYTE),
        DMI_FIELD(dmi_intel_rsd_storage_device_t, type,             BYTE),
        DMI_FIELD(dmi_intel_rsd_storage_device_t, capacity,         DWORD),
        DMI_FIELD(dmi_intel_rsd_storage_device_t, rpm,              WORD),
        DMI_FIELD(dmi_intel_rsd_storage_device_t, model,            STRING),
        DMI_FIELD(dmi_intel_rsd_storage_device_t, serial_number,    STRING),
        DMI_FIELD(dmi_intel_rsd_storage_device_t, pci_class,        BYTE),
        DMI_FIELD(dmi_intel_rsd_storage_device_t, vendor_id,        WORD),
        DMI_FIELD(dmi_intel_rsd_storage_device_t, device_id,        WORD),
        DMI_FIELD(dmi_intel_rsd_storage_device_t, sub_vendor_id,    WORD),
        DMI_FIELD(dmi_intel_rsd_storage_device_t, sub_device_id,    WORD),
        DMI_FIELD(dmi_intel_rsd_storage_device_t, firmware_version, STRING),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_intel_rsd_storage_device_t, port, STRING, {
            .code    = "port",
            .name    = "Port designator"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_storage_device_t, index, INTEGER, {
            .code    = "index",
            .name    = "Device index"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_storage_device_t, connector, ENUM, {
            .code    = "connector",
            .name    = "Connector",
            .values  = &dmi_intel_rsd_storage_connector_names,
            .unknown = dmi_value_ptr(DMI_INTEL_RSD_STORAGE_CONNECTOR_UNKNOWN)
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_storage_device_t, protocol, ENUM, {
            .code    = "protocol",
            .name    = "Protocol",
            .values  = &dmi_intel_rsd_storage_proto_names,
            .unknown = dmi_value_ptr(DMI_INTEL_RSD_STORAGE_PROTO_UNKNOWN)
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_storage_device_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .values  = &dmi_intel_rsd_storage_device_type_names,
            .unknown = dmi_value_ptr(DMI_INTEL_RSD_STORAGE_DEVICE_TYPE_UNKNOWN)
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_storage_device_t, capacity, INTEGER, {
            .code    = "capacity",
            .name    = "Capacity",
            .unit    = DMI_UNIT_GIGABYTE
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_storage_device_t, rpm, INTEGER, {
            .code    = "rpm",
            .name    = "RPM",
            .unit    = DMI_UNIT_REVOLUTION,
            .unspec  = dmi_value_ptr((uint16_t)0)
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_storage_device_t, model, STRING, {
            .code    = "model",
            .name    = "Model"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_storage_device_t, serial_number, STRING, {
            .code    = "serial-number",
            .name    = "Serial number"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_storage_device_t, pci_class, INTEGER, {
            .code    = "pci-class",
            .name    = "PCI class"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_storage_device_t, vendor_id, INTEGER, {
            .code    = "vendor-id",
            .name    = "Vendor ID",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_storage_device_t, device_id, INTEGER, {
            .code    = "device-id",
            .name    = "Device ID",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_storage_device_t, sub_vendor_id, INTEGER, {
            .code    = "sub-vendor-id",
            .name    = "Sub-vendor ID",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_storage_device_t, sub_device_id, INTEGER, {
            .code    = "sub-device-id",
            .name    = "Sub-device ID",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_storage_device_t, firmware_version, STRING, {
            .code    = "firmware-version",
            .name    = "Firmware version"
        }),
        {}
    })
};
