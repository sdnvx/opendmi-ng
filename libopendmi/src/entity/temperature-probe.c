//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdio.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/entity/probe-internal.h>
#include <opendmi/lint.h>

#include <opendmi/entity/temperature-probe-internal.h>

const dmi_entity_spec_t dmi_temperature_probe_spec =
{
    .code            = "temperature-probe",
    .name            = "Temperature probe",
    .description     = (const char *[]){
        "This structure describes the attributes for a temperature probe in "
        "the system. Each structure describes a single temperature probe. ",
        //
        nullptr
    },
    .type            = DMI_TYPE(TEMPERATURE_PROBE),
    .params = {
        .minimum_version = DMI_VERSION(2, 2, 0),
        .minimum_length  = 0x14,
        .decoded_length  = sizeof(dmi_probe_t)
    },

    .fields = dmi_probe_fields(dmi_temperature_probe_t),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_temperature_probe_t, description, STRING, {
            .code    = "description",
            .name    = "Description"
        }),
        DMI_ATTRIBUTE(dmi_temperature_probe_t, location, ENUM, {
            .code    = "location",
            .name    = "Location",
            .unspec  = dmi_value_ptr(DMI_PROBE_LOCATION_UNSPEC),
            .unknown = dmi_value_ptr(DMI_PROBE_LOCATION_UNKNOWN),
            .values  = &dmi_probe_location_names
        }),
        DMI_ATTRIBUTE(dmi_temperature_probe_t, status, ENUM, {
            .code    = "status",
            .name    = "Status",
            .unspec  = dmi_value_ptr(DMI_STATUS_UNSPEC),
            .unknown = dmi_value_ptr(DMI_STATUS_UNKNOWN),
            .values  = &dmi_status_names
        }),
        DMI_ATTRIBUTE(dmi_temperature_probe_t, maximum_value, DECIMAL, {
            .code    = "maximum-value",
            .name    = "Maximum value",
            .scale   = 1,
            .unit    = DMI_UNIT_CELSIUS,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE(dmi_temperature_probe_t, minimum_value, DECIMAL, {
            .code    = "minimum-value",
            .name    = "Minimum value",
            .scale   = 1,
            .unit    = DMI_UNIT_CELSIUS,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE(dmi_temperature_probe_t, resolution, DECIMAL, {
            .code    = "resolution",
            .name    = "Resolution",
            .scale   = 3,
            .unit    = DMI_UNIT_CELSIUS,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE(dmi_temperature_probe_t, tolerance, DECIMAL, {
            .code    = "tolerance",
            .name    = "Tolerance",
            .scale   = 1,
            .unit    = DMI_UNIT_CELSIUS,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE(dmi_temperature_probe_t, accuracy, DECIMAL, {
            .code    = "accuracy",
            .name    = "Accuracy",
            .scale   = 2,
            .unit    = DMI_UNIT_PERCENT,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE(dmi_temperature_probe_t, oem_defined, INTEGER, {
            .code    = "oem-defined",
            .name    = "OEM-defined",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_temperature_probe_t, nominal_value, DECIMAL, {
            .code    = "nominal-value",
            .name    = "Nominal value",
            .scale   = 1,
            .unit    = DMI_UNIT_CELSIUS,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("temperature-probe.range", dmi_temperature_probe_lint_range, {
            .name              = "Nominal value of the probe is within its limits",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    })
};
