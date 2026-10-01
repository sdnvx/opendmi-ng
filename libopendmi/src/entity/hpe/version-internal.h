//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_VERSION_INTERNAL_H
#define OPENDMI_ENTITY_HPE_VERSION_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/version.h>

/**
 * @internal
 * @brief Names of the firmware types.
 */
extern const dmi_name_set_t dmi_hpe_firmware_type_names;

/**
 * @internal
 * @brief Derive the version string from the version data.
 *
 * @details Version data is formatted according to its data format and the
 * platform generation. Version is left unset if the format is unknown or the
 * string does not fit the buffer.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_hpe_version_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_VERSION_INTERNAL_H
