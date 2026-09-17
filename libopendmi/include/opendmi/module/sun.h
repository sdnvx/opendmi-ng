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
typedef enum dmi_sun_type
{
    DMI_TYPE_SUN_PROCESSOR_EX      = 132, ///< Sun processor extended information
    DMI_TYPE_SUN_PORT_EX           = 136, ///< Sun port extended information
    DMI_TYPE_SUN_PCIE_ROOT_COMPLEX = 138, ///< Sun PCI-express root complex information
    DMI_TYPE_SUN_MEMORY_ARRAY_EX   = 144, ///< Sun memory array extended information
    DMI_TYPE_SUN_MEMORY_DEVICE_EX  = 145, ///< Sun memory device extended information
} dmi_sun_type_t;

__BEGIN_DECLS

extern __dmi_api const dmi_module_t dmi_sun_module;

__END_DECLS

#endif // !OPENDMI_MODULE_SUN_H
