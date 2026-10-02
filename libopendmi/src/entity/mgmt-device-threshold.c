//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <limits.h>

#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/entity/probe.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>
#include <opendmi/value.h>

#include "mgmt-device-threshold-internal.h"

/**
 * @internal
 * @brief Variant of a threshold value for the type of the component using it.
 *
 * @details Threshold values are in units of the component using them, and are
 * shown as they are stored, if the component is unknown.
 *
 * @param __type      Type of the component, as named by `DMI_TYPE_ID()`.
 * @param __member    Member holding the threshold value.
 * @param __attr_type Type of the attribute showing the value.
 * @param ...         Further options of the attribute, e.g. its unit.
 */
#define dmi_threshold_variant(__type, __member, __attr_type, ...)           \
    DMI_VARIANT(DMI_TYPE_ID(__type), dmi_mgmt_device_threshold_t, __member, \
                __attr_type, {                                              \
                    .unknown = dmi_value_ptr((short)SHRT_MIN),              \
                    .flags   = DMI_ATTRIBUTE_FLAG_SIGNED,                   \
                    __VA_ARGS__                                             \
                })

/**
 * @internal
 * @brief Variants of a threshold value for each type of the component.
 *
 * @details Values of an unknown component are shown as signed integers.
 */
#define dmi_threshold_variants(__member)                                                                   \
    DMI_VARIANTS({                                                                                         \
        dmi_threshold_variant(VOLTAGE_PROBE, __member, INTEGER, .unit = DMI_UNIT_MILLIVOLT),               \
        dmi_threshold_variant(TEMPERATURE_PROBE, __member, DECIMAL, .scale = 1, .unit = DMI_UNIT_CELSIUS), \
        dmi_threshold_variant(CURRENT_PROBE, __member, INTEGER, .unit = DMI_UNIT_MILLIAMPERE),             \
        dmi_threshold_variant(COOLING_DEVICE, __member, INTEGER, .unit = DMI_UNIT_REVOLUTION),             \
        DMI_VARIANT_DEFAULT(dmi_mgmt_device_threshold_t, __member, INTEGER, {                              \
            .unknown = dmi_value_ptr((short)SHRT_MIN),                                                     \
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED                                                           \
        }),                                                                                                \
        {}                                                                                                 \
    })

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
    .type            = DMI_TYPE(mgmt_device_threshold),
    .params = {
        .minimum_version = DMI_VERSION(2, 3, 0),
        .minimum_length  = 0x10,
        .decoded_length  = sizeof(dmi_mgmt_device_threshold_t)
    },

    .fields = DMI_FIELDS({
        // Units are unknown until components are linked
        DMI_FIELD_PRESET(dmi_mgmt_device_threshold_t, component_type, DMI_TYPE_ID_INVALID),

        DMI_FIELD(dmi_mgmt_device_threshold_t, lower_non_critical,    dmi_word_t),
        DMI_FIELD(dmi_mgmt_device_threshold_t, upper_non_critical,    dmi_word_t),
        DMI_FIELD(dmi_mgmt_device_threshold_t, lower_critical,        dmi_word_t),
        DMI_FIELD(dmi_mgmt_device_threshold_t, upper_critical,        dmi_word_t),
        DMI_FIELD(dmi_mgmt_device_threshold_t, lower_non_recoverable, dmi_word_t),
        DMI_FIELD(dmi_mgmt_device_threshold_t, upper_non_recoverable, dmi_word_t),
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
            .reader_severity   = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        DMI_LINT_RULE("mgmt-device-threshold.template", dmi_mgmt_device_threshold_lint_template, {
            .name              = "Thresholds hold values rather than the ordinals of their fields",
            .reader_severity   = DMI_LINT_SEVERITY_NOTE,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    })
};
