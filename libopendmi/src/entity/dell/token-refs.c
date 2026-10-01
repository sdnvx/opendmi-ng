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

#include <opendmi/entity/dell/token-refs.h>

const dmi_entity_spec_t dmi_dell_token_refs_1_spec =
{
    .type        = DMI_TYPE(dell_token_refs_1),
    .code        = "dell-token-refs-1",
    .name        = "Dell token references, type 1",
    .description = (const char *[]){
        "Lists tokens, most of which the calling interface or the indexed "
        "I/O access of the table defines, which the structures of a table "
        "group by the device they belong to.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x14,
        .decoded_length = sizeof(dmi_dell_token_refs_1_t)
    },

    // Tokens run to the end of the structure, 8 or 9 of them in the known
    // data, and the structure carries no number of them of its own
    .fields = DMI_FIELDS({
        DMI_FIELD_ARRAY(dmi_dell_token_refs_1_t, tokens, token_count,
            .stride = sizeof(dmi_word_t),
            .fields = DMI_FIELDS({
                DMI_FIELD_ELEMENT(dmi_dell_token_refs_1_t, tokens, dmi_word_t),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE_ARRAY(dmi_dell_token_refs_1_t, tokens, token_count, INTEGER, {
            .code  = "tokens",
            .name  = "Tokens",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        {}
    })
};

const dmi_entity_spec_t dmi_dell_token_refs_2_spec =
{
    .type        = DMI_TYPE(dell_token_refs_2),
    .code        = "dell-token-refs-2",
    .name        = "Dell token references, type 2",
    .description = (const char *[]){
        "Lists tokens, most of which the calling interface or the indexed "
        "I/O access of the table defines, following two values whose meaning "
        "is not established.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x13,
        .decoded_length = sizeof(dmi_dell_token_refs_2_t),
        // Structures of the Intel reference code the same length, which
        // some systems carry at the same type, refer to strings
        .signature      = DMI_SIGNATURE({
            .length     = 0x13,
            .no_strings = true
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_dell_token_refs_2_t, unknown_1, dmi_byte_t),
        DMI_FIELD(dmi_dell_token_refs_2_t, unknown_2, dmi_word_t),
        DMI_FIELD_VECTOR(dmi_dell_token_refs_2_t, tokens,
            .fields = DMI_FIELDS({
                DMI_FIELD_ELEMENT(dmi_dell_token_refs_2_t, tokens, dmi_word_t),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_dell_token_refs_2_t, unknown_1, INTEGER, {
            .code  = "unknown-1",
            .name  = "Unknown 1",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_token_refs_2_t, unknown_2, INTEGER, {
            .code  = "unknown-2",
            .name  = "Unknown 2",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE_VECTOR(dmi_dell_token_refs_2_t, tokens, INTEGER, {
            .code  = "tokens",
            .name  = "Tokens",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        {}
    })
};
