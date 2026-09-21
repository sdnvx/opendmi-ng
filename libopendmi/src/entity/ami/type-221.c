//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/ami.h>

#include <opendmi/entity/ami/type-221-internal.h>

const dmi_entity_spec_t dmi_ami_type_221_spec =
{
    .type           = DMI_TYPE(AMI_221),
    .code           = "ami-221",
    .name           = "AMI type 221",
    .params = {
        .minimum_length = 0x05,
        .decoded_length = sizeof(dmi_ami_type_221_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_ARRAY(dmi_ami_type_221_t, items, item_count,
            .count_length = sizeof(dmi_byte_t),
            .fields       = DMI_FIELDS({
                DMI_FIELD_STRING(dmi_ami_type_221_item_t, name),
                DMI_FIELD_STRING(dmi_ami_type_221_item_t, string),
                DMI_FIELD(dmi_ami_type_221_item_t, unknown_1, dmi_byte_t),
                DMI_FIELD(dmi_ami_type_221_item_t, unknown_2, dmi_word_t),
                DMI_FIELD(dmi_ami_type_221_item_t, unknown_3, dmi_word_t),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE_ARRAY(dmi_ami_type_221_t, items, item_count, STRUCT, {
            .code  = "items",
            .name  = "Items",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_ami_type_221_item_t, name, STRING, {
                    .code = "name",
                    .name = "Name"
                }),
                DMI_ATTRIBUTE(dmi_ami_type_221_item_t, string, STRING, {
                    .code = "string-value",
                    .name = "String value"
                }),
                DMI_ATTRIBUTE(dmi_ami_type_221_item_t, unknown_1, INTEGER, {
                    .code   = "unknown-1",
                    .name   = "Unknown 1",
                    .unspec = dmi_value_ptr((uint8_t)UINT8_MAX),
                    .flags  = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_ATTRIBUTE(dmi_ami_type_221_item_t, unknown_2, INTEGER, {
                    .code   = "unknown-2",
                    .name   = "Unknown 2",
                    .unspec = dmi_value_ptr((uint16_t)UINT16_MAX),
                    .flags  = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_ATTRIBUTE(dmi_ami_type_221_item_t, unknown_3, INTEGER, {
                    .code   = "unknown-3",
                    .name   = "Unknown 3",
                    .unspec = dmi_value_ptr((uint16_t)UINT16_MAX),
                    .flags  = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .cleanup = dmi_ami_type_221_cleanup
    }
};
