//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SYSTEM_CONFIG_INTERNAL_H
#define OPENDMI_ENTITY_SYSTEM_CONFIG_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/system-config.h>

/**
 * @internal
 * @brief Fill in the list of the configuration options.
 *
 * @details Strings of the structure are its values, so the array points at
 * the ones the structure carries rather than at anything read from the data.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_system_config_opts_derive(dmi_entity_t *entity);

/**
 * @internal
 * @brief Free the list of the configuration options of a decoded structure.
 *
 * @param[in,out] entity Structure being cleaned up.
 */
void dmi_system_config_opts_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_SYSTEM_CONFIG_INTERNAL_H
