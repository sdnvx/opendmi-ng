//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>
#include <opendmi/entity/dell/revisions.h>

static bool dmi_dell_revisions_decode(dmi_entity_t *entity);

const dmi_entity_spec_t dmi_dell_revisions_spec =
{
    .type            = DMI_TYPE(DELL_REVISIONS),
    .code            = "dell-revisions",
    .name            = "Dell revisions and IDs",
    .minimum_version = DMI_VERSION(2, 2, 0),
    .minimum_length  = 0x08,
    .decoded_length  = sizeof(dmi_dell_revisions_t),
    .attributes      = (const dmi_attribute_t[]){
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
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_dell_revisions_decode
    }
};

static bool dmi_dell_revisions_decode(dmi_entity_t *entity)
{
    dmi_dell_revisions_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(DELL_REVISIONS));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    uint8_t  impl_major = 0;
    uint8_t  impl_minor = 0;
    uint8_t  system_id  = 0;
    uint16_t system_id_ex = 0;

    bool status =
        dmi_stream_decode(stream, dmi_byte_t, &impl_major) and
        dmi_stream_decode(stream, dmi_byte_t, &impl_minor) and
        dmi_stream_decode(stream, dmi_byte_t, &system_id) and
        dmi_stream_decode(stream, dmi_byte_t, &info->hardware_revision);

    if (not status)
        return false;

    info->impl_version = dmi_version(impl_major, impl_minor, 0);
    info->system_id    = system_id;

    // Extended system ID
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);
    if (not dmi_stream_decode(stream, dmi_word_t, &system_id_ex))
        return dmi_entity_incomplete(entity);

    if (system_id == 0xFEu)
        info->system_id = system_id_ex;

    // Manufacture and first power-on dates
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    status =
        dmi_stream_decode_str(stream, &info->manufacture_date) and
        dmi_stream_decode_str(stream, &info->first_poweron_date);
    if (not status)
        return dmi_entity_incomplete(entity);

    return true;
}
