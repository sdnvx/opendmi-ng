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

/**
 * @internal
 * @brief Check whether the thresholds of a structure are a template.
 *
 * @details Firmware may leave the structure as a template, with the ordinals
 * of the fields in place of the threshold values.
 *
 * @param[in] info Decoded structure.
 *
 * @return `true` if the thresholds are a template, `false` otherwise.
 */
bool dmi_mgmt_device_threshold_is_template(const dmi_mgmt_device_threshold_t *info);

/**
 * @internal
 * @brief Check that each threshold is not beyond the more severe one.
 *
 * @details Thresholds are ordered, since crossing a critical one is worse
 * than crossing a non-critical one. Templates are left to the rule of their
 * own, which describes them better than a broken order does.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_mgmt_device_threshold_lint_order(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the thresholds of a structure are not a template.
 *
 * @details Firmware commonly leaves the template of the structure in place,
 * with the ordinals of the fields where the thresholds belong.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_mgmt_device_threshold_lint_template(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MGMT_DEVICE_THRESHOLD_INTERNAL_H
