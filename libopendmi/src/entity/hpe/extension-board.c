//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include <opendmi/entity/hpe/extension-board-internal.h>

//
// The board type at offset 0x04 selects the layout of the rest of the
// structure, which each specification tells by its signature
//
const dmi_entity_spec_t dmi_hpe_riser_spec =
{
    .type        = DMI_TYPE(hpe_riser),
    .code        = "hpe-riser",
    .name        = "HP/HPE extension board inventory record",
    .description = (const char *[]){
        "Describes a PCIe riser installed in the server.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x09,
        .decoded_length = sizeof(dmi_hpe_riser_t),
        .signature      = DMI_SIGNATURE({
            .offset = 0x04,
            .bytes  = "\x00",
            .size   = 1
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_riser_t, board_type,   dmi_byte_t),
        DMI_FIELD(dmi_hpe_riser_t, position,     dmi_byte_t),
        DMI_FIELD(dmi_hpe_riser_t, riser_id,     dmi_byte_t),
        // Version of the CPLD takes bits 0 to 6, and bit 7 tells a "B."
        // release
        DMI_FIELD_BITS(dmi_hpe_riser_t, cpld_version,      7),
        DMI_FIELD_BITS(dmi_hpe_riser_t, is_cpld_b_release, 1),
        DMI_FIELD_PAD(dmi_byte_t),
        DMI_FIELD_STRING(dmi_hpe_riser_t, name),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_riser_t, board_type, ENUM, {
            .code   = "board-type",
            .name   = "Board type",
            .values = &dmi_hpe_board_type_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_riser_t, position, ENUM, {
            .code   = "position",
            .name   = "Riser position",
            .values = &dmi_hpe_riser_position_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_riser_t, riser_id, INTEGER, {
            .code = "riser-id",
            .name = "Riser ID"
        }),
        DMI_ATTRIBUTE(dmi_hpe_riser_t, cpld_version, INTEGER, {
            .code   = "cpld-version",
            .name   = "CPLD version",
            .unspec = dmi_value_ptr((uint8_t)0),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_riser_t, is_cpld_b_release, BOOL, {
            .code = "is-cpld-b-release",
            .name = "CPLD B. release"
        }),
        DMI_ATTRIBUTE(dmi_hpe_riser_t, name, STRING, {
            .code = "name",
            .name = "Riser name"
        }),
        {}
    })
};

const dmi_entity_spec_t dmi_hpe_mhs_riser_spec =
{
    .type        = DMI_TYPE(hpe_mhs_riser),
    .code        = "hpe-mhs-riser",
    .name        = "HP/HPE extension board inventory record",
    .description = (const char *[]){
        "Describes a PCIe riser installed in a server of an MHS platform.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x0B,
        .decoded_length = sizeof(dmi_hpe_mhs_riser_t),
        .signature      = DMI_SIGNATURE({
            .offset = 0x04,
            .bytes  = "\x01",
            .size   = 1
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_mhs_riser_t, board_type,     dmi_byte_t),
        DMI_FIELD(dmi_hpe_mhs_riser_t, riser_id,       dmi_byte_t),
        DMI_FIELD(dmi_hpe_mhs_riser_t, firmware_major, dmi_byte_t),
        DMI_FIELD(dmi_hpe_mhs_riser_t, firmware_minor, dmi_byte_t),

        DMI_FIELD_BITS(dmi_hpe_mhs_riser_t, is_downgradable, 1),
        DMI_FIELD_PAD(dmi_byte_t),

        DMI_FIELD_STRING(dmi_hpe_mhs_riser_t, name),
        DMI_FIELD_ARRAY(dmi_hpe_mhs_riser_t, slot_ids, slot_count,
            .count_length = sizeof(dmi_byte_t),
            .count_member = dmi_member(dmi_hpe_mhs_riser_t, slot_total),
            .fields       = DMI_FIELDS({
                DMI_FIELD_ELEMENT(dmi_hpe_mhs_riser_t, slot_ids, dmi_byte_t),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_mhs_riser_t, board_type, ENUM, {
            .code   = "board-type",
            .name   = "Board type",
            .values = &dmi_hpe_board_type_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_mhs_riser_t, riser_id, INTEGER, {
            .code = "riser-id",
            .name = "Riser ID"
        }),
        DMI_ATTRIBUTE(dmi_hpe_mhs_riser_t, firmware_major, INTEGER, {
            .code   = "firmware-major",
            .name   = "Firmware major version",
            .unspec = dmi_value_ptr((uint8_t)0),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_mhs_riser_t, firmware_minor, INTEGER, {
            .code  = "firmware-minor",
            .name  = "Firmware minor version",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_mhs_riser_t, is_downgradable, BOOL, {
            .code = "is-downgradable",
            .name = "Downgradable"
        }),
        DMI_ATTRIBUTE(dmi_hpe_mhs_riser_t, name, STRING, {
            .code = "name",
            .name = "Riser name"
        }),
        DMI_ATTRIBUTE(dmi_hpe_mhs_riser_t, slot_total, INTEGER, {
            .code = "slot-total",
            .name = "Slot count"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_hpe_mhs_riser_t, slot_ids, slot_count, INTEGER, {
            .code  = "slot-ids",
            .name  = "Slot IDs",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        {}
    })
};

const dmi_name_set_t dmi_hpe_board_type_names =
{
    .code  = "hpe-board-type",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_BOARD_TYPE_RISER,
            .code = "riser",
            .name = "PCIe riser"
        },
        {
            .id   = DMI_HPE_BOARD_TYPE_MHS_RISER,
            .code = "mhs-riser",
            .name = "PCIe riser of an MHS platform"
        },
        {}
    })
};

const char *dmi_hpe_board_type_name(dmi_hpe_board_type_t value)
{
    return dmi_name_lookup(&dmi_hpe_board_type_names, (int)value);
}

const dmi_name_set_t dmi_hpe_riser_position_names =
{
    .code  = "hpe-riser-position",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_RISER_POSITION_PRIMARY,
            .code = "primary",
            .name = "Primary"
        },
        {
            .id   = DMI_HPE_RISER_POSITION_SECONDARY,
            .code = "secondary",
            .name = "Secondary"
        },
        {
            .id   = DMI_HPE_RISER_POSITION_TERTIARY,
            .code = "tertiary",
            .name = "Tertiary"
        },
        {
            .id   = DMI_HPE_RISER_POSITION_QUATERNARY,
            .code = "quaternary",
            .name = "Quaternary"
        },
        {
            .id   = DMI_HPE_RISER_POSITION_FRONT,
            .code = "front",
            .name = "Front"
        },
        {}
    })
};

const char *dmi_hpe_riser_position_name(dmi_hpe_riser_position_t value)
{
    return dmi_name_lookup(&dmi_hpe_riser_position_names, (int)value);
}
