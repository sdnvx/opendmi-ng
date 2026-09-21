//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/protected-area-1-internal.h>

const dmi_entity_spec_t dmi_dell_protected_area_1_spec =
{
    .type            = DMI_TYPE(DELL_PROTECTED_AREA_1),
    .code            = "dell-protected-area-1",
    .name            = "Dell protected area type 1",
    .description     = (const char *[]){
        "Describes a protected value, such as a password, which is stored in "
        "CMOS and accessed through the indexed I/O ports, and its checksum.",
        //
        nullptr
    },
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x11,
        .decoded_length  = sizeof(dmi_dell_protected_area_1_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_dell_protected_area_1_t, token_id,       dmi_word_t),
        DMI_FIELD(dmi_dell_protected_area_1_t, value_length,   dmi_byte_t),
        DMI_FIELD(dmi_dell_protected_area_1_t, value_format,   dmi_byte_t),
        DMI_FIELD(dmi_dell_protected_area_1_t, validation_key, dmi_word_t),
        DMI_FIELD(dmi_dell_protected_area_1_t, index_port,     dmi_word_t),
        DMI_FIELD(dmi_dell_protected_area_1_t, data_port,      dmi_word_t),
        DMI_FIELD(dmi_dell_protected_area_1_t, check_type,     dmi_byte_t),
        DMI_FIELD(dmi_dell_protected_area_1_t, value_start,    dmi_byte_t),
        DMI_FIELD(dmi_dell_protected_area_1_t, check_index,    dmi_byte_t),
        {}
    }),

    .attributes      = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_dell_protected_area_1_t, token_id, INTEGER, {
            .code   = "token-id",
            .name   = "Token ID",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_1_t, value_length, INTEGER, {
            .code   = "value-length",
            .name   = "Value length"
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_1_t, value_format, ENUM, {
            .code   = "value-format",
            .name   = "Value format",
            .values = &dmi_dell_value_format_names
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_1_t, validation_key, INTEGER, {
            .code   = "validation-key",
            .name   = "Validation key",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_1_t, index_port, INTEGER, {
            .code   = "index-port",
            .name   = "Index port",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_1_t, data_port, INTEGER, {
            .code   = "data-port",
            .name   = "Data port",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_1_t, check_type, ENUM, {
            .code   = "check-type",
            .name   = "Checksum type",
            .values = &dmi_dell_check_type_names
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_1_t, value_start, INTEGER, {
            .code   = "value-start",
            .name   = "Value index",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_1_t, check_index, INTEGER, {
            .code   = "check-index",
            .name   = "Checksum index",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        {}
    })
};
