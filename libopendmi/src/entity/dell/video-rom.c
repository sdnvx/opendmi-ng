//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/video-rom.h>

const dmi_entity_spec_t dmi_dell_video_rom_spec =
{
    .type        = DMI_TYPE(dell_video_rom),
    .code        = "dell-video-rom",
    .name        = "Dell video BIOS information",
    .description = (const char *[]){
        "Tells the vendor and the version of the video BIOS.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x09,
        .decoded_length = sizeof(dmi_dell_video_rom_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_STRING(dmi_dell_video_rom_t, vendor),
        DMI_FIELD_STRING(dmi_dell_video_rom_t, version),
        DMI_FIELD(dmi_dell_video_rom_t, unknown_1, dmi_byte_t),
        DMI_FIELD(dmi_dell_video_rom_t, unknown_2, dmi_word_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_dell_video_rom_t, vendor, STRING, {
            .code = "vendor",
            .name = "Vendor"
        }),
        DMI_ATTRIBUTE(dmi_dell_video_rom_t, version, STRING, {
            .code = "version",
            .name = "Version"
        }),
        DMI_ATTRIBUTE(dmi_dell_video_rom_t, unknown_1, INTEGER, {
            .code  = "unknown-1",
            .name  = "Unknown 1",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_video_rom_t, unknown_2, INTEGER, {
            .code  = "unknown-2",
            .name  = "Unknown 2",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        {}
    })
};
