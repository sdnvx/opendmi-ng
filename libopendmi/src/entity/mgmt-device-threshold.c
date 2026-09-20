//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
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

