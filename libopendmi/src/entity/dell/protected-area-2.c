//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/protected-area-2-internal.h>

const dmi_entity_spec_t dmi_dell_protected_area_2_spec =
{
    .type            = DMI_TYPE(DELL_PROTECTED_AREA_2),
    .code            = "dell-protected-area-2",
    .name            = "Dell protected area type 2",
    .description     = (const char *[]){
        "Describes a protected value, such as a password, which is stored in "
        "CMOS and accessed through the indexed I/O ports, and its checksum.",
        //
        "In addition to the checksum of the value, the checksum of a CMOS "
        "range is described.",
        //
        nullptr
    },
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x15,
        .decoded_length  = sizeof(dmi_dell_protected_area_2_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_dell_protected_area_2_t, token_id,          WORD),
        DMI_FIELD(dmi_dell_protected_area_2_t, value_length,      BYTE),
        DMI_FIELD(dmi_dell_protected_area_2_t, value_format,      BYTE),
        DMI_FIELD(dmi_dell_protected_area_2_t, validation_key,    WORD),
        DMI_FIELD(dmi_dell_protected_area_2_t, index_port,        WORD),
        DMI_FIELD(dmi_dell_protected_area_2_t, data_port,         WORD),
        DMI_FIELD(dmi_dell_protected_area_2_t, check_type,        BYTE),
        DMI_FIELD(dmi_dell_protected_area_2_t, value_start,       BYTE),
        DMI_FIELD(dmi_dell_protected_area_2_t, check_index,       BYTE),
        DMI_FIELD(dmi_dell_protected_area_2_t, range_check_type,  BYTE),
        DMI_FIELD(dmi_dell_protected_area_2_t, range_check_start, BYTE),
        DMI_FIELD(dmi_dell_protected_area_2_t, range_check_end,   BYTE),
        DMI_FIELD(dmi_dell_protected_area_2_t, range_check_index, BYTE),
        {}
    }),

    .attributes      = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_dell_protected_area_2_t, token_id, INTEGER, {
            .code   = "token-id",
            .name   = "Token ID",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_2_t, value_length, INTEGER, {
            .code   = "value-length",
            .name   = "Value length"
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_2_t, value_format, ENUM, {
            .code   = "value-format",
            .name   = "Value format",
            .values = &dmi_dell_value_format_names
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_2_t, validation_key, INTEGER, {
            .code   = "validation-key",
            .name   = "Validation key",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_2_t, index_port, INTEGER, {
            .code   = "index-port",
            .name   = "Index port",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_2_t, data_port, INTEGER, {
            .code   = "data-port",
            .name   = "Data port",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_2_t, check_type, ENUM, {
            .code   = "check-type",
            .name   = "Checksum type",
            .values = &dmi_dell_check_type_names
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_2_t, value_start, INTEGER, {
            .code   = "value-start",
            .name   = "Value index",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_2_t, check_index, INTEGER, {
            .code   = "check-index",
            .name   = "Checksum index",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_2_t, range_check_type, ENUM, {
            .code   = "range-check-type",
            .name   = "Range checksum type",
            .values = &dmi_dell_check_type_names
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_2_t, range_check_start, INTEGER, {
            .code   = "range-check-start",
            .name   = "Checked range start",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_2_t, range_check_end, INTEGER, {
            .code   = "range-check-end",
            .name   = "Checked range end",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_protected_area_2_t, range_check_index, INTEGER, {
            .code   = "range-check-index",
            .name   = "Range checksum index",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        {}
    })
};
