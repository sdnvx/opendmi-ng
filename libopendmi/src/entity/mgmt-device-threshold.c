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

const dmi_entity_spec_t dmi_mgmt_device_threshold_spec =
{
    .code            = "mgmt-device-threshold",
    .name            = "Management device threshold data",
    .description     = (const char *[]){
        "The information in this structure defines threshold information for "
        "a component (probe or cooling-unit) contained within a Management "
        "Device.",
        //
        nullptr
    },
    .type            = DMI_TYPE(MGMT_DEVICE_THRESHOLD),
    .params = {
        .minimum_version = DMI_VERSION(2, 3, 0),
        .minimum_length  = 0x10,
        .decoded_length  = sizeof(dmi_mgmt_device_threshold_t)
    },

    .fields = DMI_FIELDS({
        // Units are unknown until components are linked
        DMI_FIELD_PRESET(dmi_mgmt_device_threshold_t, component_type, DMI_TYPE_INVALID),

        DMI_FIELD(dmi_mgmt_device_threshold_t, lower_non_critical,    WORD),
        DMI_FIELD(dmi_mgmt_device_threshold_t, upper_non_critical,    WORD),
        DMI_FIELD(dmi_mgmt_device_threshold_t, lower_critical,        WORD),
        DMI_FIELD(dmi_mgmt_device_threshold_t, upper_critical,        WORD),
        DMI_FIELD(dmi_mgmt_device_threshold_t, lower_non_recoverable, WORD),
        DMI_FIELD(dmi_mgmt_device_threshold_t, upper_non_recoverable, WORD),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE_VARIANT(dmi_mgmt_device_threshold_t, component_type, {
            .code     = "lower-non-critical",
            .name     = "Lower non-critical",
            .variants = dmi_threshold_variants(lower_non_critical)
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_mgmt_device_threshold_t, component_type, {
            .code     = "upper-non-critical",
            .name     = "Upper non-critical",
            .variants = dmi_threshold_variants(upper_non_critical)
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_mgmt_device_threshold_t, component_type, {
            .code     = "lower-critical",
            .name     = "Lower critical",
            .variants = dmi_threshold_variants(lower_critical)
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_mgmt_device_threshold_t, component_type, {
            .code     = "upper-critical",
            .name     = "Upper critical",
            .variants = dmi_threshold_variants(upper_critical)
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_mgmt_device_threshold_t, component_type, {
            .code     = "lower-non-recoverable",
            .name     = "Lower non-recoverable",
            .variants = dmi_threshold_variants(lower_non_recoverable)
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_mgmt_device_threshold_t, component_type, {
            .code     = "upper-non-recoverable",
            .name     = "Upper non-recoverable",
            .variants = dmi_threshold_variants(upper_non_recoverable)
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("mgmt-device-threshold.order", dmi_mgmt_device_threshold_lint_order, {
            .name              = "Thresholds grow from non-recoverable to non-critical and back",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        DMI_LINT_RULE("mgmt-device-threshold.template", dmi_mgmt_device_threshold_lint_template, {
            .name              = "Thresholds hold values rather than the ordinals of their fields",
            .severity          = DMI_LINT_SEVERITY_NOTE,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    })
};
