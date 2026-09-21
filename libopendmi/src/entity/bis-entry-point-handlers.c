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

const dmi_attribute_t dmi_bis_real_mode_address_attrs[] =
{
    DMI_ATTRIBUTE(dmi_bis_real_mode_address_t, segment, INTEGER, {
        .code  = "segment",
        .name  = "Segment",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_bis_real_mode_address_t, offset, INTEGER, {
        .code  = "offset",
        .name  = "Offset",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    {}
};

//
// Checksum covers the whole structure, as read by the stream.
//
bool dmi_bis_entry_point_derive(dmi_entity_t *entity)
{
    dmi_bis_entry_point_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(BIS_ENTRY_POINT));
    if (info == nullptr)
        return false;

    const dmi_stream_t *stream = dmi_entity_stream(entity);

    info->is_valid = dmi_checksum_test(stream->data, entity->body_length);

    return true;
}
