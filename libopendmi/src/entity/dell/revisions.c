//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/revisions-internal.h>

const dmi_entity_spec_t dmi_dell_revisions_spec =
{
    .type            = DMI_TYPE(DELL_REVISIONS),
    .code            = "dell-revisions",
    .name            = "Dell revisions and IDs",
    .params = {
        .minimum_version = DMI_VERSION(2, 2, 0),
        .minimum_length  = 0x08,
        .decoded_length  = sizeof(dmi_dell_revisions_t)
    },

    .fields = DMI_FIELDS({
        // Major and minor numbers are one byte each, in this order
        DMI_FIELD(dmi_dell_revisions_t, impl_version, WORD,
                  .convert = dmi_dell_revisions_convert_version),

        DMI_FIELD(dmi_dell_revisions_t, system_id,         BYTE),
        DMI_FIELD(dmi_dell_revisions_t, hardware_revision, BYTE),

        // Systems whose identifier does not fit into one byte carry it here
        DMI_FIELD_GROUP(),
        DMI_FIELD_EXTENDED(dmi_dell_revisions_t, system_id, WORD, .when_raw = 0xFEu),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_dell_revisions_t, manufacture_date,   STRING),
        DMI_FIELD(dmi_dell_revisions_t, first_poweron_date, STRING),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_dell_revisions_t, impl_version, VERSION, {
            .code  = "implementation-version",
            .name  = "Implementation version",
            .scale = 2
        }),
        DMI_ATTRIBUTE(dmi_dell_revisions_t, system_id, INTEGER, {
            .code  = "system-id",
            .name  = "System ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_revisions_t, hardware_revision, INTEGER, {
            .code = "hardware-revision",
            .name = "Hardware revision"
        }),
        DMI_ATTRIBUTE(dmi_dell_revisions_t, manufacture_date, STRING, {
            .code = "manufacture-date",
            .name = "Manufacture date"
        }),
        DMI_ATTRIBUTE(dmi_dell_revisions_t, first_poweron_date, STRING, {
            .code = "first-poweron-date",
            .name = "First power-on date"
        }),
        {}
    })
};
