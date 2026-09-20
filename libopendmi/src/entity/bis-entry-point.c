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

#include <opendmi/entity/bis-entry-point.h>

static bool dmi_bis_entry_point_decode(dmi_entity_t *entity);

static const dmi_attribute_t dmi_bis_real_mode_address_attrs[] =
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
    DMI_ATTRIBUTE_NULL
};

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
    .minimum_version = DMI_VERSION(2, 3, 0),
    .minimum_length  = 0x14,
    .decoded_length  = sizeof(dmi_bis_entry_point_t),
    .attributes      = (const dmi_attribute_t[]){
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
        DMI_ATTRIBUTE_NULL
    },
    .handlers        = {
        .decode = dmi_bis_entry_point_decode
    }
};

static bool dmi_bis_entry_point_decode(dmi_entity_t *entity)
{
    dmi_bis_entry_point_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(BIS_ENTRY_POINT));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    // Real mode entry point is stored as offset followed by segment
    bool status =
        dmi_stream_decode(stream, dmi_byte_t, &info->checksum) and
        dmi_stream_skip(stream, sizeof(dmi_byte_t) + sizeof(dmi_word_t)) and
        dmi_stream_decode(stream, dmi_word_t, &info->entry_point_16.offset) and
        dmi_stream_decode(stream, dmi_word_t, &info->entry_point_16.segment) and
        dmi_stream_decode(stream, dmi_dword_t, &info->entry_point_32);
    if (not status)
        return false;

    // Checksum covers the whole structure, as read by the stream
    info->is_valid = dmi_checksum_test(stream->data, entity->body_length);

    return true;
}
