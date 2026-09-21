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

#include <opendmi/entity/bis-entry-point-internal.h>

const dmi_entity_spec_t dmi_bis_entry_point_spec =
{
    .code            = "bis-entry-point",
    .name            = "Boot Integrity Services (BIS) entry point",
    .description     = (const char *[]){
        "Structure type 31 is reserved for use by the Boot Integrity Services "
        "(BIS). See the Boot Integrity Services API Specification for details.",
        //
        nullptr
    },
    .type            = DMI_TYPE(BIS_ENTRY_POINT),
    .params = {
        .minimum_version = DMI_VERSION(2, 3, 0),
        .minimum_length  = 0x14,
        .decoded_length  = sizeof(dmi_bis_entry_point_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_bis_entry_point_t, checksum, BYTE),

        // Reserved byte and word the structure carries and nothing reads
        DMI_FIELD_SKIP(sizeof(dmi_byte_t) + sizeof(dmi_word_t)),

        // Real mode entry point is stored as offset followed by segment
        DMI_FIELD(dmi_bis_entry_point_t, entry_point_16.offset,  WORD),
        DMI_FIELD(dmi_bis_entry_point_t, entry_point_16.segment, WORD),
        DMI_FIELD(dmi_bis_entry_point_t, entry_point_32,         DWORD),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_bis_entry_point_t, checksum, INTEGER, {
            .code  = "checksum",
            .name  = "Checksum",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_bis_entry_point_t, entry_point_16, STRUCT, {
            .code  = "entry-point-16",
            .name  = "16-bit entry point",
            .attrs = dmi_bis_real_mode_address_attrs
        }),
        DMI_ATTRIBUTE(dmi_bis_entry_point_t, entry_point_32, ADDRESS, {
            .code  = "entry-point-32",
            .name  = "32-bit entry point"
        }),
        DMI_ATTRIBUTE(dmi_bis_entry_point_t, is_valid, BOOL, {
            .code  = "is-valid",
            .name  = "Valid"
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_bis_entry_point_derive
    }
};
