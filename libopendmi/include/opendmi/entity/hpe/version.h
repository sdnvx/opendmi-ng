//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_VERSION_H
#define OPENDMI_ENTITY_HPE_VERSION_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_HPE_VERSION_T
#   define DMI_HPE_VERSION_T
    typedef struct dmi_hpe_version dmi_hpe_version_t;
#endif // !DMI_HPE_VERSION_T

/**
 * @brief Firmware component a version indicator describes.
 */
typedef enum dmi_hpe_firmware_type
{
    DMI_HPE_FIRMWARE_TYPE_SYSTEM_ROM           = 0x01, ///< System ROM
    DMI_HPE_FIRMWARE_TYPE_REDUNDANT_ROM        = 0x02, ///< Redundant system ROM
    DMI_HPE_FIRMWARE_TYPE_BOOTBLOCK            = 0x03, ///< System ROM bootblock
    DMI_HPE_FIRMWARE_TYPE_PMC                  = 0x04, ///< Power management controller firmware
    DMI_HPE_FIRMWARE_TYPE_PMC_BOOTLOADER       = 0x05, ///< Power management controller firmware bootloader
    DMI_HPE_FIRMWARE_TYPE_SL_CHASSIS           = 0x06, ///< SL chassis firmware
    DMI_HPE_FIRMWARE_TYPE_SL_CHASSIS_LOADER    = 0x07, ///< SL chassis firmware bootloader
    DMI_HPE_FIRMWARE_TYPE_CPLD                 = 0x08, ///< Hardware PAL/CPLD
    DMI_HPE_FIRMWARE_TYPE_SPS                  = 0x09, ///< SPS firmware (ME firmware)
    DMI_HPE_FIRMWARE_TYPE_SL_CHASSIS_CPLD      = 0x0A, ///< SL chassis PAL/CPLD
    DMI_HPE_FIRMWARE_TYPE_CSM                  = 0x0B, ///< Compatibility support module (CSM)
    DMI_HPE_FIRMWARE_TYPE_APML                 = 0x0C, ///< APML
    DMI_HPE_FIRMWARE_TYPE_BATTERY              = 0x0D, ///< Smart storage battery (Megacell) firmware
    DMI_HPE_FIRMWARE_TYPE_TRUSTED_MODULE       = 0x0E, ///< Trusted module (TPM or TCM) firmware
    DMI_HPE_FIRMWARE_TYPE_NVME_BACKPLANE       = 0x0F, ///< NVMe backplane firmware
    DMI_HPE_FIRMWARE_TYPE_PROVISIONING         = 0x10, ///< Intelligent Provisioning
    DMI_HPE_FIRMWARE_TYPE_SPI_DESCRIPTOR       = 0x11, ///< SPI descriptor version
    DMI_HPE_FIRMWARE_TYPE_IE                   = 0x12, ///< Innovation Engine firmware (IE firmware)
    DMI_HPE_FIRMWARE_TYPE_UMB_BACKPLANE        = 0x13, ///< UMB backplane firmware
    DMI_HPE_FIRMWARE_TYPE_DIAGNOSTICS          = 0x14, ///< Embedded diagnostics
    DMI_HPE_FIRMWARE_TYPE_EL_CHASSIS_ABSTRACT  = 0x20, ///< EL chassis abstraction revision
    DMI_HPE_FIRMWARE_TYPE_EL_CHASSIS           = 0x21, ///< EL chassis firmware revision
    DMI_HPE_FIRMWARE_TYPE_EL_CHASSIS_CPLD      = 0x22, ///< EL chassis PAL/CPLD
    DMI_HPE_FIRMWARE_TYPE_EL_CARTRIDGE         = 0x23, ///< EL cartridge abstraction revision
    DMI_HPE_FIRMWARE_TYPE_VIDEO                = 0x30, ///< Embedded video controller
    DMI_HPE_FIRMWARE_TYPE_RISER_CPLD           = 0x31, ///< PCIe riser programmable logic device
    DMI_HPE_FIRMWARE_TYPE_CARD_CPLD            = 0x32, ///< PCIe card CPLD
    DMI_HPE_FIRMWARE_TYPE_VROC_NVME            = 0x33, ///< Intel NVMe VROC
    DMI_HPE_FIRMWARE_TYPE_VROC_SATA            = 0x34, ///< Intel SATA VROC
    DMI_HPE_FIRMWARE_TYPE_INTEL_SPS            = 0x35, ///< Intel SPS firmware
    DMI_HPE_FIRMWARE_TYPE_SECONDARY_CPLD       = 0x36, ///< Secondary system programmable logic device
    DMI_HPE_FIRMWARE_TYPE_MEZZANINE_CPLD       = 0x37, ///< CPU mezzanine board CPLD
    DMI_HPE_FIRMWARE_TYPE_ARCTIC_SOUND         = 0x38, ///< Intel Arctic Sound-M accelerator firmware
    DMI_HPE_FIRMWARE_TYPE_AMPERE_SCP           = 0x39, ///< Ampere system control processor (SCP)
    DMI_HPE_FIRMWARE_TYPE_INTEL_CFR            = 0x3A, ///< Intel CFR information
    DMI_HPE_FIRMWARE_TYPE_OCP                  = 0x3B, ///< OCP card
    DMI_HPE_FIRMWARE_TYPE_DC_SCM_CPLD          = 0x3C, ///< DC-SCM CPLD
    DMI_HPE_FIRMWARE_TYPE_POWER_BOARD_CPLD     = 0x3D, ///< Power distribution board CPLD
    DMI_HPE_FIRMWARE_TYPE_SWITCH_BOARD_CPLD    = 0x3E, ///< PCIe switch board CPLD
    DMI_HPE_FIRMWARE_TYPE_SIDEBAND_BOARD_CPLD  = 0x3F, ///< Sideband board CPLD
    DMI_HPE_FIRMWARE_TYPE_RISER_MCU            = 0x40, ///< PCIe riser MCU firmware
    DMI_HPE_FIRMWARE_TYPE_SWITCH_BOARD         = 0x41, ///< PCIe switch board firmware
    DMI_HPE_FIRMWARE_TYPE_POWER_SUPPLY         = 0x42, ///< Power supply firmware
    DMI_HPE_FIRMWARE_TYPE_BMC                  = 0x43  ///< BMC firmware
} dmi_hpe_firmware_type_t;

/**
 * @brief HP/HPE version indicator record (type 216), from Gen8 onwards.
 *
 * Tells the version of a firmware component of the server, e.g. of a CPLD
 * or of a power management controller, which type 193 does not scale to.
 * There is a record for each component.
 */
struct dmi_hpe_version
{
    /**
     * @brief Firmware component.
     */
    dmi_hpe_firmware_type_t firmware_type;

    /**
     * @brief Name of the firmware.
     */
    const char *firmware_name;

    /**
     * @brief Version of the firmware, as a string.
     */
    const char *version_string;

    /**
     * @brief Format of `version_data`, zero if there is none.
     */
    uint8_t data_format;

    /**
     * @brief Version of the firmware, in the format `data_format` tells.
     */
    uint8_t version_data[12];

    /**
     * @brief Version of the firmware formatted from `version_data`, or
     * @c nullptr if there is none or its format is not known.
     *
     * The string belongs to the structure, and is freed along with it.
     */
    char *version;

    /**
     * @brief Unique ID of the firmware flash, zero if none.
     */
    uint16_t unique_id;
};

/**
 * @brief HP/HPE version indicator record entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_version_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_hpe_firmware_type_name(dmi_hpe_firmware_type_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_HPE_VERSION_H
