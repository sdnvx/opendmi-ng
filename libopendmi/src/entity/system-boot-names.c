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

#include <opendmi/entity/system-boot-internal.h>

const dmi_name_set_t dmi_system_boot_status_names =
{
    .code = "system-boot-status",
    .names = DMI_NAMES({
        {
            .id   = DMI_BOOT_STATUS_NO_ERRORS_DETECTED,
            .code = "no-errors-detected",
            .name = "No errors detected"
        },
        {
            .id   = DMI_BOOT_STATUS_NO_BOOTABLE_MEDIA,
            .code = "no-bootable-media",
            .name = "No bootable media"
        },
        {
            .id   = DMI_BOOT_STATUS_OS_FAILED_TO_LOAD,
            .code = "os-failed-to-load",
            .name = "Operating system failed to load"
        },
        {
            .id   = DMI_BOOT_STATUS_FW_DETECTED_HW_FAILURE,
            .code = "fw-detected-hw-failure",
            .name = "Firmware-detected hardware failure"
        },
        {
            .id   = DMI_BOOT_STATUS_OS_DETECTED_HW_FAILURE,
            .code = "os-detected-hw-failure",
            .name = "Operating system-detected hardware failure"
        },
        {
            .id   = DMI_BOOT_STATUS_USER_REQUESTED_BOOT,
            .code = "user-requested-boot",
            .name = "User-requested boot"
        },
        {
            .id   = DMI_BOOT_STATUS_SYSTEM_SECURITY_VIOLATION,
            .code = "system-security-violation",
            .name = "System security violation"
        },
        {
            .id   = DMI_BOOT_STATUS_PREVIOUSLY_REQUESTED_IMAGE,
            .code = "previously-requested-image",
            .name = "Previously requested image"
        },
        {
            .id   = DMI_BOOT_STATUS_SYSTEM_WDT_EXPIRED,
            .code = "system-wdt-expired",
            .name = "System watchdog timer expired"
        },
        {}
    }),
    .ranges = DMI_NAME_RANGES({
        {
            .start_id = __DMI_BOOT_STATUS_RESERVED_START,
            .end_id   = __DMI_BOOT_STATUS_RESERVED_END,
            .code     = "reserved",
            .name     = "Reserved"
        },
        {
            .start_id = __DMI_BOOT_STATUS_VENDOR_SPECIFIC_START,
            .end_id   = __DMI_BOOT_STATUS_VENDOR_SPECIFIC_END,
            .code     = "vendor-specific",
            .name     = "Vendor/OEM-specific"
        },
        {
            .start_id = __DMI_BOOT_STATUS_PRODUCT_SPECIFIC_START,
            .end_id   = __DMI_BOOT_STATUS_PRODUCT_SPECIFIC_END,
            .code     = "product-specific",
            .name     = "Product-specific"
        },
        {}
    })
};
