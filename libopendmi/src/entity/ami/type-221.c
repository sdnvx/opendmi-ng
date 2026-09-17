//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>

#include <opendmi/internal.h>
#include <opendmi/module/ami.h>
#include <opendmi/entity/ami/type-221.h>

static bool dmi_ami_type_221_decode(dmi_entity_t *entity);
static void dmi_ami_type_221_cleanup(dmi_entity_t *entity);

const dmi_entity_spec_t dmi_ami_type_221_spec =
{
    .type           = DMI_TYPE(AMI_221),
    .code           = "ami-221",
    .name           = "AMI type 221",
    .minimum_length = 0x05,
    .decoded_length = sizeof(dmi_ami_type_221_t),
    .attributes     = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE_ARRAY(dmi_ami_type_221_t, items, item_count, STRUCT, {
            .code  = "items",
            .name  = "Items",
            .attrs = (const dmi_attribute_t[]){
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
                DMI_ATTRIBUTE_NULL
            }
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_ami_type_221_decode,
        .cleanup = dmi_ami_type_221_cleanup
    }
};

static bool dmi_ami_type_221_decode(dmi_entity_t *entity)
{
    dmi_ami_type_221_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(AMI_221));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    if (not dmi_stream_decode(stream, dmi_byte_t, &info->item_count))
        return false;

    if (!info->item_count)
        return true;

    info->items = dmi_alloc_array(entity->context, sizeof(dmi_ami_type_221_item_t), info->item_count);
    if (info->items == nullptr)
        return false;

    for (size_t i = 0; i < info->item_count; i++) {
        dmi_ami_type_221_item_t *item = &info->items[i];

        bool status =
            dmi_stream_decode_str(stream, &item->name) and
            dmi_stream_decode_str(stream, &item->string) and
            dmi_stream_decode(stream, dmi_byte_t, &item->unknown_1) and
            dmi_stream_decode(stream, dmi_word_t, &item->unknown_2) and
            dmi_stream_decode(stream, dmi_word_t, &item->unknown_3);

        if (!status)
            return false;
    }

    return true;
}

static void dmi_ami_type_221_cleanup(dmi_entity_t *entity)
{
    dmi_ami_type_221_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(AMI_221));
    if (info == nullptr)
        return;

    dmi_free(info->items);
}
