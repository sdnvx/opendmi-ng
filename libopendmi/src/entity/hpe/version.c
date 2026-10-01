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

#include "version-internal.h"

const dmi_entity_spec_t dmi_hpe_version_spec =
{
    .type        = DMI_TYPE(hpe_version),
    .code        = "hpe-version",
    .name        = "HP/HPE version indicator record",
    .description = (const char *[]){
        "Tells the version of a firmware component of the server, e.g. of a "
        "CPLD or of a power management controller.",
        //
        nullptr
    },
    .params = {
        .generations    = { .minimum = DMI_HPE_GEN8 },
        .minimum_length = 0x17,
        .decoded_length = sizeof(dmi_hpe_version_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_version_t, firmware_type, dmi_word_t),
        DMI_FIELD_STRING(dmi_hpe_version_t, firmware_name),
        DMI_FIELD_STRING(dmi_hpe_version_t, version_string),
        DMI_FIELD(dmi_hpe_version_t, data_format, dmi_byte_t),
        DMI_FIELD_VECTOR(dmi_hpe_version_t, version_data,
            .fields = DMI_FIELDS({
                DMI_FIELD_ELEMENT(dmi_hpe_version_t, version_data, dmi_byte_t),
                {}
            })),
        DMI_FIELD(dmi_hpe_version_t, unique_id, dmi_word_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_version_t, firmware_type, ENUM, {
            .code   = "firmware-type",
            .name   = "Firmware type",
            .values = &dmi_hpe_firmware_type_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_version_t, firmware_name, STRING, {
            .code = "firmware-name",
            .name = "Firmware name"
        }),
        DMI_ATTRIBUTE(dmi_hpe_version_t, version_string, STRING, {
            .code = "version-string",
            .name = "Firmware version string"
        }),
        DMI_ATTRIBUTE(dmi_hpe_version_t, data_format, INTEGER, {
            .code = "data-format",
            .name = "Version data format"
        }),
        DMI_ATTRIBUTE_VECTOR(dmi_hpe_version_t, version_data, INTEGER, {
            .code  = "version-data",
            .name  = "Version data",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_version_t, version, STRING, {
            .code = "version",
            .name = "Version"
        }),
        DMI_ATTRIBUTE(dmi_hpe_version_t, unique_id, INTEGER, {
            .code   = "unique-id",
            .name   = "Unique ID",
            .unspec = dmi_value_ptr((uint16_t)0),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_hpe_version_derive
    }
};

const dmi_name_set_t dmi_hpe_firmware_type_names =
{
    .code  = "hpe-firmware-type",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_SYSTEM_ROM,
            .code = "system-rom",
            .name = "System ROM"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_REDUNDANT_ROM,
            .code = "redundant-rom",
            .name = "Redundant system ROM"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_BOOTBLOCK,
            .code = "bootblock",
            .name = "System ROM bootblock"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_PMC,
            .code = "pmc",
            .name = "Power management controller firmware"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_PMC_BOOTLOADER,
            .code = "pmc-bootloader",
            .name = "Power management controller firmware bootloader"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_SL_CHASSIS,
            .code = "sl-chassis",
            .name = "SL chassis firmware"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_SL_CHASSIS_LOADER,
            .code = "sl-chassis-loader",
            .name = "SL chassis firmware bootloader"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_CPLD,
            .code = "cpld",
            .name = "Hardware PAL/CPLD"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_SPS,
            .code = "sps",
            .name = "SPS firmware (ME firmware)"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_SL_CHASSIS_CPLD,
            .code = "sl-chassis-cpld",
            .name = "SL chassis PAL/CPLD"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_CSM,
            .code = "csm",
            .name = "Compatibility support module (CSM)"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_APML,
            .code = "apml",
            .name = "APML"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_BATTERY,
            .code = "battery",
            .name = "Smart storage battery (Megacell) firmware"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_TRUSTED_MODULE,
            .code = "trusted-module",
            .name = "Trusted module (TPM or TCM) firmware"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_NVME_BACKPLANE,
            .code = "nvme-backplane",
            .name = "NVMe backplane firmware"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_PROVISIONING,
            .code = "provisioning",
            .name = "Intelligent Provisioning"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_SPI_DESCRIPTOR,
            .code = "spi-descriptor",
            .name = "SPI descriptor version"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_IE,
            .code = "ie",
            .name = "Innovation Engine firmware (IE firmware)"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_UMB_BACKPLANE,
            .code = "umb-backplane",
            .name = "UMB backplane firmware"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_DIAGNOSTICS,
            .code = "diagnostics",
            .name = "Embedded diagnostics"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_EL_CHASSIS_ABSTRACT,
            .code = "el-chassis-abstract",
            .name = "EL chassis abstraction revision"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_EL_CHASSIS,
            .code = "el-chassis",
            .name = "EL chassis firmware revision"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_EL_CHASSIS_CPLD,
            .code = "el-chassis-cpld",
            .name = "EL chassis PAL/CPLD"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_EL_CARTRIDGE,
            .code = "el-cartridge",
            .name = "EL cartridge abstraction revision"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_VIDEO,
            .code = "video",
            .name = "Embedded video controller"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_RISER_CPLD,
            .code = "riser-cpld",
            .name = "PCIe riser programmable logic device"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_CARD_CPLD,
            .code = "card-cpld",
            .name = "PCIe card CPLD"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_VROC_NVME,
            .code = "vroc-nvme",
            .name = "Intel NVMe VROC"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_VROC_SATA,
            .code = "vroc-sata",
            .name = "Intel SATA VROC"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_INTEL_SPS,
            .code = "intel-sps",
            .name = "Intel SPS firmware"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_SECONDARY_CPLD,
            .code = "secondary-cpld",
            .name = "Secondary system programmable logic device"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_MEZZANINE_CPLD,
            .code = "mezzanine-cpld",
            .name = "CPU mezzanine board CPLD"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_ARCTIC_SOUND,
            .code = "arctic-sound",
            .name = "Intel Arctic Sound-M accelerator firmware"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_AMPERE_SCP,
            .code = "ampere-scp",
            .name = "Ampere system control processor (SCP)"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_INTEL_CFR,
            .code = "intel-cfr",
            .name = "Intel CFR information"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_OCP,
            .code = "ocp",
            .name = "OCP card"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_DC_SCM_CPLD,
            .code = "dc-scm-cpld",
            .name = "DC-SCM CPLD"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_POWER_BOARD_CPLD,
            .code = "power-board-cpld",
            .name = "Power distribution board CPLD"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_SWITCH_BOARD_CPLD,
            .code = "switch-board-cpld",
            .name = "PCIe switch board CPLD"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_SIDEBAND_BOARD_CPLD,
            .code = "sideband-board-cpld",
            .name = "Sideband board CPLD"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_RISER_MCU,
            .code = "riser-mcu",
            .name = "PCIe riser MCU firmware"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_SWITCH_BOARD,
            .code = "switch-board",
            .name = "PCIe switch board firmware"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_POWER_SUPPLY,
            .code = "power-supply",
            .name = "Power supply firmware"
        },
        {
            .id   = DMI_HPE_FIRMWARE_TYPE_BMC,
            .code = "bmc",
            .name = "BMC firmware"
        },
        {}
    })
};

const char *dmi_hpe_firmware_type_name(dmi_hpe_firmware_type_t value)
{
    return dmi_name_lookup(&dmi_hpe_firmware_type_names, (int)value);
}
