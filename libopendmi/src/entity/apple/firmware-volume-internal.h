//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_APPLE_FIRMWARE_VOLUME_INTERNAL_H
#define OPENDMI_ENTITY_APPLE_FIRMWARE_VOLUME_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/apple/firmware-volume.h>

/**
 * @internal
 * @brief Names of the firmware features.
 *
 * @details Bits of no known meaning are named after their numbers, the way
 * OpenCore names them, since they are not reserved.
 */
extern const dmi_name_set_t dmi_apple_firmware_feature_names;

/**
 * @internal
 * @brief Names of the extended firmware features.
 *
 * @details Bits of no known meaning are named after their numbers, the way
 * OpenCore names them, since they are not reserved.
 */
extern const dmi_name_set_t dmi_apple_extended_feature_names;
extern const dmi_name_set_t dmi_apple_region_type_names;

bool dmi_apple_firmware_volume_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_APPLE_FIRMWARE_VOLUME_INTERNAL_H
