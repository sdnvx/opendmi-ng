//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_DIMM_CONFIG_INTERNAL_H
#define OPENDMI_ENTITY_HPE_DIMM_CONFIG_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/dimm-config.h>

/**
 * @internal
 * @brief Names of the health states of an interleave set.
 */
extern const dmi_name_set_t dmi_hpe_interleave_health_names;

/**
 * @internal
 * @brief Derive the size of the region in bytes and whether a passphrase is
 * required.
 *
 * @details Size is held in mebibytes, and any passphrase state other than zero
 * says that a passphrase is required.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_hpe_dimm_config_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_DIMM_CONFIG_INTERNAL_H
