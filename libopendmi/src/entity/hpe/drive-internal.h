//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_DRIVE_INTERNAL_H
#define OPENDMI_ENTITY_HPE_DRIVE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/drive.h>

/**
 * @internal
 * @brief Names of the drive types.
 */
extern const dmi_name_set_t dmi_hpe_drive_type_names;

/**
 * @internal
 * @brief Names of the drive form factors.
 */
extern const dmi_name_set_t dmi_hpe_drive_form_factor_names;

/**
 * @internal
 * @brief Names of the drive health states.
 */
extern const dmi_name_set_t dmi_hpe_drive_health_names;

#endif // !OPENDMI_ENTITY_HPE_DRIVE_INTERNAL_H
