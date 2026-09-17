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

#include <opendmi/entity/system-boot.h>

static bool dmi_system_boot_decode(dmi_entity_t *entity);

static const dmi_name_set_t dmi_system_boot_status_names =
{
    .code = "system-boot-statuses",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    },
    .ranges = (dmi_name_range_t[]){
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
        DMI_NAME_RANGE_NULL
    }
};

const dmi_entity_spec_t dmi_system_boot_spec =
{
    .code           = "system-boot",
    .name           = "System boot information",
    .description    = (const char *[]){
        "The client system firmware (for example, BIOS) communicates the "
        "System Boot Status to the client’s Pre-boot Execution Environment "
        "(PXE) boot image or OS-present management application through this "
        "structure.",
        //
        "When used in the PXE environment, for example, this code identifies "
        "the reason the PXE was initiated and can be used by boot-image "
        "software to further automate an enterprise’s PXE sessions. For "
        "example, an enterprise could choose to automatically download a "
        "hardware-diagnostic image to a client whose reason code indicated "
        "either a firmware- or an operating system-detected hardware failure.",
        //
        nullptr
    },
    .type           = DMI_TYPE(SYSTEM_BOOT),
    .required_from  = DMI_VERSION(2, 3, 0),
    .required_till  = DMI_VERSION_NONE,
    .minimum_length = 0x0B,
    .decoded_length = sizeof(dmi_system_boot_t),
    .attributes     = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_system_boot_t, status, ENUM, {
            .code   = "status",
            .name   = "Boot status",
            .values = &dmi_system_boot_status_names
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_system_boot_decode
    }
};

static bool dmi_system_boot_decode(dmi_entity_t *entity)
{
    dmi_system_boot_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_BOOT));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    // Reserved bytes
    if (not dmi_stream_skip(stream, 6 * sizeof(dmi_byte_t)))
        return false;

    // Boot status data has variable length, only the first bytes are kept
    size_t status_data_length = dmi_stream_remaining(stream);
    if (status_data_length > countof(info->status_data))
        status_data_length = countof(info->status_data);

    if (not dmi_stream_read_data(stream, info->status_data, status_data_length))
        return false;

    info->status = info->status_data[0];

    return true;
}
