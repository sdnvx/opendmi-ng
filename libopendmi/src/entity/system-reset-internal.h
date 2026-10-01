//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SYSTEM_RESET_INTERNAL_H
#define OPENDMI_ENTITY_SYSTEM_RESET_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/system-reset.h>

/**
 * @internal
 * @brief Names of the boot options taken on a watchdog reset or on reaching
 * the reset limit.
 */
extern const dmi_name_set_t dmi_boot_option_names;

/**
 * @internal
 * @brief Check that the number of resets does not exceed the reset limit.
 *
 * @details Counters holding `0xFFFF` are unknown, and are not checked.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_system_reset_lint_limit(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_SYSTEM_RESET_INTERNAL_H
