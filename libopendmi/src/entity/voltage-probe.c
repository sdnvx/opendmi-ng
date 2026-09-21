//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>

#include <opendmi/entity/probe.h>
#include <opendmi/entity/voltage-probe.h>

static void dmi_voltage_probe_lint_range(dmi_lint_t *lint, const dmi_entity_t *entity);

static const dmi_lint_rule_t dmi_voltage_probe_range_rule =
{
    .code              = "voltage-probe.range",
    .name              = "Nominal value of the probe is within its limits",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_voltage_probe_lint_range
};

const dmi_entity_spec_t dmi_voltage_probe_spec =
{
    .code            = "voltage-probe",
    .name            = "Voltage probe",
    .description     = (const char *[]){
        "This describes the attributes for a voltage probe in the system. "
        "Each structure describes a single voltage probe.",
        //
        nullptr
    },
    .type            = DMI_TYPE(VOLTAGE_PROBE),
    .minimum_version = DMI_VERSION(2, 2, 0),
    .minimum_length  = 0x14,
    .decoded_length  = sizeof(dmi_voltage_probe_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_voltage_probe_t, description, STRING, {
            .code    = "description",
            .name    = "Description"
        }),
        DMI_ATTRIBUTE(dmi_voltage_probe_t, location, ENUM, {
            .code    = "location",
            .name    = "Location",
            .unspec  = dmi_value_ptr(DMI_PROBE_LOCATION_UNSPEC),
            .unknown = dmi_value_ptr(DMI_PROBE_LOCATION_UNKNOWN),
            .values  = &dmi_probe_location_names
        }),
        DMI_ATTRIBUTE(dmi_voltage_probe_t, status, ENUM, {
            .code    = "status",
            .name    = "Status",
            .unspec  = dmi_value_ptr(DMI_STATUS_UNSPEC),
            .unknown = dmi_value_ptr(DMI_STATUS_UNKNOWN),
            .values  = &dmi_status_names
        }),
        DMI_ATTRIBUTE(dmi_voltage_probe_t, maximum_value, INTEGER, {
            .code    = "maximum-value",
            .name    = "Maximum value",
            .unit    = DMI_UNIT_MILLIVOLT,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE(dmi_voltage_probe_t, minimum_value, INTEGER, {
            .code    = "minimum-value",
            .name    = "Minimum value",
            .unit    = DMI_UNIT_MILLIVOLT,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE(dmi_voltage_probe_t, resolution, DECIMAL, {
            .code    = "resolution",
            .name    = "Resolution",
            .scale   = 1,
            .unit    = DMI_UNIT_MILLIVOLT,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE(dmi_voltage_probe_t, tolerance, INTEGER, {
            .code    = "tolerance",
            .name    = "Tolerance",
            .unit    = DMI_UNIT_MILLIVOLT,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE(dmi_voltage_probe_t, accuracy, DECIMAL, {
            .code    = "accuracy",
            .name    = "Accuracy",
            .scale   = 2,
            .unit    = DMI_UNIT_PERCENT,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE(dmi_voltage_probe_t, oem_defined, INTEGER, {
            .code    = "oem-defined",
            .name    = "OEM-defined",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_voltage_probe_t, nominal_value, INTEGER, {
            .code    = "nominal-value",
            .name    = "Nominal value",
            .unit    = DMI_UNIT_MILLIVOLT,
            .unknown = dmi_value_ptr(DMI_PROBE_VALUE_UNKNOWN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE_NULL
    },
    .lint_rules      = (const dmi_lint_rule_t *const[]){
        &dmi_voltage_probe_range_rule,
        nullptr
    },

    .handlers = {
        .decode = dmi_probe_decode,
    }
};

static void dmi_voltage_probe_lint_range(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_probe_t *info = dmi_entity_info(entity, DMI_TYPE(VOLTAGE_PROBE));
    if (info == nullptr)
        return;

    size_t offset = dmi_lint_entity_offset(lint, entity);

    bool has_minimum = ((dmi_word_t)info->minimum_value != DMI_PROBE_VALUE_UNKNOWN);
    bool has_maximum = ((dmi_word_t)info->maximum_value != DMI_PROBE_VALUE_UNKNOWN);
    bool has_nominal = ((dmi_word_t)info->nominal_value != DMI_PROBE_VALUE_UNKNOWN);

    if (has_minimum and has_maximum and (info->minimum_value > info->maximum_value)) {
        dmi_lint_issue(lint, entity, "minimum-value", offset,
                       "minimum value of %d is above the maximum of %d",
                       info->minimum_value, info->maximum_value);
        return;
    }

    if (not has_nominal)
        return;

    if (has_minimum and (info->nominal_value < info->minimum_value)) {
        dmi_lint_issue(lint, entity, "nominal-value", offset,
                       "nominal value of %d is below the minimum of %d",
                       info->nominal_value, info->minimum_value);
    }

    if (has_maximum and (info->nominal_value > info->maximum_value)) {
        dmi_lint_issue(lint, entity, "nominal-value", offset,
                       "nominal value of %d is above the maximum of %d",
                       info->nominal_value, info->maximum_value);
    }
}
