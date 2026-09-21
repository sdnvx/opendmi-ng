//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/entity/probe-internal.h>
#include <opendmi/lint.h>

#include <opendmi/entity/current-probe-internal.h>

const dmi_entity_spec_t dmi_current_probe_spec =
{
    .code            = "current-probe",
    .name            = "Electrical current probe",
    .description     = (const char *[]){
        "This structure describes the attributes for an electrical current "
        "probe in the system. Each structure describes a single electrical "
        "current probe.",
        //
        nullptr
    },
    .type            = DMI_TYPE(CURRENT_PROBE),
    .params = {
        .minimum_version = DMI_VERSION(2, 2, 0),
        .minimum_length  = 0x14,
        .decoded_length  = sizeof(dmi_current_probe_t)
    },

    .fields = dmi_probe_fields(dmi_current_probe_t),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_current_probe_t, description, STRING, {
            .code    = "description",
            .name    = "Description"
        }),
        DMI_ATTRIBUTE(dmi_current_probe_t, location, ENUM, {
            .code    = "location",
            .name    = "Location",
            .unspec  = dmi_value_ptr(DMI_PROBE_LOCATION_UNSPEC),
            .unknown = dmi_value_ptr(DMI_PROBE_LOCATION_UNKNOWN),
            .values  = &dmi_probe_location_names
        }),
        DMI_ATTRIBUTE(dmi_current_probe_t, status, ENUM, {
            .code    = "status",
            .name    = "Status",
            .unspec  = dmi_value_ptr(DMI_STATUS_UNSPEC),
            .unknown = dmi_value_ptr(DMI_STATUS_UNKNOWN),
            .values  = &dmi_status_names
        }),
        DMI_ATTRIBUTE(dmi_current_probe_t, maximum_value, INTEGER, {
            .code    = "maximum-value",
            .name    = "Maximum value",
            .unit    = DMI_UNIT_MILLIAMPERE,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE(dmi_current_probe_t, minimum_value, INTEGER, {
            .code    = "minimum-value",
            .name    = "Minimum value",
            .unit    = DMI_UNIT_MILLIAMPERE,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE(dmi_current_probe_t, resolution, DECIMAL, {
            .code    = "resolution",
            .name    = "Resolution",
            .scale   = 1,
            .unit    = DMI_UNIT_MILLIAMPERE,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE(dmi_current_probe_t, tolerance, INTEGER, {
            .code    = "tolerance",
            .name    = "Tolerance",
            .unit    = DMI_UNIT_MILLIAMPERE,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE(dmi_current_probe_t, accuracy, DECIMAL, {
            .code    = "accuracy",
            .name    = "Accuracy",
            .scale   = 2,
            .unit    = DMI_UNIT_PERCENT,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE(dmi_current_probe_t, oem_defined, INTEGER, {
            .code    = "oem-defined",
            .name    = "OEM-defined",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_current_probe_t, nominal_value, INTEGER, {
            .code    = "nominal-value",
            .name    = "Nominal value",
            .unit    = DMI_UNIT_MILLIAMPERE,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("current-probe.range", dmi_current_probe_lint_range, {
            .name              = "Nominal value of the probe is within its limits",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    })
};
