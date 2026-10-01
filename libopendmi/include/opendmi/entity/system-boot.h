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

#ifndef DMI_SYSTEM_BOOT_T
#   define DMI_SYSTEM_BOOT_T
    typedef struct dmi_system_boot dmi_system_boot_t;
#endif // !DMI_SYSTEM_BOOT_T

/**
 * @brief System boot status values.
 */
typedef enum dmi_boot_status
{
    DMI_BOOT_STATUS_NO_ERRORS_DETECTED         = 0x00, ///< No errors detected
    DMI_BOOT_STATUS_NO_BOOTABLE_MEDIA          = 0x01, ///< No bootable media
    DMI_BOOT_STATUS_OS_FAILED_TO_LOAD          = 0x02, ///< Operating system failed to load
    DMI_BOOT_STATUS_FW_DETECTED_HW_FAILURE     = 0x03, ///< Hardware failure detected by the firmware
    DMI_BOOT_STATUS_OS_DETECTED_HW_FAILURE     = 0x04, ///< Hardware failure detected by the operating system
    DMI_BOOT_STATUS_USER_REQUESTED_BOOT        = 0x05, ///< Boot requested by the user
    DMI_BOOT_STATUS_SYSTEM_SECURITY_VIOLATION  = 0x06, ///< System security violation
    DMI_BOOT_STATUS_PREVIOUSLY_REQUESTED_IMAGE = 0x07, ///< Boot image requested earlier
    DMI_BOOT_STATUS_SYSTEM_WDT_EXPIRED         = 0x08, ///< System watchdog timer expired
    __DMI_BOOT_STATUS_RESERVED_START           = 0x09, ///< First reserved code
    __DMI_BOOT_STATUS_RESERVED_END             = 0x7F, ///< Last reserved code
    __DMI_BOOT_STATUS_VENDOR_SPECIFIC_START    = 0x80, ///< First vendor/OEM-specific code
    __DMI_BOOT_STATUS_VENDOR_SPECIFIC_END      = 0xBF, ///< Last vendor/OEM-specific code
    __DMI_BOOT_STATUS_PRODUCT_SPECIFIC_START   = 0xC0, ///< First product-specific code
    __DMI_BOOT_STATUS_PRODUCT_SPECIFIC_END     = 0xFF  ///< Last product-specific code
} dmi_boot_status_t;

/**
 * @brief System boot information structure (type 32).
 *
 * Tells the boot image or the management software why the system has
 * booted, for example after a hardware failure.
 */
struct dmi_system_boot
{
    /**
     * @brief Boot status.
     */
    dmi_boot_status_t status;

    /**
     * @brief Additional boot status data following the status code, as
     * stored.
     */
    dmi_binary_t status_data;

    /**
     * @brief Set if the status code is vendor/OEM-specific or
     * product-specific, so that additional status data is defined by vendor.
     * Selects whether the data is shown.
     */
    bool has_status_data;
};

/**
 * @brief System boot information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_system_boot_spec;

__BEGIN_DECLS

/**
 * @brief Get system boot status name.
 *
 * Returns the name of the system boot status, as the command line tool shows
 * it, translated into the locale when the library is built with the
 * translations.
 *
 * @param[in] value System boot status value.
 *
 * @return The name of the value, or @c nullptr if @p value has no name.
 */
__dmi_api const char *dmi_boot_status_name(dmi_boot_status_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_SYSTEM_BOOT_H
