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

#include <opendmi/entity/group-assoc.h>

static bool dmi_group_assoc_decode(dmi_entity_t *entity);
static bool dmi_group_assoc_link(dmi_entity_t *entity);
static void dmi_group_assoc_cleanup(dmi_entity_t *entity);

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
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x05,
    .decoded_length  = sizeof(dmi_group_assoc_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_group_assoc_t, group_name, STRING, {
            .code  = "group-name",
            .name  = "Group name"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_group_assoc_t, items, item_count, STRUCT, {
            .code  = "items",
            .name  = "Items",
            .attrs = (const dmi_attribute_t[]){
                DMI_ATTRIBUTE(dmi_group_assoc_item_t, type, INTEGER, {
                    .code  = "type",
                    .name  = "Type",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_ATTRIBUTE(dmi_group_assoc_item_t, handle, HANDLE, {
                    .code = "handle",
                    .name = "Handle"
                }),
                DMI_ATTRIBUTE_NULL
            }
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_group_assoc_decode,
        .link    = dmi_group_assoc_link,
        .cleanup = dmi_group_assoc_cleanup
    }
};

static bool dmi_group_assoc_decode(dmi_entity_t *entity)
{
    dmi_group_assoc_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(GROUP_ASSOC));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    if (not dmi_stream_decode_str(stream, &info->group_name))
        return false;

    // Number of items is determined by the structure length
    size_t remaining = dmi_stream_remaining(stream);
    size_t item_size = sizeof(dmi_byte_t) + sizeof(dmi_handle_t);

    info->item_count = remaining / item_size;

    info->items = dmi_alloc_array(entity->context, sizeof(dmi_group_assoc_item_t), info->item_count);
    if (info->items == nullptr)
        return false;

    bool status = true;
    for (size_t i = 0; i < info->item_count; i++) {
        dmi_group_assoc_item_t *item = &info->items[i];

        status =
            dmi_stream_decode(stream, dmi_byte_t, &item->type) and
            dmi_stream_decode(stream, dmi_handle_t, &item->handle);

        if (not status)
            break;
    }

    if (not status)
        return false;

    // Remaining bytes do not form a complete item
    if (not dmi_stream_is_done(stream))
        return dmi_entity_incomplete(entity);

    return true;
}

static bool dmi_group_assoc_link(dmi_entity_t *entity)
{
    dmi_group_assoc_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(GROUP_ASSOC));
    if (info == nullptr)
        return false;

    dmi_registry_t *registry = entity->context->state.registry;

    for (size_t i = 0; i < info->item_count; i++) {
        dmi_group_assoc_item_t *item = &info->items[i];
        item->entity = dmi_registry_get(registry, item->handle, item->type, false);
    }

    return true;
}

static void dmi_group_assoc_cleanup(dmi_entity_t *entity)
{
    dmi_group_assoc_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(GROUP_ASSOC));
    if (info == nullptr)
        return;

    dmi_free(info->items);
}
