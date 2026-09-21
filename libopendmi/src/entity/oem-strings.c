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

#include <opendmi/entity/oem-strings-internal.h>

const dmi_entity_spec_t dmi_oem_strings_spec =
{
    .code            = "oem-strings",
    .name            = "OEM strings",
    .description     = (const char *[]){
        "This structure contains free-form strings defined by the OEM. "
        "Examples are part numbers for system reference documents, contact "
        "information for the manufacturer, and so on.",
        //
        nullptr
    },
    .type            = DMI_TYPE(OEM_STRINGS),
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x05,
        .decoded_length  = sizeof(dmi_oem_strings_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_oem_strings_t, string_count, BYTE),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_oem_strings_t, string_count, INTEGER, {
            .code = "string-count",
            .name = "String count"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_oem_strings_t, strings, string_count, STRING, {
            .code    = "strings",
            .name    = "Strings"
        }),
        {}
    }),

    .handlers = {
        .derive  = dmi_oem_strings_derive,
        .cleanup = dmi_oem_strings_cleanup
    }
};
