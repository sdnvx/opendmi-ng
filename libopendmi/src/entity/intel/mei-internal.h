//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_MEI_INTERNAL_H
#define OPENDMI_ENTITY_INTEL_MEI_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/intel/mei.h>

/**
 * @internal
 * @brief Names of the states of the Management Engine.
 */
extern const dmi_name_set_t dmi_intel_me_state_names;

/**
 * @internal
 * @brief Names of the error codes, as coreboot names them.
 */
extern const dmi_name_set_t dmi_intel_me_error_names;

/**
 * @internal
 * @brief Names of the operation modes of the Management Engine.
 */
extern const dmi_name_set_t dmi_intel_me_mode_names;

/**
 * @internal
 * @brief Names of the SKUs of the Management Engine firmware.
 */
extern const dmi_name_set_t dmi_intel_me_sku_names;

/**
 * @internal
 * @brief Derive the presence of the interfaces and the state of the Management
 * Engine.
 *
 * @details Interface is reported unless its registers are all zeroes, and is
 * present unless they have all bits set. State, mode, error code and SKU of
 * the firmware are read from the status registers of the first interface,
 * which is the one of the host, and are left unspecified if it is not present.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_intel_mei_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_INTEL_MEI_INTERNAL_H
