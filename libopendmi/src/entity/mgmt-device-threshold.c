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

#include <opendmi/entity/mgmt-device-threshold.h>

static bool dmi_mgmt_device_threshold_decode(dmi_entity_t *entity);

//
// Threshold values are in units of the component using them, and are shown as
// they are stored, if the component is unknown
//
#define dmi_threshold_variant(__type, __member, __attr_type, ...)        \
    DMI_VARIANT(DMI_TYPE(__type), dmi_mgmt_device_threshold_t, __member, \
                __attr_type, {                                           \
                    .unknown = dmi_value_ptr((short)SHRT_MIN),           \
                    .flags   = DMI_ATTRIBUTE_FLAG_SIGNED,                \
                    __VA_ARGS__                                          \
                })

#define dmi_threshold_variants(__member)                                                                   \
    (const dmi_attribute_variant_t[]){                                                                     \
        dmi_threshold_variant(VOLTAGE_PROBE, __member, INTEGER, .unit = DMI_UNIT_MILLIVOLT),               \
        dmi_threshold_variant(TEMPERATURE_PROBE, __member, DECIMAL, .scale = 1, .unit = DMI_UNIT_CELSIUS), \
        dmi_threshold_variant(CURRENT_PROBE, __member, INTEGER, .unit = DMI_UNIT_MILLIAMPERE),             \
        dmi_threshold_variant(COOLING_DEVICE, __member, INTEGER, .unit = DMI_UNIT_REVOLUTION),             \
        DMI_VARIANT_DEFAULT(dmi_mgmt_device_threshold_t, __member, INTEGER, {                              \
            .unknown = dmi_value_ptr((short)SHRT_MIN),                                                     \
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED                                                           \
        }),                                                                                                \
        DMI_VARIANT_NULL                                                                                   \
    }

static void dmi_mgmt_device_threshold_lint_order(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_mgmt_device_threshold_lint_template(dmi_lint_t *lint, const dmi_entity_t *entity);

static const dmi_lint_rule_t dmi_mgmt_device_threshold_order_rule =
{
    .code              = "mgmt-device-threshold.order",
    .name              = "Thresholds grow from non-recoverable to non-critical and back",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_mgmt_device_threshold_lint_order
};

static const dmi_lint_rule_t dmi_mgmt_device_threshold_template_rule =
{
    .code              = "mgmt-device-threshold.template",
    .name              = "Thresholds hold values rather than the ordinals of their fields",
    .severity          = DMI_LINT_SEVERITY_NOTE,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_mgmt_device_threshold_lint_template
};

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
    .minimum_version = DMI_VERSION(2, 3, 0),
    .minimum_length  = 0x10,
    .decoded_length  = sizeof(dmi_mgmt_device_threshold_t),
    .attributes      = (const dmi_attribute_t[]){
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
        DMI_ATTRIBUTE_NULL
    },
    .lint_rules      = (const dmi_lint_rule_t *const[]){
        &dmi_mgmt_device_threshold_order_rule,
        &dmi_mgmt_device_threshold_template_rule,
        nullptr
    },

    .handlers = {
        .decode = dmi_mgmt_device_threshold_decode
    }
};

static bool dmi_mgmt_device_threshold_decode(dmi_entity_t *entity)
{
    dmi_mgmt_device_threshold_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_DEVICE_THRESHOLD));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    // Units are unknown until components are linked
    info->component_type = DMI_TYPE_INVALID;

    return
        dmi_stream_decode(stream, dmi_word_t, &info->lower_non_critical) and
        dmi_stream_decode(stream, dmi_word_t, &info->upper_non_critical) and
        dmi_stream_decode(stream, dmi_word_t, &info->lower_critical) and
        dmi_stream_decode(stream, dmi_word_t, &info->upper_critical) and
        dmi_stream_decode(stream, dmi_word_t, &info->lower_non_recoverable) and
        dmi_stream_decode(stream, dmi_word_t, &info->upper_non_recoverable);
}

void dmi_mgmt_device_threshold_set_component(dmi_entity_t *entity, dmi_type_t type)
{
    dmi_mgmt_device_threshold_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_DEVICE_THRESHOLD));
    if ((info == nullptr) or info->is_ambiguous)
        return;

    if (info->component_type == DMI_TYPE_INVALID) {
        info->component_type = type;
    } else if (info->component_type != type) {
        info->component_type = DMI_TYPE_INVALID;
        info->is_ambiguous   = true;
    }
}

//
// Thresholds of a structure which firmware has left as a template, with the
// ordinals of the fields in place of the values.
//
static bool dmi_mgmt_device_threshold_is_template(const dmi_mgmt_device_threshold_t *info)
{
    const short values[] =
    {
        info->lower_non_critical,    info->upper_non_critical,
        info->lower_critical,        info->upper_critical,
        info->lower_non_recoverable, info->upper_non_recoverable
    };

    // Fields of the structure follow each other, so the ordinals differ by
    // one, and start where the first field is
    for (size_t i = 1; i < countof(values); i++) {
        if (values[i] != values[i - 1] + 1)
            return false;
    }

    return true;
}

//
// Thresholds are ordered, since crossing a critical one is worse than
// crossing a non-critical one. Templates are left to the rule of their own,
// which describes them better than a broken order does.
//
static void dmi_mgmt_device_threshold_lint_order(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_mgmt_device_threshold_t *info =
            dmi_entity_info(entity, DMI_TYPE(MGMT_DEVICE_THRESHOLD));

    if ((info == nullptr) or dmi_mgmt_device_threshold_is_template(info))
        return;

    const struct
    {
        short       lower;
        short       upper;
        const char *lower_code;
        const char *upper_code;
    } pairs[] =
    {
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
static void dmi_mgmt_device_threshold_lint_template(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_mgmt_device_threshold_t *info =
            dmi_entity_info(entity, DMI_TYPE(MGMT_DEVICE_THRESHOLD));

    if ((info == nullptr) or not dmi_mgmt_device_threshold_is_template(info))
        return;

    dmi_lint_issue(lint, entity, nullptr, dmi_lint_entity_offset(lint, entity),
                   "thresholds are the ordinals %d to %d of the fields holding them",
                   info->lower_non_critical, info->upper_non_recoverable);
}
