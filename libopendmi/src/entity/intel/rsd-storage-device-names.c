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

const dmi_name_set_t dmi_intel_rsd_storage_connector_names =
{
    .code  = "intel-rsd-storage-connector",
    .names = DMI_NAMES({
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
    })
};

const dmi_name_set_t dmi_intel_rsd_storage_proto_names =
{
    .code  = "intel-rsd-storage-protocol",
    .names = DMI_NAMES({
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
    })
};

const dmi_name_set_t dmi_intel_rsd_storage_device_type_names =
{
    .code  = "intel-rsd-storage-device-type",
    .names = DMI_NAMES({
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
    })
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
