//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DMI_ENTITY_INTEL_RSD_PROCESSOR_CPUID_H
#define DMI_ENTITY_INTEL_RSD_PROCESSOR_CPUID_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_intel_rsd_processor_cpuid dmi_intel_rsd_processor_cpuid_t;
typedef struct dmi_intel_rsd_cpuid_leaf      dmi_intel_rsd_cpuid_leaf_t;

/**
 * @brief Intel RSD processor CPUID information subtypes.
 */
typedef enum dmi_intel_rsd_cpuid_subtype
{
    DMI_INTEL_RSD_CPUID_SUBTYPE_BASIC    = 0x01, ///< Leaves 00h to 10h
    DMI_INTEL_RSD_CPUID_SUBTYPE_EXTENDED = 0x02  ///< Leaves 14h to 80000008h
} dmi_intel_rsd_cpuid_subtype_t;

/**
 * @brief CPUID leaf, as returned by the CPUID instruction.
 */
struct dmi_intel_rsd_cpuid_leaf
{
    /**
     * @brief Leaf number (input value of EAX).
     */
    uint32_t leaf;

    /**
     * @brief Set if the subleaf number is specified.
     */
    bool has_subleaf;

    /**
     * @brief Subleaf number (input value of ECX).
     */
    uint32_t subleaf;

    uint32_t eax; ///< EAX register value
    uint32_t ebx; ///< EBX register value
    uint32_t ecx; ///< ECX register value
    uint32_t edx; ///< EDX register value
};

/**
 * @brief Intel RSD processor CPUID information (type 193).
 */
struct dmi_intel_rsd_processor_cpuid
{
    /**
     * @brief Physically labelled socket or board position of the processor.
     */
    const char *socket_designation;

    /**
     * @brief Subtype, which defines the set of leaves.
     */
    dmi_intel_rsd_cpuid_subtype_t subtype;

    /**
     * @brief Number of completely present leaves.
     */
    size_t leaf_count;

    /**
     * @brief CPUID leaves, @c nullptr if the subtype is unknown.
     */
    dmi_intel_rsd_cpuid_leaf_t *leaves;

    /**
     * @brief Set if the subtype is unknown, so that the data is shown as
     * stored.
     */
    bool is_raw;

    /**
     * @brief CPUID data, as stored.
     */
    dmi_binary_t data;
};

/**
 * @brief Intel RSD processor CPUID information subtypes.
 */
extern __dmi_api const dmi_name_set_t dmi_intel_rsd_cpuid_subtype_names;

/**
 * @brief Intel RSD processor CPUID information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_rsd_processor_cpuid_spec;

#endif // !DMI_ENTITY_INTEL_RSD_PROCESSOR_CPUID_H
