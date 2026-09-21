//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <string.h>
#include <opendmi/stream.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/firmware-internal.h>

const dmi_entity_spec_t dmi_firmware_spec =
{
    .code = "firmware",
    .name = "Platform firmware information",
    .type = DMI_TYPE(FIRMWARE),

    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .required_from   = DMI_VERSION(2, 3, 0),
        .required_till   = DMI_VERSION_NONE,
        .unique          = true,
        .minimum_length  = 0x12,
        .decoded_length  = sizeof(dmi_firmware_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_firmware_t, vendor,       STRING),
        DMI_FIELD(dmi_firmware_t, version,      STRING),
        DMI_FIELD(dmi_firmware_t, bios_segment, WORD),

        DMI_FIELD(dmi_firmware_t, release_date, STRING,
                  .decode = dmi_firmware_decode_date,
                  .encode = dmi_firmware_encode_date),

        // ROM size is carried as the number of the granules it takes, and the
        // chips too large for one byte carry it in the extended field instead
        DMI_FIELD(dmi_firmware_t, rom_size, BYTE,
                  .decode = dmi_firmware_decode_rom_size,
                  .encode = dmi_firmware_encode_rom_size),

        DMI_FIELD(dmi_firmware_t, features, QWORD),

        // Extension bytes were added one at a time, so the structures of the
        // platforms older than the second one end between them
        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 1, 0)),
        DMI_FIELD(dmi_firmware_t, features_ex.__value[0], BYTE),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 3, 0)),
        DMI_FIELD(dmi_firmware_t, features_ex.__value[1], BYTE),

        // Versions are one byte of major and one of minor, and the major
        // number of 0xFF says that the platform carries no version at all
        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 4, 0)),
        DMI_FIELD(dmi_firmware_t, platform_version, WORD,
                  .decode = dmi_firmware_decode_version,
                  .encode = dmi_firmware_encode_version),
        DMI_FIELD(dmi_firmware_t, controller_version, WORD,
                  .decode = dmi_firmware_decode_version,
                  .encode = dmi_firmware_encode_version),

        DMI_FIELD_GROUP(.since = DMI_VERSION(3, 1, 0)),
        DMI_FIELD_EXTENDED(dmi_firmware_t, rom_size, WORD,
                           .when_raw = DMI_FIRMWARE_ROM_SIZE_EXTENDED,
                           .decode   = dmi_firmware_decode_rom_size_ex,
                           .encode   = dmi_firmware_encode_rom_size_ex),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_firmware_t, vendor, STRING, {
            .code   = "vendor",
            .name   = "Vendor"
        }),
        DMI_ATTRIBUTE(dmi_firmware_t, version, STRING, {
            .code   = "version",
            .name   = "Version"
        }),
        DMI_ATTRIBUTE(dmi_firmware_t, bios_segment, ADDRESS, {
            .code   = "bios-segment",
            .name   = "BIOS segment",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_firmware_t, release_date, DATE, {
            .code   = "release-date",
            .name   = "Release date",
            .unspec = dmi_value_ptr(DMI_DATE_NONE)
        }),
        DMI_ATTRIBUTE(dmi_firmware_t, rom_size, SIZE, {
            .code   = "rom-size",
            .name   = "ROM size"
        }),
        DMI_ATTRIBUTE(dmi_firmware_t, features, SET, {
            .code   = "features",
            .name   = "Features",
            .values = &dmi_firmware_feature_names
        }),
        DMI_ATTRIBUTE(dmi_firmware_t, features_ex, SET, {
            .code   = "features-ex",
            .name   = "Extra features",
            .values = &dmi_firmware_feature_ex_names,
            .level  = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_firmware_t, platform_version, VERSION, {
            .code   = "platform-version",
            .name   = "Platform firmware version",
            .scale  = 2,
            .unspec = dmi_value_ptr(DMI_VERSION_NONE),
            .level  = DMI_VERSION(2, 4, 0)
        }),
        DMI_ATTRIBUTE(dmi_firmware_t, controller_version, VERSION, {
            .code   = "controller-version",
            .name   = "Embedded controller firmware version",
            .scale  = 2,
            .unspec = dmi_value_ptr(DMI_VERSION_NONE),
            .level  = DMI_VERSION(2, 4, 0)
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("firmware.rom-size", dmi_firmware_lint_rom_size, {
            .name              = "Extended ROM size is present when the plain one needs it",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        DMI_LINT_RULE("firmware.release-date", dmi_firmware_lint_release_date, {
            .name              = "Release date is written the way the specification requires",
            .severity          = DMI_LINT_SEVERITY_NOTE,
            .producer_severity = DMI_LINT_SEVERITY_WARNING
        }),
        {}
    })
};
