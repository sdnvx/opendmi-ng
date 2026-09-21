//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/entity/probe.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>
#include <opendmi/value.h>

#include <opendmi/entity/mgmt-device-threshold-internal.h>

//
// Thresholds are ordered, since crossing a critical one is worse than
// crossing a non-critical one. Templates are left to the rule of their own,
// which describes them better than a broken order does.
//
void dmi_mgmt_device_threshold_lint_order(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_mgmt_device_threshold_t *info =
            dmi_entity_info(entity, DMI_TYPE(MGMT_DEVICE_THRESHOLD));

    if ((info == nullptr) or dmi_mgmt_device_threshold_is_template(info))
        return;

    const struct {
        short       lower;
        short       upper;
        const char *lower_code;
        const char *upper_code;
    } pairs[] = {
        { info->lower_non_recoverable, info->lower_critical,
          "lower-non-recoverable", "lower-critical" },
        { info->lower_critical, info->lower_non_critical,
          "lower-critical", "lower-non-critical" },
        { info->upper_non_critical, info->upper_critical,
          "upper-non-critical", "upper-critical" },
        { info->upper_critical, info->upper_non_recoverable,
          "upper-critical", "upper-non-recoverable" }
    };

    for (size_t i = 0; i < countof(pairs); i++) {
        if (((dmi_word_t)pairs[i].lower == DMI_PROBE_VALUE_UNKNOWN) or
            ((dmi_word_t)pairs[i].upper == DMI_PROBE_VALUE_UNKNOWN))
            continue;

        if (pairs[i].lower <= pairs[i].upper)
            continue;

        dmi_lint_issue(lint, entity, pairs[i].lower_code, dmi_lint_entity_offset(lint, entity),
                       "%s threshold of %d is above the %s one of %d",
                       pairs[i].lower_code, pairs[i].lower, pairs[i].upper_code, pairs[i].upper);
    }
}

//
// Firmware commonly leaves the template of the structure in place, with the
// ordinals of the fields where the thresholds belong.
//
void dmi_mgmt_device_threshold_lint_template(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_mgmt_device_threshold_t *info =
            dmi_entity_info(entity, DMI_TYPE(MGMT_DEVICE_THRESHOLD));

    if ((info == nullptr) or not dmi_mgmt_device_threshold_is_template(info))
        return;

    dmi_lint_issue(lint, entity, nullptr, dmi_lint_entity_offset(lint, entity),
                   "thresholds are the ordinals %d to %d of the fields holding them",
                   info->lower_non_critical, info->upper_non_recoverable);
}
