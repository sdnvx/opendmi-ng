//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_MODULE_INTEL_RSD_H
#define OPENDMI_MODULE_INTEL_RSD_H

#pragma once

#include <opendmi/module.h>

/**
 * @brief Intel Rack Scale Design (RSD) structure type identifiers.
 */
typedef enum dmi_intel_rsd_type_id
{
    DMI_TYPE_ID_INTEL_RSD_NETWORK_CARD        = 190, ///< Intel RSD Network card information
    DMI_TYPE_ID_INTEL_RSD_PCIE                = 192, ///< Intel RSD PCIe information
    DMI_TYPE_ID_INTEL_RSD_PROCESSOR_CPUID     = 193, ///< Intel RSD Processor CPUID information
    DMI_TYPE_ID_INTEL_RSD_STORAGE_DEVICE      = 194, ///< Intel RSD Storage device information
    DMI_TYPE_ID_INTEL_RSD_TPM                 = 195, ///< Intel RSD TPM information
    DMI_TYPE_ID_INTEL_RSD_TXT                 = 196, ///< Intel RSD TXT information
    DMI_TYPE_ID_INTEL_RSD_MEMORY_DEVICE       = 197, ///< Intel RSD Memory device extended information
    DMI_TYPE_ID_INTEL_RSD_FPGA                = 198, ///< Intel RSD FPGA information
    DMI_TYPE_ID_INTEL_RSD_CABLED_PCIE         = 199, ///< Intel RSD Cabled PCIe port information
    DMI_TYPE_ID_INTEL_RSD_PHYS_DEVICE_MAPPING = 200  ///< Intel RSD SMBIOS physical device mapping
} dmi_intel_rsd_type_id_t;

__BEGIN_DECLS

/** @brief Intel RSD Network card information */
extern __dmi_api const dmi_type_t dmi_type_intel_rsd_network_card;

/** @brief Intel RSD PCIe information */
extern __dmi_api const dmi_type_t dmi_type_intel_rsd_pcie;

/** @brief Intel RSD Processor CPUID information */
extern __dmi_api const dmi_type_t dmi_type_intel_rsd_processor_cpuid;

/** @brief Intel RSD Storage device information */
extern __dmi_api const dmi_type_t dmi_type_intel_rsd_storage_device;

/** @brief Intel RSD TPM information */
extern __dmi_api const dmi_type_t dmi_type_intel_rsd_tpm;

/** @brief Intel RSD TXT information */
extern __dmi_api const dmi_type_t dmi_type_intel_rsd_txt;

/** @brief Intel RSD Memory device extended information */
extern __dmi_api const dmi_type_t dmi_type_intel_rsd_memory_device;

/** @brief Intel RSD FPGA information */
extern __dmi_api const dmi_type_t dmi_type_intel_rsd_fpga;

/** @brief Intel RSD Cabled PCIe port information */
extern __dmi_api const dmi_type_t dmi_type_intel_rsd_cabled_pcie;

/** @brief Intel RSD SMBIOS physical device mapping */
extern __dmi_api const dmi_type_t dmi_type_intel_rsd_phys_device_mapping;

extern __dmi_api const dmi_module_t dmi_intel_rsd_module;

__END_DECLS

#endif // !OPENDMI_MODULE_INTEL_RSD_H
