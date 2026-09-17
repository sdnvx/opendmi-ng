//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SYSTEM_BOOT_H
#define OPENDMI_ENTITY_SYSTEM_BOOT_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_system_boot dmi_system_boot_t;

/**
 * @brief System boot status values.
 */
typedef enum dmi_boot_status
{
    DMI_BOOT_STATUS_NO_ERRORS_DETECTED         = 0x00,
    DMI_BOOT_STATUS_NO_BOOTABLE_MEDIA          = 0x01,
    DMI_BOOT_STATUS_OS_FAILED_TO_LOAD          = 0x02,
    DMI_BOOT_STATUS_FW_DETECTED_HW_FAILURE     = 0x03,
    DMI_BOOT_STATUS_OS_DETECTED_HW_FAILURE     = 0x04,
    DMI_BOOT_STATUS_USER_REQUESTED_BOOT        = 0x05,
    DMI_BOOT_STATUS_SYSTEM_SECURITY_VIOLATION  = 0x06,
    DMI_BOOT_STATUS_PREVIOUSLY_REQUESTED_IMAGE = 0x07,
    DMI_BOOT_STATUS_SYSTEM_WDT_EXPIRED         = 0x08,
    __DMI_BOOT_STATUS_RESERVED_START           = 0x09,
    __DMI_BOOT_STATUS_RESERVED_END             = 0x7F,
    __DMI_BOOT_STATUS_VENDOR_SPECIFIC_START    = 0x80,
    __DMI_BOOT_STATUS_VENDOR_SPECIFIC_END      = 0xBF,
    __DMI_BOOT_STATUS_PRODUCT_SPECIFIC_START   = 0xC0,
    __DMI_BOOT_STATUS_PRODUCT_SPECIFIC_END     = 0xFF
} dmi_boot_status_t;

/**
 * @brief System boot information structure (type 32).
 */
struct dmi_system_boot
{
    /**
     * @brief Boot status.
     */
    dmi_boot_status_t status;

    /**
     * @brief Raw boot status data. Might be useful for vendor/OEM-specific
     * or product-specific implementations.
     */
    dmi_byte_t status_data[10];
};

/**
 * @brief System boot information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_system_boot_spec;

#endif // !OPENDMI_ENTITY_SYSTEM_BOOT_H
