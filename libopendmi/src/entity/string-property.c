//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/registry.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/string-property-internal.h>

const dmi_entity_spec_t dmi_string_property_spec =
{
    .code            = "string-property",
    .name            = "String property",
    .description     = (const char *[]){
        "This structure defines a string property for another structure. This "
        "allows adding string properties that are common to several "
        "structures without having to modify the definitions of these "
        "structures. Multiple type 46 structures can add string properties "
        "to the same parent structure.",
        //
        nullptr
    },
    .type            = DMI_TYPE(STRING_PROPERTY),
    .params = {
        .minimum_version = DMI_VERSION(3, 5, 0),
        .minimum_length  = 0x09,
        .decoded_length  = sizeof(dmi_string_property_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_string_property_t, ident,         WORD),
        DMI_FIELD(dmi_string_property_t, value,         STRING),
        DMI_FIELD(dmi_string_property_t, parent_handle, WORD),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_string_property_t, ident, ENUM, {
            .code   = "ident",
            .name   = "Identifier",
            .values = &dmi_property_names
        }),
        DMI_ATTRIBUTE(dmi_string_property_t, value, STRING, {
            .code = "value",
            .name = "Value"
        }),
        DMI_ATTRIBUTE(dmi_string_property_t, parent_handle, HANDLE, {
            .code = "parent-handle",
            .name = "Parent handle"
        }),
        {}
    }),

    .handlers = {
        .link   = dmi_string_property_link
    }
};
