//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_MODULE_INTEL_H
#define OPENDMI_MODULE_INTEL_H

#pragma once

#include <opendmi/module.h>

/**
 * @brief Intel structure type identifiers.
 */
typedef enum dmi_intel_type
{
    DMI_TYPE_INTEL_RSD_NETWORK_CARD        = 190, ///< Intel RSD Network card information
    DMI_TYPE_INTEL_RSD_PCIE                = 192, ///< Intel RSD PCIe information
    DMI_TYPE_INTEL_RSD_PROCESSOR_CPUID     = 193, ///< Intel RSD Processor CPUID information
    DMI_TYPE_INTEL_RSD_STORAGE_DEVICE      = 194, ///< Intel RSD Storage device information
    DMI_TYPE_INTEL_RSD_TPM                 = 195, ///< Intel RSD TPM information
    DMI_TYPE_INTEL_RSD_TXT                 = 196, ///< Intel RSD TXT information
    DMI_TYPE_INTEL_RSD_MEMORY_DEVICE       = 197, ///< Intel RSD Memory device extended information
    DMI_TYPE_INTEL_RSD_FPGA                = 198, ///< Intel RSD FPGA information
    DMI_TYPE_INTEL_RSD_CABLED_PCIE         = 199, ///< Intel RSD Cabled PCIe port information
    DMI_TYPE_INTEL_RSD_PHYS_DEVICE_MAPPING = 200  ///< Intel RSD SMBIOS physical device mapping
} dmi_intel_type_t;

__BEGIN_DECLS

extern __dmi_api const dmi_module_t dmi_intel_module;

__END_DECLS

#endif // !OPENDMI_MODULE_INTEL_H
