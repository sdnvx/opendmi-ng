//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_TEMPERATURE_PROBE_INTERNAL_H
#define OPENDMI_ENTITY_TEMPERATURE_PROBE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/temperature-probe.h>

/**
 * @internal
 * @brief Check that the minimum value of a temperature probe is not above
 * its maximum, and that the nominal value lies between them.
 *
 * @details Values of 0x8000 stand for "unknown" and are not checked.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_temperature_probe_lint_range(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_TEMPERATURE_PROBE_INTERNAL_H
