//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/apple.h>

#include <opendmi/entity/apple/firmware-volume-internal.h>

const dmi_entity_spec_t dmi_apple_firmware_volume_spec =
{
    .type        = DMI_TYPE(apple_firmware_volume),
    .code        = "apple-firmware-volume",
    .name        = "Apple firmware volume information",
    .description = (const char *[]){
        "Tells the features of the firmware and the layout of its flash "
        "memory.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x58,
        .decoded_length = sizeof(dmi_apple_firmware_volume_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_apple_firmware_volume_t, region_count, dmi_byte_t),
        DMI_FIELD_SKIP(3 * sizeof(dmi_byte_t)),
        DMI_FIELD(dmi_apple_firmware_volume_t, features,      dmi_dword_t),
        DMI_FIELD(dmi_apple_firmware_volume_t, features_mask, dmi_dword_t),

        // Types of the regions come in a row before the regions themselves
        DMI_FIELD_VECTOR(dmi_apple_firmware_volume_t, region_types,
            .fields = DMI_FIELDS({
                DMI_FIELD_ELEMENT(dmi_apple_firmware_volume_t, region_types, dmi_byte_t),
                {}
            })),
        DMI_FIELD_VECTOR(dmi_apple_firmware_volume_t, regions,
            .fields = DMI_FIELDS({
                DMI_FIELD(dmi_apple_flash_region_t, start_address, dmi_dword_t),
                DMI_FIELD(dmi_apple_flash_region_t, end_address,   dmi_dword_t),
                {}
            })),

        // Extended features the older firmware does not tell
        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_apple_firmware_volume_t, extended_features,      dmi_dword_t),
        DMI_FIELD(dmi_apple_firmware_volume_t, extended_features_mask, dmi_dword_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_apple_firmware_volume_t, region_count, INTEGER, {
            .code = "region-count",
            .name = "Number of regions"
        }),
        DMI_ATTRIBUTE(dmi_apple_firmware_volume_t, features, SET, {
            .code   = "features",
            .name   = "Firmware features",
            .values = &dmi_apple_firmware_feature_names
        }),
        DMI_ATTRIBUTE(dmi_apple_firmware_volume_t, features_mask, INTEGER, {
            .code  = "features-mask",
            .name  = "Firmware features mask",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE_VECTOR(dmi_apple_firmware_volume_t, regions, STRUCT, {
            .code  = "regions",
            .name  = "Regions",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_apple_flash_region_t, type, ENUM, {
                    .code   = "type",
                    .name   = "Type",
                    .values = &dmi_apple_region_type_names
                }),
                DMI_ATTRIBUTE(dmi_apple_flash_region_t, start_address, ADDRESS, {
                    .code = "start-address",
                    .name = "Start address"
                }),
                DMI_ATTRIBUTE(dmi_apple_flash_region_t, end_address, ADDRESS, {
                    .code = "end-address",
                    .name = "End address"
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE(dmi_apple_firmware_volume_t, extended_features, SET, {
            .code   = "extended-features",
            .name   = "Extended firmware features",
            .values = &dmi_apple_extended_feature_names
        }),
        DMI_ATTRIBUTE(dmi_apple_firmware_volume_t, extended_features_mask, INTEGER, {
            .code  = "extended-features-mask",
            .name  = "Extended firmware features mask",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_apple_firmware_volume_derive
    }
};

// Bits of no known meaning are named after their numbers, the way OpenCore
// names them, since they are not reserved
const dmi_name_set_t dmi_apple_firmware_feature_names =
{
    .code  = "apple-firmware-feature",
    .names = DMI_NAMES({
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_CSM_LEGACY_MODE,
            .code = "csm-legacy-mode",
            .name = "Legacy boot (CSM)"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_CD_DRIVE_BOOT,
            .code = "cd-drive-boot",
            .name = "Optical drive boot"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_TARGET_DISK_MODE,
            .code = "target-disk-mode",
            .name = "Target disk mode"
        },
        {
            .id   = 3,
            .code = "unknown-bit-3",
            .name = "Unknown bit 3"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_NET_BOOT,
            .code = "net-boot",
            .name = "Network boot"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_SLING_SHOT,
            .code = "sling-shot",
            .name = "Embedded recovery (SlingShot)"
        },
        {
            .id   = 6,
            .code = "unknown-bit-6",
            .name = "Unknown bit 6"
        },
        {
            .id   = 7,
            .code = "unknown-bit-7",
            .name = "Unknown bit 7"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_WIRELESS,
            .code = "wireless",
            .name = "Wireless networks"
        },
        {
            .id   = 9,
            .code = "unknown-bit-9",
            .name = "Unknown bit 9"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_SECURITY_POLICY_1,
            .code = "security-policy-1",
            .name = "Platform security policy bit 0"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_SECURITY_POLICY_2,
            .code = "security-policy-2",
            .name = "Platform security policy bit 1"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_FIRMWARE_LOCKED,
            .code = "firmware-locked",
            .name = "Firmware locked"
        },
        {
            .id   = 13,
            .code = "unknown-bit-13",
            .name = "Unknown bit 13"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_HIGH_SPEED_USB,
            .code = "high-speed-usb",
            .name = "USB 2.0"
        },
        {
            .id   = 15,
            .code = "unknown-bit-15",
            .name = "Unknown bit 15"
        },
        {
            .id   = 16,
            .code = "unknown-bit-16",
            .name = "Unknown bit 16"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_NO_USB_SUBSTITUTE_WORKAROUND,
            .code = "no-usb-substitute-workaround",
            .name = "No USB port substitution"
        },
        {
            .id   = 18,
            .code = "unknown-bit-18",
            .name = "Unknown bit 18"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_APFS,
            .code = "apfs",
            .name = "APFS"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_APFS_EXTRA,
            .code = "apfs-extra",
            .name = "APFS (extra)"
        },
        {
            .id   = 21,
            .code = "unknown-bit-21",
            .name = "Unknown bit 21"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_TRBX,
            .code = "trbx",
            .name = "Tamper resistant boot X"
        },
        {
            .id   = 23,
            .code = "unknown-bit-23",
            .name = "Unknown bit 23"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_SECURITY_POLICY,
            .code = "security-policy",
            .name = "Platform security policy"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_EXTENDED_FEATURES,
            .code = "extended-features",
            .name = "Extended firmware features"
        },
        {
            .id   = 26,
            .code = "unknown-bit-26",
            .name = "Unknown bit 26"
        },
        {
            .id   = 27,
            .code = "unknown-bit-27",
            .name = "Unknown bit 27"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_NO_MBA_S4_WORKAROUND,
            .code = "no-mba-s4-workaround",
            .name = "No memory map adjustment on hibernation wake"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_UEFI_WINDOWS_BOOT,
            .code = "uefi-windows-boot",
            .name = "UEFI Windows boot"
        },
        {
            .id   = 30,
            .code = "unknown-bit-30",
            .name = "Unknown bit 30"
        },
        {
            .id   = DMI_APPLE_FIRMWARE_FEATURE_NO_BOOTSCRIPT_WORKAROUND,
            .code = "no-bootscript-workaround",
            .name = "No memory map adjustment for boot script"
        },
        {}
    })
};

// Bits of no known meaning are named after their numbers, the way OpenCore
// names them, since they are not reserved
const dmi_name_set_t dmi_apple_extended_feature_names =
{
    .code  = "apple-extended-feature",
    .names = DMI_NAMES({
        {
            .id   = 0,
            .code = "unknown-bit-32",
            .name = "Unknown bit 32"
        },
        {
            .id   = 1,
            .code = "unknown-bit-33",
            .name = "Unknown bit 33"
        },
        {
            .id   = 2,
            .code = "unknown-bit-34",
            .name = "Unknown bit 34"
        },
        {
            .id   = DMI_APPLE_EXTENDED_FEATURE_LARGE_BASESYSTEM,
            .code = "large-basesystem",
            .name = "Large BaseSystem"
        },
        {}
    })
};

const dmi_name_set_t dmi_apple_region_type_names =
{
    .code  = "apple-region-type",
    .names = DMI_NAMES({
        {
            .id   = DMI_APPLE_REGION_TYPE_UNUSED,
            .code = "unused",
            .name = "Unused"
        },
        {
            .id   = DMI_APPLE_REGION_TYPE_RECOVERY,
            .code = "recovery",
            .name = "Recovery"
        },
        {
            .id   = DMI_APPLE_REGION_TYPE_MAIN,
            .code = "main",
            .name = "Main firmware"
        },
        {
            .id   = DMI_APPLE_REGION_TYPE_NVRAM,
            .code = "nvram",
            .name = "NVRAM"
        },
        {
            .id   = DMI_APPLE_REGION_TYPE_CONFIG,
            .code = "config",
            .name = "Configuration"
        },
        {
            .id   = DMI_APPLE_REGION_TYPE_DIAGVAULT,
            .code = "diagvault",
            .name = "Diagnostics vault"
        },
        {}
    })
};

const char *dmi_apple_firmware_feature_name(dmi_apple_firmware_feature_t value)
{
    return dmi_name_lookup(&dmi_apple_firmware_feature_names, (int)value);
}

const char *dmi_apple_extended_feature_name(dmi_apple_extended_feature_t value)
{
    return dmi_name_lookup(&dmi_apple_extended_feature_names, (int)value);
}

const char *dmi_apple_region_type_name(dmi_apple_region_type_t value)
{
    return dmi_name_lookup(&dmi_apple_region_type_names, (int)value);
}

bool dmi_apple_firmware_volume_derive(dmi_entity_t *entity)
{
    dmi_apple_firmware_volume_t *info = dmi_entity_info(entity, DMI_TYPE(apple_firmware_volume));
    if (info == nullptr)
        return false;

    // Each region takes its type from the map which precedes the regions
    for (size_t i = 0; i < DMI_APPLE_FLASH_REGION_COUNT; i++)
        info->regions[i].type = (dmi_apple_region_type_t)info->region_types[i];

    return true;
}
