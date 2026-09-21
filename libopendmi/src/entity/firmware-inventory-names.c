//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/firmware-inventory-internal.h>

const dmi_name_set_t dmi_version_format_names =
{
    .code  = "version-format",
    .names = DMI_NAMES({
        {
            .id   = DMI_VERSION_FORMAT_FREE,
            .code = "free-form",
            .name = "Free form"
        },
        {
            .id   = DMI_VERSION_FORMAT_SEMANTIC,
            .code = "semantic",
            .name = "Semantic"
        },
        {
            .id   = DMI_VERSION_FORMAT_HEX_32,
            .code = "hexadecimal-32",
            .name = "Hexadecimal (32-bit)"
        },
        {
            .id   = DMI_VERSION_FORMAT_HEX_64,
            .code = "hexadecimal-64",
            .name = "Hexadecimal (64-bit)"
        },
        {}
    })
};

const dmi_name_set_t dmi_firmware_ident_format_names =
{
    .code  = "firmware-ident-format",
    .names = DMI_NAMES({
        {
            .id   = DMI_FIRMWARE_IDENT_FORMAT_FREE,
            .code = "free-form",
            .name = "Free form"
        },
        {
            .id   = DMI_FIRMWARE_IDENT_FORMAT_GUID,
            .code = "guid",
            .name = "GUID"
        },
        {}
    }),
    .ranges = DMI_NAME_RANGES({
        {
            .start_id = __DMI_FIRMWARE_IDENT_FORMAT_RESERVED_START,
            .end_id   = __DMI_FIRMWARE_IDENT_FORMAT_RESERVED_END,
            .code     = "reserved",
            .name     = "Reserved"
        },
        {
            .start_id = __DMI_FIRMWARE_IDENT_FORMAT_VENDOR_SPECIFIC_START,
            .end_id   = __DMI_FIRMWARE_IDENT_FORMAT_VENDOR_SPECIFIC_END,
            .code     = "vendor-specific",
            .name     = "Vendor/OEM-specific"
        },
        {}
    })
};

const dmi_name_set_t dmi_firmware_inventory_feature_names =
{
    .code  = "firmware-inventory-feature",
    .names = DMI_NAMES({
        {
            .id   = 0,
            .code = "is-updatable",
            .name = "Updatable"
        },
        {
            .id   = 1,
            .code = "is-write-protected",
            .name = "Write-protected"
        },
        {}
    })
};

const dmi_name_set_t dmi_firmware_inventory_state_names =
{
    .code  = "firmware-inventory-state",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_FIRMWARE_INVENTORY_STATE_UNSPEC),
        DMI_NAME_OTHER(DMI_FIRMWARE_INVENTORY_STATE_OTHER),
        DMI_NAME_UNKNOWN(DMI_FIRMWARE_INVENTORY_STATE_UNKNOWN),
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_DISABLED,
            .code = "disabled",
            .name = "Disabled"
        },
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_ENABLED,
            .code = "enabled",
            .name = "Enabled"
        },
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_ABSENT,
            .code = "absent",
            .name = "Absent"
        },
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_STANDBY_OFFLINE,
            .code = "standby-offline",
            .name = "Standby offline"
        },
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_STANDBY_SPARE,
            .code = "standby-spare",
            .name = "Standby spare"
        },
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_UNAVAIL_OFFLINE,
            .code = "unavail-offline",
            .name = "Unavailable offline"
        },
        {}
    })
};

const char *dmi_version_format_name(dmi_version_format_t value)
{
    return dmi_name_lookup(&dmi_version_format_names, (int)value);
}

const char *dmi_firmware_ident_format_name(dmi_firmware_ident_format_t value)
{
    return dmi_name_lookup(&dmi_firmware_ident_format_names, (int)value);
}

const char *dmi_firmware_inventory_state_name(dmi_firmware_inventory_state_t value)
{
    return dmi_name_lookup(&dmi_firmware_inventory_state_names, (int)value);
}
