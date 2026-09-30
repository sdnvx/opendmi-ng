//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <ctype.h>
#include <stdio.h>

#include <opendmi/context.h>
#include <opendmi/field.h>
#include <opendmi/platform.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include <opendmi/entity/hpe/version-internal.h>

/**
 * @internal
 * @brief Format the version data of a version indicator.
 *
 * @return Number of the characters written, or a negative value if the
 *         format is not known.
 */
static int dmi_hpe_version_format(dmi_hpe_version_t *info, unsigned generation);

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

bool dmi_hpe_version_derive(dmi_entity_t *entity)
{
    dmi_hpe_version_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_version));
    if (info == nullptr)
        return false;

    const dmi_platform_t *platform = dmi_get_platform(dmi_entity_context(entity));
    unsigned generation = (platform != nullptr) ? platform->generation : 0;

    int length = dmi_hpe_version_format(info, generation);
    if ((length > 0) and ((size_t)length < sizeof(info->version_buffer)))
        info->version = info->version_buffer;

    return true;
}

static int dmi_hpe_version_format(dmi_hpe_version_t *info, unsigned generation)
{
    unsigned d[countof(info->version_data)];
    for (size_t i = 0; i < countof(d); i++)
        d[i] = info->version_data[i];

    char *buf = info->version_buffer;
    size_t size = sizeof(info->version_buffer);

    // Words and double words are held low byte first
    #define W(i)  (d[i] | (d[(i) + 1] << 8))
    #define DW(i) ((unsigned long)W(i) | ((unsigned long)W((i) + 2) << 16))

    int length = -1;

    switch (info->data_format) {
    case 1:
        if (d[0] & 0x80)
            length = snprintf(buf, size, "0x%02X B.0x%02X", d[1] & 0x7F, d[0] & 0x7F);
        else
            length = snprintf(buf, size, "0x%02X", d[1] & 0x7F);
        break;
    case 2:
        length = snprintf(buf, size, "%u.%u", d[0] >> 4, d[0] & 0x0F);
        break;
    case 4:
        length = snprintf(buf, size, "%u.%u.%u", d[0] >> 4, d[0] & 0x0F, d[1] & 0x7F);
        break;
    case 5:
        // The layout of the format has changed with the generations
        if (generation == DMI_HPE_GEN9)
            length = snprintf(buf, size, "%u.%u.%u", d[0] >> 4, d[0] & 0x0F, d[1] & 0x7F);
        else if ((generation == DMI_HPE_GEN10) or (generation == DMI_HPE_GEN10_PLUS))
            length = snprintf(buf, size, "%u.%u.%u.%u", d[1] & 0x0F, d[3] & 0x0F,
                              d[5] & 0x0F, d[6] & 0x0F);
        break;
    case 6:
        length = snprintf(buf, size, "%u.%u", d[1], d[0]);
        break;
    case 7:
        length = snprintf(buf, size, "v%u.%02u (%02u/%02u/%u)", d[0], d[1], d[2], d[3], W(4));
        break;
    case 8:
        length = snprintf(buf, size, "%u.%u", W(4), W(0));
        break;
    case 9:
        length = snprintf(buf, size, "%u.%u.%u", d[0], d[1], W(2));
        break;
    case 10:
        length = snprintf(buf, size, "%u.%u.%u Build %u", d[0], d[1], d[2], d[3]);
        break;
    case 11:
        length = snprintf(buf, size, "%u.%u %lu", W(2), W(0), DW(4));
        break;
    case 12:
        length = snprintf(buf, size, "%u.%u.%u.%u", W(0), W(2), W(4), W(6));
        break;
    case 13:
        length = snprintf(buf, size, "%u", d[0]);
        break;
    case 14:
        length = snprintf(buf, size, "%u.%u.%u.%u", d[0], d[1], d[2], W(3));
        break;
    case 15:
        length = snprintf(buf, size, "%u.%u.%u.%u (%02u/%02u/%u)",
                          W(0), W(2), W(4), W(6), d[8], d[9], W(10));
        break;
    case 16:
        for (size_t i = 0; i < 4; i++) {
            if (not isprint((int)d[i]))
                return -1;
        }
        length = snprintf(buf, size, "%c%c%c%c.%u%u", (int)d[0], (int)d[1], (int)d[2], (int)d[3], d[4], d[5]);
        break;
    case 17:
        length = snprintf(buf, size, "%08lX", DW(0));
        break;
    case 18:
        length = snprintf(buf, size, "%u.%02u", d[0], d[1]);
        break;
    case 19:
        length = snprintf(buf, size, "0x%02x.0x%02x.0x%02x", d[0], d[1], d[2]);
        break;
    case 20:
        length = snprintf(buf, size, "%u.%u.%u.%u", d[0], d[1], d[2], d[3]);
        break;
    default:
        // Format 0 carries no version data, and format 3 is reserved
        break;
    }

    #undef W
    #undef DW

    return length;
}
