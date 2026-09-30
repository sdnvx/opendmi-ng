//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_MODULE_SUN_H
#define OPENDMI_MODULE_SUN_H

#pragma once

#include <opendmi/module.h>

/**
 * @brief Sun structure type identifiers.
 */
typedef enum dmi_sun_type_id
{
    DMI_TYPE_ID_SUN_PROCESSOR_EX      = 132, ///< Sun processor extended information
    DMI_TYPE_ID_SUN_PORT_EX           = 136, ///< Sun port extended information
    DMI_TYPE_ID_SUN_PCIE_ROOT_COMPLEX = 138, ///< Sun PCI-express root complex information
    DMI_TYPE_ID_SUN_MEMORY_ARRAY_EX   = 144, ///< Sun memory array extended information
    DMI_TYPE_ID_SUN_MEMORY_DEVICE_EX  = 145  ///< Sun memory device extended information
} dmi_sun_type_id_t;

__BEGIN_DECLS

/** @brief Sun processor extended information */
extern __dmi_api const dmi_type_t dmi_type_sun_processor_ex;

/** @brief Sun port extended information */
extern __dmi_api const dmi_type_t dmi_type_sun_port_ex;

/** @brief Sun PCI-express root complex information */
extern __dmi_api const dmi_type_t dmi_type_sun_pcie_root_complex;

/** @brief Sun memory array extended information */
extern __dmi_api const dmi_type_t dmi_type_sun_memory_array_ex;

/** @brief Sun memory device extended information */
extern __dmi_api const dmi_type_t dmi_type_sun_memory_device_ex;

extern __dmi_api const dmi_module_t dmi_sun_module;

__END_DECLS

#endif // !OPENDMI_MODULE_SUN_H
