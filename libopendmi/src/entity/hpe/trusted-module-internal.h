//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_TRUSTED_MODULE_INTERNAL_H
#define OPENDMI_ENTITY_HPE_TRUSTED_MODULE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/trusted-module.h>

/**
 * @internal
 * @brief Names of the presence states of the trusted module.
 */
extern const dmi_name_set_t dmi_hpe_tm_presence_names;

/**
 * @internal
 * @brief Names of the reasons the trusted module is disabled for.
 */
extern const dmi_name_set_t dmi_hpe_tm_disable_reason_names;

/**
 * @internal
 * @brief Names of the types of the trusted module.
 */
extern const dmi_name_set_t dmi_hpe_tm_type_names;

/**
 * @internal
 * @brief Names of the ways the trusted module is mounted.
 */
extern const dmi_name_set_t dmi_hpe_tm_mounting_names;

/**
 * @internal
 * @brief Names of the FIPS certification states of the trusted module.
 */
extern const dmi_name_set_t dmi_hpe_tm_fips_names;

/**
 * @internal
 * @brief Names of the chips of the trusted module.
 */
extern const dmi_name_set_t dmi_hpe_tm_chip_names;

/**
 * @internal
 * @brief Names of the error conditions of the trusted module.
 */
extern const dmi_name_set_t dmi_hpe_tm_error_names;

#endif // !OPENDMI_ENTITY_HPE_TRUSTED_MODULE_INTERNAL_H
