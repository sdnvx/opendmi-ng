//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_APPLE_PLATFORM_FEATURE_INTERNAL_H
#define OPENDMI_ENTITY_APPLE_PLATFORM_FEATURE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/apple/platform-feature.h>

/**
 * @internal
 * @brief Names of the platform feature bits.
 *
 * @details Bits of no known meaning are named after their numbers, the way
 * OpenCore names them, since they are not reserved.
 */
extern const dmi_name_set_t dmi_apple_platform_feature_bit_names;

#endif // !OPENDMI_ENTITY_APPLE_PLATFORM_FEATURE_INTERNAL_H
