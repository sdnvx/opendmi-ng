//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_DRIVE_H
#define OPENDMI_ENTITY_HPE_DRIVE_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/hpe/common.h>

#ifndef DMI_HPE_DRIVE_T
#   define DMI_HPE_DRIVE_T
    typedef struct dmi_hpe_drive dmi_hpe_drive_t;
#endif // !DMI_HPE_DRIVE_T

/**
 * @brief Type of a drive.
 */
typedef enum dmi_hpe_drive_type
{
    DMI_HPE_DRIVE_TYPE_UNDETERMINED = 0x00, ///< Undetermined
    DMI_HPE_DRIVE_TYPE_NVME_SSD     = 0x01, ///< NVMe SSD
    DMI_HPE_DRIVE_TYPE_SATA         = 0x02, ///< SATA
    DMI_HPE_DRIVE_TYPE_SAS          = 0x03, ///< SAS
    DMI_HPE_DRIVE_TYPE_SATA_SSD     = 0x04, ///< SATA SSD
    DMI_HPE_DRIVE_TYPE_NVME_VROC    = 0x05  ///< NVMe managed by VROC/VMD
} dmi_hpe_drive_type_t;

/**
 * @brief Form factor of a drive.
 */
typedef enum dmi_hpe_drive_form
{
    DMI_HPE_DRIVE_FORM_3_5         = 0x02, ///< 3.5-inch form factor
    DMI_HPE_DRIVE_FORM_2_5         = 0x03, ///< 2.5-inch form factor
    DMI_HPE_DRIVE_FORM_1_8         = 0x04, ///< 1.8-inch form factor
    DMI_HPE_DRIVE_FORM_SMALLER     = 0x05, ///< Smaller than 1.8-inch form factor
    DMI_HPE_DRIVE_FORM_MSATA       = 0x06, ///< mSATA
    DMI_HPE_DRIVE_FORM_M2          = 0x07, ///< M.2
    DMI_HPE_DRIVE_FORM_MICRO_SSD   = 0x08, ///< MicroSSD
    DMI_HPE_DRIVE_FORM_CFAST       = 0x09, ///< CFast
    DMI_HPE_DRIVE_FORM_EDSFF       = 0x20, ///< EDSFF of unknown form factor
    DMI_HPE_DRIVE_FORM_EDSFF_1U_S  = 0x21, ///< EDSFF 1U short
    DMI_HPE_DRIVE_FORM_EDSFF_1U_L  = 0x22, ///< EDSFF 1U long
    DMI_HPE_DRIVE_FORM_EDSFF_E3_S  = 0x23, ///< EDSFF E3 short
    DMI_HPE_DRIVE_FORM_EDSFF_E3_L  = 0x24  ///< EDSFF E3 long
} dmi_hpe_drive_form_t;

/**
 * @brief Health status of a drive.
 */
typedef enum dmi_hpe_drive_health
{
    DMI_HPE_DRIVE_HEALTH_OK       = 0x00, ///< OK
    DMI_HPE_DRIVE_HEALTH_WARNING  = 0x01, ///< Warning
    DMI_HPE_DRIVE_HEALTH_CRITICAL = 0x02, ///< Critical
    DMI_HPE_DRIVE_HEALTH_UNKNOWN  = 0xFF  ///< Unknown
} dmi_hpe_drive_health_t;

/**
 * @brief HP/HPE hard drive inventory record (type 242), from Gen10 onwards.
 *
 * Describes an NVMe or SATA drive attached to the system directly, which the
 * firmware has found at boot. Drives behind a storage controller, e.g. a
 * Smart Array one, and the ones plugged in later are not described.
 */
struct dmi_hpe_drive
{
    /**
     * @brief Handle of the device correlation record (type 203).
     */
    dmi_handle_t correlation_handle;

    /**
     * @brief Type of the drive.
     */
    dmi_hpe_drive_type_t drive_type;

    /**
     * @brief Unique ID of the drive: WWID of SATA drives, and IEEE extended
     * unique identifier of NVMe ones.
     */
    uint64_t unique_id;

    /**
     * @brief Capacity of the drive in megabytes.
     */
    uint32_t legacy_capacity;

    /**
     * @brief Number of the hours the drive has been powered on for.
     */
    uint64_t power_on_hours;

    /**
     * @brief Power of the drive in watts, zero if unknown.
     */
    uint8_t power;

    /**
     * @brief Form factor of the drive.
     */
    dmi_hpe_drive_form_t form_factor;

    /**
     * @brief Health status of the drive.
     */
    dmi_hpe_drive_health_t health;

    /**
     * @brief Serial number of the drive.
     */
    const char *serial_number;

    /**
     * @brief Model number of the drive.
     */
    const char *model_number;

    /**
     * @brief Firmware revision of the drive.
     */
    const char *firmware_revision;

    /**
     * @brief Location of the drive.
     */
    const char *location;

    /**
     * @brief Encryption status of the drive, as the firmware tells it. Set
     * to `UINT8_MAX` when the structure holds none.
     */
    dmi_hpe_encryption_t encryption;

    /**
     * @brief Capacity of the drive in bytes, zero if the structure holds
     * none.
     */
    uint64_t capacity;

    /**
     * @brief Size of a logical block in bytes, zero if the structure holds
     * none.
     */
    uint32_t block_size;

    /**
     * @brief Nominal rotation speed in RPM: zero if not reported, and one
     * for solid state drives.
     */
    uint16_t rotation_speed;

    /**
     * @brief Negotiated speed of the bus in Gbit/s, zero if unknown.
     */
    uint16_t negotiated_speed;

    /**
     * @brief Fastest speed of the bus the drive is capable of in Gbit/s,
     * zero if unknown.
     */
    uint16_t capable_speed;
};

/**
 * @brief HP/HPE hard drive inventory record entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_drive_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_hpe_drive_type_name(dmi_hpe_drive_type_t value);
__dmi_api const char *dmi_hpe_drive_form_name(dmi_hpe_drive_form_t value);
__dmi_api const char *dmi_hpe_drive_health_name(dmi_hpe_drive_health_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_HPE_DRIVE_H
