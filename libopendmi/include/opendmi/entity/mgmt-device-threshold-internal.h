//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MGMT_DEVICE_THRESHOLD_INTERNAL_H
#define OPENDMI_ENTITY_MGMT_DEVICE_THRESHOLD_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/mgmt-device-threshold.h>

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
    DMI_VARIANTS({                                                            \
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

// Operation handlers, see mgmt-device-threshold-handlers.c
bool dmi_mgmt_device_threshold_is_template(const dmi_mgmt_device_threshold_t *info);

// Checks the lint rules of the specification perform, see mgmt-device-threshold-rules.c
void dmi_mgmt_device_threshold_lint_order(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_mgmt_device_threshold_lint_template(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MGMT_DEVICE_THRESHOLD_INTERNAL_H
