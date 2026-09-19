//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SUN_PROCESSOR_EX_H
#define OPENDMI_ENTITY_SUN_PROCESSOR_EX_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_sun_processor_ex dmi_sun_processor_ex_t;

/**
 * @brief Sun processor extended information (type 132).
 */
struct dmi_sun_processor_ex
{
    /**
     * @brief Handle of the extended processor information structure.
     */
    dmi_handle_t processor_handle;

    /**
     * @brief FRU indicator.
     */
    uint8_t fru;

    /**
     * @brief Number of initial APIC IDs, as specified in the structure.
     */
    uint8_t apic_id_total;

    /**
     * @brief Number of completely present initial APIC IDs.
     */
    size_t apic_id_count;

    /**
     * @brief Initial APIC IDs of the processor strands.
     */
    uint16_t *apic_ids;
};

/**
 * @brief Sun processor extended information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_sun_processor_ex_spec;

#endif // !OPENDMI_ENTITY_SUN_PROCESSOR_EX_H
