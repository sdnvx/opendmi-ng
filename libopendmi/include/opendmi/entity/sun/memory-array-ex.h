//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SUN_MEMORY_ARRAY_EX_H
#define OPENDMI_ENTITY_SUN_MEMORY_ARRAY_EX_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_sun_memory_array_ex dmi_sun_memory_array_ex_t;

/**
 * @brief Sun memory array extended information (type 144).
 */
struct dmi_sun_memory_array_ex
{
    /**
     * @brief Handle of the extended physical memory array structure.
     */
    dmi_handle_t memory_array_handle;

    /**
     * @brief Handle of the parent component, e.g. the processor containing
     * the memory controller.
     */
    dmi_handle_t component_handle;

    /**
     * @brief PCI bus number.
     */
    uint8_t bus_number;

    /**
     * @brief PCI device number.
     */
    uint8_t device_number;

    /**
     * @brief PCI function number.
     */
    uint8_t function_number;
};

/**
 * @brief Sun memory array extended information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_sun_memory_array_ex_spec;

#endif // !OPENDMI_ENTITY_SUN_MEMORY_ARRAY_EX_H
