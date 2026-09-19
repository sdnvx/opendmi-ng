//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SUN_PCIE_ROOT_COMPLEX_H
#define OPENDMI_ENTITY_SUN_PCIE_ROOT_COMPLEX_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_sun_pcie_root_complex dmi_sun_pcie_root_complex_t;

/**
 * @brief Sun PCI-express root complex information (type 138).
 */
struct dmi_sun_pcie_root_complex
{
    /**
     * @brief Handle of the baseboard containing the root complex or root
     * port.
     */
    dmi_handle_t baseboard_handle;

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
 * @brief Sun PCI-express root complex information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_sun_pcie_root_complex_spec;

#endif // !OPENDMI_ENTITY_SUN_PCIE_ROOT_COMPLEX_H
