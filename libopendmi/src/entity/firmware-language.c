//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/firmware-language-internal.h>

const dmi_entity_spec_t dmi_firmware_language_spec =
{
    .code            = "firmware-language",
    .name            = "Firmware language information",
    .description     = (const char *[]){
        "The information in this structure defines the installable language "
        "attributes of the platform firmware.",
        //
        nullptr
    },
    .type            = DMI_TYPE(FIRMWARE_LANGUAGE),
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x16,
        .decoded_length  = sizeof(dmi_firmware_language_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_firmware_language_t, language_count, BYTE),
        DMI_FIELD(dmi_firmware_language_t, flags,          BYTE),

        // Reserved bytes
        DMI_FIELD_SKIP(15 * sizeof(dmi_byte_t)),

        DMI_FIELD(dmi_firmware_language_t, current_language, STRING),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE_ARRAY(dmi_firmware_language_t, languages, language_count, STRING, {
            .code   = "languages",
            .name   = "Languages"
        }),
        DMI_ATTRIBUTE(dmi_firmware_language_t, flags, SET, {
            .code   = "flags",
            .name   = "Flags",
            .values = &dmi_firmware_language_flag_names
        }),
        DMI_ATTRIBUTE(dmi_firmware_language_t, current_language, STRING, {
            .code   = "current-language",
            .name   = "Current language"
        }),
        {}
    }),

    .handlers = {
        .derive  = dmi_firmware_language_derive,
        .cleanup = dmi_firmware_language_cleanup
    }
};
