//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_MICROCODE_H
#define OPENDMI_ENTITY_HPE_MICROCODE_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/utils/datetime.h>

typedef struct dmi_hpe_microcode       dmi_hpe_microcode_t;
typedef struct dmi_hpe_microcode_patch dmi_hpe_microcode_patch_t;

/**
 * @brief CPU microcode patch the firmware carries.
 */
struct dmi_hpe_microcode_patch
{
    /**
     * @brief Patch ID, which is the microcode revision.
     */
    uint32_t patch_id;

    /**
     * @brief Release date of the patch, as the microcode header gives it:
     * the month, the day and the year in binary-coded decimal.
     */
    uint32_t raw_date;

    /**
     * @brief Release date of the patch.
     */
    dmi_date_t date;

    /**
     * @brief Processor signature (CPUID leaf 1 EAX) the patch applies to,
     * as the structure holds it. AMD platforms leave the base family out.
     */
    uint32_t cpuid;

    /**
     * @brief Processor signature the patch applies to, with the base family
     * of 15 put back on AMD platforms, the way dmidecode does, which is valid
     * for the families of 15 onwards.
     */
    uint32_t signature;
};

/**
 * @brief HP/HPE CPU microcode patch support information (type 199).
 *
 * Lists the CPU microcode patches the firmware carries.
 */
struct dmi_hpe_microcode
{
    /**
     * @brief Number of patches.
     */
    size_t patch_count;

    /**
     * @brief Patches.
     */
    dmi_hpe_microcode_patch_t *patches;
};

/**
 * @brief HP/HPE CPU microcode patch support information entity
 * specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_microcode_spec;

#endif // !OPENDMI_ENTITY_HPE_MICROCODE_H
