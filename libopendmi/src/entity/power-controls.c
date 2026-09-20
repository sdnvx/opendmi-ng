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

#include <opendmi/entity/power-controls.h>

static bool dmi_power_controls_decode(dmi_entity_t *entity);

const dmi_entity_spec_t dmi_power_controls_spec =
{
    .code            = "power-controls",
    .name            = "System power controls",
    .description     = (const char *[]){
        "This structure describes the attributes for controlling the main "
        "power supply to the system.",
        //
        "Software that interprets this structure uses the month, day, hour, "
        "minute, and second values to determine the number of seconds until "
        "the next power-on of the system. The presence of this structure "
        "implies that a timed power-on facility is available for the system.",
        //
        nullptr
    },
    .type            = DMI_TYPE(POWER_CONTROLS),
    .minimum_version = DMI_VERSION(2, 2, 0),
    .minimum_length  = 0x09,
    .decoded_length  = sizeof(dmi_power_controls_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_power_controls_t, poweron_month, INTEGER, {
            .code = "poweron-month",
            .name = "Next power-on month"
        }),
        DMI_ATTRIBUTE(dmi_power_controls_t, poweron_day, INTEGER, {
            .code = "poweron-day",
            .name = "Next power-on day of month"
        }),
        DMI_ATTRIBUTE(dmi_power_controls_t, poweron_hour, INTEGER, {
            .code = "poweron-hour",
            .name = "Next power-on hour"
        }),
        DMI_ATTRIBUTE(dmi_power_controls_t, poweron_minute, INTEGER, {
            .code = "poweron-minute",
            .name = "Next power-on minute"
        }),
        DMI_ATTRIBUTE(dmi_power_controls_t, poweron_second, INTEGER, {
            .code = "poweron-second",
            .name = "Next power-on second"
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_power_controls_decode
    }
};

static bool dmi_power_controls_decode(dmi_entity_t *entity)
{
    dmi_power_controls_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(POWER_CONTROLS));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    return
        dmi_stream_decode_bcd(stream, dmi_byte_t, &info->poweron_month) and
        dmi_stream_decode_bcd(stream, dmi_byte_t, &info->poweron_day) and
        dmi_stream_decode_bcd(stream, dmi_byte_t, &info->poweron_hour) and
        dmi_stream_decode_bcd(stream, dmi_byte_t, &info->poweron_minute) and
        dmi_stream_decode_bcd(stream, dmi_byte_t, &info->poweron_second);
}
