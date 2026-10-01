//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_APPLE_PLATFORM_FEATURE_H
#define OPENDMI_ENTITY_APPLE_PLATFORM_FEATURE_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_APPLE_PLATFORM_FEATURE_T
#   define DMI_APPLE_PLATFORM_FEATURE_T
    typedef struct dmi_apple_platform_feature dmi_apple_platform_feature_t;
#endif // !DMI_APPLE_PLATFORM_FEATURE_T

/**
 * @brief Features of the platform, which macOS reads.
 */
typedef enum dmi_apple_platform_feature_bit
{
    DMI_APPLE_PLATFORM_FEATURE_SOLDERED_MEMORY = 1, ///< System memory is soldered
    DMI_APPLE_PLATFORM_FEATURE_HEADLESS_GPU    = 2, ///< Headless GPU is present
    DMI_APPLE_PLATFORM_FEATURE_HOST_PM         = 3, ///< Supports host power management
    DMI_APPLE_PLATFORM_FEATURE_POWER_CHIME     = 4  ///< Supports the chime at boot
} dmi_apple_platform_feature_bit_t;

/**
 * @brief Apple platform feature information structure (type 133).
 *
 * Tells the features of the platform, which macOS reads. Laid out as the
 * `AppleSmBios.h` header of OpenCore describes it.
 */
struct dmi_apple_platform_feature
{
    /**
     * @brief Features of the platform, a set of
     * `dmi_apple_platform_feature_bit_t` bits.
     */
    uint64_t features;
};

/**
 * @brief Apple platform feature information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_apple_platform_feature_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_apple_platform_feature_bit_name(dmi_apple_platform_feature_bit_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_APPLE_PLATFORM_FEATURE_H
