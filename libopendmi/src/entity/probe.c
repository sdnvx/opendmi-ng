//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/stream.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/probe.h>

const dmi_name_set_t dmi_probe_location_names =
{
    .code  = "probe-locations",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_PROBE_LOCATION_UNSPEC),
        DMI_NAME_OTHER(DMI_PROBE_LOCATION_OTHER),
        DMI_NAME_UNKNOWN(DMI_PROBE_LOCATION_UNKNOWN),
        {
            .id   = DMI_PROBE_LOCATION_PROCESSOR,
            .code = "processor",
            .name = "Processor"
        },
        {
            .id   = DMI_PROBE_LOCATION_DISK,
            .code = "disk",
            .name = "Disk"
        },
        {
            .id   = DMI_PROBE_LOCATION_PERIPHERAL_BAY,
            .code = "peripheral-bay",
            .name = "Peripheral bay"
        },
        {
            .id   = DMI_PROBE_LOCATION_SYSTEM_MGMT_MODULE,
            .code = "system-mgmt-module",
            .name = "System management module"
        },
        {
            .id   = DMI_PROBE_LOCATION_MOTHERBOARD,
            .code = "motherboard",
            .name = "Motherboard"
        },
        {
            .id   = DMI_PROBE_LOCATION_MEMORY_MODULE,
            .code = "memory-module",
            .name = "Memory module"
        },
        {
            .id   = DMI_PROBE_LOCATION_PROCESSOR_MODULE,
            .code = "processor-module",
            .name = "Processor module"
        },
        {
            .id   = DMI_PROBE_LOCATION_POWER_UNIT,
            .code = "power-unit",
            .name = "Power unit"
        },
        {
            .id   = DMI_PROBE_LOCATION_ADDIN_CARD,
            .code = "addin-card",
            .name = "Add-in card"
        },
        {
            .id   = DMI_PROBE_LOCATION_FRONT_PANEL_BOARD,
            .code = "front-panel-board",
            .name = "Front panel board"
        },
        {
            .id   = DMI_PROBE_LOCATION_BACK_PANEL_BOARD,
            .code = "back-panel-board",
            .name = "Back panel board"
        },
        {
            .id   = DMI_PROBE_LOCATION_POWER_SYSTEM_BOARD,
            .code = "power-system-board",
            .name = "Power system board"
        },
        {
            .id   = DMI_PROBE_LOCATION_DRIVE_BACK_PLANE,
            .code = "drive-back-plane",
            .name = "Drive back plane"
        },
        DMI_NAME_NULL
    }
};

const char *dmi_probe_location_name(dmi_probe_location_t value)
{
    return dmi_name_lookup(&dmi_probe_location_names, (int)value);
}

bool dmi_probe_decode(dmi_entity_t *entity)
{
    dmi_probe_t *info = nullptr;

    info = dmi_cast(info, entity->info);
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    dmi_probe_details_t details;

    bool status =
        dmi_stream_decode_str(stream, &info->description) and
        dmi_stream_decode(stream, dmi_byte_t, &details.__value) and
        dmi_stream_decode(stream, dmi_word_t, &info->maximum_value) and
        dmi_stream_decode(stream, dmi_word_t, &info->minimum_value) and
        dmi_stream_decode(stream, dmi_word_t, &info->resolution) and
        dmi_stream_decode(stream, dmi_word_t, &info->tolerance) and
        dmi_stream_decode(stream, dmi_word_t, &info->accuracy) and
        dmi_stream_decode(stream, dmi_dword_t, &info->oem_defined);
    if (not status)
        return false;

    info->location      = details.location;
    info->status        = details.status;
    info->nominal_value = SHRT_MIN;

    // Nominal value
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);
    if (not dmi_stream_decode(stream, dmi_word_t, &info->nominal_value))
        return dmi_entity_incomplete(entity);

    return true;
}
