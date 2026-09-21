//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/lint.h>
#include <opendmi/stream.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/power-controls.h>

static bool dmi_power_controls_decode(dmi_entity_t *entity);
static void dmi_power_controls_lint_bcd(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Fields of the next scheduled power-on, at the offsets the
 * specification gives them, along with the attributes they are decoded into.
 */
static const struct
{
    size_t      offset;
    const char *code;
} dmi_power_controls_bcd_fields[] =
{
    { 0x04, "poweron-month"  },
    { 0x05, "poweron-day"    },
    { 0x06, "poweron-hour"   },
    { 0x07, "poweron-minute" },
    { 0x08, "poweron-second" }
};

const dmi_lint_rule_t dmi_power_controls_bcd_rule =
{
    .code              = "power-controls.bcd",
    .name              = "Fields of the next scheduled power-on hold binary-coded decimals",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_power_controls_lint_bcd
};

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
    .lint_rules      = (const dmi_lint_rule_t *const[]){
        &dmi_power_controls_bcd_rule,
        nullptr
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

//
// Values are decoded into plain numbers, and an invalid pair of digits is
// silently turned into one of them, so the raw data is read instead.
//
static void dmi_power_controls_lint_bcd(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_stream_t stream;

    if (not dmi_stream_initialize(&stream, entity))
        return;

    for (size_t i = 0; i < countof(dmi_power_controls_bcd_fields); i++) {
        size_t offset = dmi_power_controls_bcd_fields[i].offset;
        dmi_byte_t value;

        if (not dmi_stream_read_data_at(&stream, &value, offset, sizeof(value)))
            break;

        // Fields which are not set hold 0xFF, which is no decimal either
        if (value == 0xFF)
            continue;

        if (((value & 0x0F) <= 9) and (((value >> 4) & 0x0F) <= 9))
            continue;

        dmi_lint_issue(lint, entity, dmi_power_controls_bcd_fields[i].code,
                       dmi_lint_entity_offset(lint, entity) + offset,
                       "value 0x%02X is not a binary-coded decimal", (unsigned)value);
    }
}
