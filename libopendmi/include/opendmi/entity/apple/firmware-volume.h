//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_APPLE_FIRMWARE_VOLUME_H
#define OPENDMI_ENTITY_APPLE_FIRMWARE_VOLUME_H

#pragma once

#include <opendmi/entity.h>

#define DMI_APPLE_FLASH_REGION_COUNT 8

typedef struct dmi_apple_flash_region    dmi_apple_flash_region_t;
typedef struct dmi_apple_firmware_volume dmi_apple_firmware_volume_t;

/**
 * @brief Firmware features, which macOS and its boot loader read.
 */
typedef enum dmi_apple_firmware_feature
{
    DMI_APPLE_FIRMWARE_FEATURE_CSM_LEGACY_MODE              = 0,  ///< Supports booting legacy operating systems (CSM)
    DMI_APPLE_FIRMWARE_FEATURE_CD_DRIVE_BOOT                = 1,  ///< Supports booting from optical drives
    DMI_APPLE_FIRMWARE_FEATURE_TARGET_DISK_MODE             = 2,  ///< Supports target disk mode
    DMI_APPLE_FIRMWARE_FEATURE_NET_BOOT                     = 4,  ///< Supports network boot
    DMI_APPLE_FIRMWARE_FEATURE_SLING_SHOT                   = 5,  ///< Supports recovery embedded into the firmware
    DMI_APPLE_FIRMWARE_FEATURE_WIRELESS                     = 8,  ///< Supports wireless networks in the firmware
    DMI_APPLE_FIRMWARE_FEATURE_SECURITY_POLICY_1            = 10, ///< Platform security policy bit 0
    DMI_APPLE_FIRMWARE_FEATURE_SECURITY_POLICY_2            = 11, ///< Platform security policy bit 1
    DMI_APPLE_FIRMWARE_FEATURE_FIRMWARE_LOCKED              = 12, ///< Firmware is locked
    DMI_APPLE_FIRMWARE_FEATURE_HIGH_SPEED_USB               = 14, ///< Supports USB 2.0 in the firmware
    DMI_APPLE_FIRMWARE_FEATURE_NO_USB_SUBSTITUTE_WORKAROUND = 17, ///< USB ports are not substituted
    DMI_APPLE_FIRMWARE_FEATURE_APFS                         = 19, ///< Supports APFS
    DMI_APPLE_FIRMWARE_FEATURE_APFS_EXTRA                   = 20, ///< Supports APFS, unused
    DMI_APPLE_FIRMWARE_FEATURE_TRBX                         = 22, ///< Supports tamper resistant boot X
    DMI_APPLE_FIRMWARE_FEATURE_SECURITY_POLICY              = 24, ///< Supports platform security policy
    DMI_APPLE_FIRMWARE_FEATURE_EXTENDED_FEATURES            = 25, ///< Supports extended firmware features
    DMI_APPLE_FIRMWARE_FEATURE_NO_MBA_S4_WORKAROUND         = 28, ///< Memory map is not adjusted on hibernation wake
    DMI_APPLE_FIRMWARE_FEATURE_UEFI_WINDOWS_BOOT            = 29, ///< Supports UEFI Windows boot
    DMI_APPLE_FIRMWARE_FEATURE_NO_BOOTSCRIPT_WORKAROUND     = 31  ///< Memory map is not adjusted for boot script
} dmi_apple_firmware_feature_t;

/**
 * @brief Extended firmware features, the high 32 bits of the 64-bit firmware
 * features.
 */
typedef enum dmi_apple_extended_feature
{
    DMI_APPLE_EXTENDED_FEATURE_LARGE_BASESYSTEM = 3 ///< Supports large BaseSystem, required by macOS 12
} dmi_apple_extended_feature_t;

/**
 * @brief Type of a region of the flash memory of the firmware.
 */
typedef enum dmi_apple_region_type
{
    DMI_APPLE_REGION_TYPE_UNUSED    = 0, ///< Entry is not used
    DMI_APPLE_REGION_TYPE_RECOVERY  = 1, ///< Recovery
    DMI_APPLE_REGION_TYPE_MAIN      = 2, ///< Main firmware
    DMI_APPLE_REGION_TYPE_NVRAM     = 3, ///< Non-volatile variables
    DMI_APPLE_REGION_TYPE_CONFIG    = 4, ///< Configuration
    DMI_APPLE_REGION_TYPE_DIAGVAULT = 5  ///< Diagnostics vault
} dmi_apple_region_type_t;

/**
 * @brief Region of the flash memory of the firmware.
 */
struct dmi_apple_flash_region
{
    /**
     * @brief Type of the region, from the region type map of the structure.
     */
    dmi_apple_region_type_t type;

    /**
     * @brief Start address of the region.
     */
    uint32_t start_address;

    /**
     * @brief End address of the region.
     */
    uint32_t end_address;
};

/**
 * @brief Apple firmware volume information structure (type 128).
 *
 * Tells the features of the firmware and the layout of its flash memory.
 * Laid out as the `AppleSmBios.h` header of OpenCore describes it.
 */
struct dmi_apple_firmware_volume
{
    /**
     * @brief Number of the regions of the flash memory.
     */
    uint8_t region_count;

    /**
     * @brief Features of the firmware, a set of `dmi_apple_firmware_feature_t`
     * bits.
     */
    uint32_t features;

    /**
     * @brief Mask of the features the firmware tells.
     */
    uint32_t features_mask;

    /**
     * @brief Types of the regions, as the data holds them.
     */
    uint8_t region_types[DMI_APPLE_FLASH_REGION_COUNT];

    /**
     * @brief Regions of the flash memory, of which the entries past
     * `region_count` are not used.
     */
    dmi_apple_flash_region_t regions[DMI_APPLE_FLASH_REGION_COUNT];

    /**
     * @brief Extended features of the firmware, the high 32 bits of the
     * 64-bit features, a set of `dmi_apple_extended_feature_t` bits. Set to
     * zero when the structure is too short to hold them.
     */
    uint32_t extended_features;

    /**
     * @brief Mask of the extended features the firmware tells. Set to zero
     * when the structure is too short to hold it.
     */
    uint32_t extended_features_mask;
};

/**
 * @brief Apple firmware volume information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_apple_firmware_volume_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_apple_firmware_feature_name(dmi_apple_firmware_feature_t value);
__dmi_api const char *dmi_apple_extended_feature_name(dmi_apple_extended_feature_t value);
__dmi_api const char *dmi_apple_region_type_name(dmi_apple_region_type_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_APPLE_FIRMWARE_VOLUME_H
