//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_APPLE_SMC_VERSION_H
#define OPENDMI_ENTITY_APPLE_SMC_VERSION_H

#pragma once

#include <opendmi/entity.h>

#define DMI_APPLE_SMC_VERSION_SIZE 16

#ifndef DMI_APPLE_SMC_VERSION_T
#   define DMI_APPLE_SMC_VERSION_T
    typedef struct dmi_apple_smc_version dmi_apple_smc_version_t;
#endif // !DMI_APPLE_SMC_VERSION_T

/**
 * @brief Apple SMC version information structure (type 134).
 *
 * Tells the version of the firmware of the System Management Controller
 * (SMC). Laid out as the `AppleSmBios.h` header of OpenCore describes it.
 */
struct dmi_apple_smc_version
{
    /**
     * @brief Version of the SMC firmware, as the structure holds it, of
     * `DMI_APPLE_SMC_VERSION_SIZE` bytes.
     */
    dmi_binary_t version_raw;

    /**
     * @brief Version of the SMC firmware as text, e.g. `1.59f2`, which the
     * bytes of zero end. @c nullptr if the structure holds no text.
     */
    const char *version;

    /**
     * @brief Buffer holding the text of `version`.
     */
    char version_buffer[DMI_APPLE_SMC_VERSION_SIZE + 1];
};

/**
 * @brief Apple SMC version information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_apple_smc_version_spec;

#endif // !OPENDMI_ENTITY_APPLE_SMC_VERSION_H
