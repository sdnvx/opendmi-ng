//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_VOLTAGE_PROBE_INTERNAL_H
#define OPENDMI_ENTITY_VOLTAGE_PROBE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/voltage-probe.h>

/**
 * @internal
 * @brief Check that the minimum value of a probe is not above the maximum
 * one, and that the nominal value is within them.
 *
 * @details Values which are unknown are left out of the check.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_voltage_probe_lint_range(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_VOLTAGE_PROBE_INTERNAL_H
