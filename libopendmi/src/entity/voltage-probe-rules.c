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

#include <opendmi/entity/voltage-probe-internal.h>

void dmi_voltage_probe_lint_range(dmi_lint_t *lint, const dmi_entity_t *entity)
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
