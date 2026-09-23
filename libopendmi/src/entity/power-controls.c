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
#include <opendmi/reader.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/power-controls-internal.h>

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
    .params = {
        .minimum_version = DMI_VERSION(2, 2, 0),
        .minimum_length  = 0x09,
        .decoded_length  = sizeof(dmi_power_controls_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_BCD(dmi_power_controls_t, poweron_month,  dmi_byte_t),
        DMI_FIELD_BCD(dmi_power_controls_t, poweron_day,    dmi_byte_t),
        DMI_FIELD_BCD(dmi_power_controls_t, poweron_hour,   dmi_byte_t),
        DMI_FIELD_BCD(dmi_power_controls_t, poweron_minute, dmi_byte_t),
        DMI_FIELD_BCD(dmi_power_controls_t, poweron_second, dmi_byte_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_power_controls_t, poweron_month, INTEGER, {
            .code = "poweron-month",
            .name = "Next power-on month",
            .minimum = dmi_value_ptr((unsigned short)1),
            .maximum = dmi_value_ptr((unsigned short)12),
        }),
        DMI_ATTRIBUTE(dmi_power_controls_t, poweron_day, INTEGER, {
            .code = "poweron-day",
            .name = "Next power-on day of month",
            .minimum = dmi_value_ptr((unsigned short)1),
            .maximum = dmi_value_ptr((unsigned short)31),
        }),
        DMI_ATTRIBUTE(dmi_power_controls_t, poweron_hour, INTEGER, {
            .code = "poweron-hour",
            .name = "Next power-on hour",
            .minimum = dmi_value_ptr((unsigned short)0),
            .maximum = dmi_value_ptr((unsigned short)23),
        }),
        DMI_ATTRIBUTE(dmi_power_controls_t, poweron_minute, INTEGER, {
            .code = "poweron-minute",
            .name = "Next power-on minute",
            .minimum = dmi_value_ptr((unsigned short)0),
            .maximum = dmi_value_ptr((unsigned short)59),
        }),
        DMI_ATTRIBUTE(dmi_power_controls_t, poweron_second, INTEGER, {
            .code = "poweron-second",
            .name = "Next power-on second",
            .minimum = dmi_value_ptr((unsigned short)0),
            .maximum = dmi_value_ptr((unsigned short)59),
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("power-controls.bcd", dmi_power_controls_lint_bcd, {
            .name              = "Fields of the next scheduled power-on hold binary-coded decimals",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    })
};
