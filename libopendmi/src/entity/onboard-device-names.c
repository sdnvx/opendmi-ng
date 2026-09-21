//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/onboard-device-internal.h>

const dmi_name_set_t dmi_onboard_device_type_names =
{
    .code  = "onboard-device-type",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_ONBOARD_DEVICE_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_ONBOARD_DEVICE_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_ONBOARD_DEVICE_TYPE_UNKNOWN),
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_VIDEO,
            .code = "video",
            .name = "Video"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_SCSI_CONTROLLER,
            .code = "scsi-controller",
            .name = "SCSI controller"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_ETHERNET,
            .code = "ethernet",
            .name = "Ethernet"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_TOKEN_RING,
            .code = "token-ring",
            .name = "Token ring"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_SOUND,
            .code = "sound",
            .name = "Sound"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_PATA_CONTROLLER,
            .code = "pata-controller",
            .name = "PATA controller"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_SATA_CONTROLLER,
            .code = "sata-controller",
            .name = "SATA controller"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_SAS_CONTROLLER,
            .code = "sas-controller",
            .name = "SAS controller"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_WIRELESS_LAN,
            .code = "wireless lan",
            .name = "Wireless LAN"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_BLUETOOTH,
            .code = "bluetooth",
            .name = "Bluetooth"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_WIRELESS_WAN,
            .code = "wireless-wan",
            .name = "Wireless WAN"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_EMM_CONTROLLER,
            .code = "emm-controller",
            .name = "eMM controller"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_NVME_CONTROLLER,
            .code = "nvme-controller",
            .name = "NVMe controller"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_UFS_CONTROLLER,
            .code = "ufs-controller",
            .name = "UFS controller"
        },
        {}
    })
};

const char *dmi_onboard_device_type_name(dmi_onboard_device_type_t value)
{
    return dmi_name_lookup(&dmi_onboard_device_type_names, (int)value);
}
