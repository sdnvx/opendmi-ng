//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>
#include <opendmi/entity/intel/rsd-storage-device.h>

static bool dmi_intel_rsd_storage_device_decode(dmi_entity_t *entity);

const dmi_name_set_t dmi_intel_rsd_storage_connector_names =
{
    .code  = "intel-rsd-storage-connectors",
    .names = (const dmi_name_t[]){
        DMI_NAME_UNKNOWN(DMI_INTEL_RSD_STORAGE_CONNECTOR_UNKNOWN),
        {
            .id   = DMI_INTEL_RSD_STORAGE_CONNECTOR_SATA,
            .code = "sata",
            .name = "SATA"
        },
        {
            .id   = DMI_INTEL_RSD_STORAGE_CONNECTOR_SAS,
            .code = "sas",
            .name = "SAS"
        },
        {
            .id   = DMI_INTEL_RSD_STORAGE_CONNECTOR_PCIE,
            .code = "pcie",
            .name = "PCIe"
        },
        {
            .id   = DMI_INTEL_RSD_STORAGE_CONNECTOR_M2,
            .code = "m2",
            .name = "M.2"
        },
        {
            .id   = DMI_INTEL_RSD_STORAGE_CONNECTOR_USB,
            .code = "usb",
            .name = "USB"
        },
        {
            .id   = DMI_INTEL_RSD_STORAGE_CONNECTOR_U2,
            .code = "u2",
            .name = "U.2"
        },
        {}
    }
};

const dmi_name_set_t dmi_intel_rsd_storage_proto_names =
{
    .code  = "intel-rsd-storage-protocols",
    .names = (const dmi_name_t[]){
        DMI_NAME_UNKNOWN(DMI_INTEL_RSD_STORAGE_PROTO_UNKNOWN),
        {
            .id   = DMI_INTEL_RSD_STORAGE_PROTO_IDE,
            .code = "ide",
            .name = "IDE"
        },
        {
            .id   = DMI_INTEL_RSD_STORAGE_PROTO_AHCI,
            .code = "ahci",
            .name = "AHCI"
        },
        {
            .id   = DMI_INTEL_RSD_STORAGE_PROTO_NVME,
            .code = "nvme",
            .name = "NVMe"
        },
        {
            .id   = DMI_INTEL_RSD_STORAGE_PROTO_USB,
            .code = "usb",
            .name = "USB"
        },
        {}
    }
};

const dmi_name_set_t dmi_intel_rsd_storage_device_type_names =
{
    .code  = "intel-rsd-storage-device-types",
    .names = (const dmi_name_t[]){
        DMI_NAME_UNKNOWN(DMI_INTEL_RSD_STORAGE_DEVICE_TYPE_UNKNOWN),
        {
            .id = DMI_INTEL_RSD_STORAGE_DEVICE_TYPE_HDD,
            .code = "hdd",
            .name = "HDD"
        },
        {
            .id = DMI_INTEL_RSD_STORAGE_DEVICE_TYPE_SSD,
            .code = "ssd",
            .name = "SSD"
        },
        {
            .id = DMI_INTEL_RSD_STORAGE_DEVICE_TYPE_DVD,
            .code = "dvd",
            .name = "Optical - DVD"
        },
        {
            .id = DMI_INTEL_RSD_STORAGE_DEVICE_TYPE_BLURAY,
            .code = "bluray",
            .name = "Optical - Blu-ray"
        },
        {
            .id = DMI_INTEL_RSD_STORAGE_DEVICE_TYPE_USB,
            .code = "usb",
            .name = "USB"
        },
        {}
    }
};

const dmi_entity_spec_t dmi_intel_rsd_storage_device_spec =
{
    .code            = "intel-rsd-storage-device",
    .name            = "Intel RSD storage device information",
    .type            = DMI_TYPE(INTEL_RSD_STORAGE_DEVICE),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x1B,
    .decoded_length  = sizeof(dmi_intel_rsd_storage_device_t),
    .attributes      = (const dmi_attribute_t[]){
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
    },
    .handlers = {
        .decode = dmi_intel_rsd_storage_device_decode
    }
};

const char *dmi_intel_rsd_storage_connector_name(dmi_intel_rsd_storage_connector_t value)
{
    return dmi_name_lookup(&dmi_intel_rsd_storage_connector_names, (int)value);
}

const char *dmi_intel_rsd_storage_proto_name(dmi_intel_rsd_storage_proto_t value)
{
    return dmi_name_lookup(&dmi_intel_rsd_storage_proto_names, (int)value);
}

const char *dmi_intel_rsd_storage_device_type_name(dmi_intel_rsd_storage_device_type_t value)
{
    return dmi_name_lookup(&dmi_intel_rsd_storage_device_type_names, (int)value);
}

static bool dmi_intel_rsd_storage_device_decode(dmi_entity_t *entity)
{
    dmi_intel_rsd_storage_device_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_STORAGE_DEVICE));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    return
        dmi_stream_decode_str(stream, &info->port) and
        dmi_stream_decode(stream, dmi_byte_t, &info->index) and
        dmi_stream_decode(stream, dmi_byte_t, &info->connector) and
        dmi_stream_decode(stream, dmi_byte_t, &info->protocol) and
        dmi_stream_decode(stream, dmi_byte_t, &info->type) and
        dmi_stream_decode(stream, dmi_dword_t, &info->capacity) and
        dmi_stream_decode(stream, dmi_word_t, &info->rpm) and
        dmi_stream_decode_str(stream, &info->model) and
        dmi_stream_decode_str(stream, &info->serial_number) and
        dmi_stream_decode(stream, dmi_byte_t, &info->pci_class) and
        dmi_stream_decode(stream, dmi_word_t, &info->vendor_id) and
        dmi_stream_decode(stream, dmi_word_t, &info->device_id) and
        dmi_stream_decode(stream, dmi_word_t, &info->sub_vendor_id) and
        dmi_stream_decode(stream, dmi_word_t, &info->sub_device_id) and
        dmi_stream_decode_str(stream, &info->firmware_version);
}
