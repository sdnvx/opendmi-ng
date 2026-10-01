//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_APPLE_SMC_VERSION_INTERNAL_H
#define OPENDMI_ENTITY_APPLE_SMC_VERSION_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/apple/smc-version.h>

/**
 * @internal
 * @brief Take the SMC version text from the raw bytes of the field.
 *
 * @details Text is followed by bytes of zero up to the end of the field,
 * which are left out of the version.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_apple_smc_version_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_APPLE_SMC_VERSION_INTERNAL_H
