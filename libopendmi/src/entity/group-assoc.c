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

#include <opendmi/entity/group-assoc-internal.h>

const dmi_entity_spec_t dmi_group_assoc_spec =
{
    .code            = "group-associations",
    .name            = "Group associations",
    .description     = (const char *[]){
        "The Group Associations structure is provided for OEMs who want to "
        "specify the arrangement or hierarchy of certain components (including "
        "other Group Associations) within the system. For example, you can use "
        " the Group Associations structure to indicate that two CPUs share a "
        "common external cache system.",
        //
        nullptr
    },
    .type            = DMI_TYPE(GROUP_ASSOC),
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x05,
        .decoded_length  = sizeof(dmi_group_assoc_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_STRING(dmi_group_assoc_t, group_name),

        // Items run to the end of the structure, which carries no number of
        // them of its own
        DMI_FIELD_ARRAY(dmi_group_assoc_t, items, item_count,
            .stride = sizeof(dmi_byte_t) + sizeof(dmi_handle_t),
            .fields = DMI_FIELDS({
                DMI_FIELD(dmi_group_assoc_item_t, type,   dmi_byte_t),
                DMI_FIELD(dmi_group_assoc_item_t, handle, dmi_word_t),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_group_assoc_t, group_name, STRING, {
            .code  = "group-name",
            .name  = "Group name"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_group_assoc_t, items, item_count, STRUCT, {
            .code  = "items",
            .name  = "Items",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_group_assoc_item_t, type, INTEGER, {
                    .code  = "type",
                    .name  = "Type",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_ATTRIBUTE(dmi_group_assoc_item_t, handle, HANDLE, {
                    .code = "handle",
                    .name = "Handle"
                }),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .link    = dmi_group_assoc_link,
        .cleanup = dmi_group_assoc_cleanup
    }
};
