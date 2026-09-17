//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-cabled-pcie.h>
#include <opendmi/entity/intel/rsd-fpga.h>
#include <opendmi/entity/intel/rsd-memory-device.h>
#include <opendmi/entity/intel/rsd-network-card.h>
#include <opendmi/entity/intel/rsd-pcie.h>
#include <opendmi/entity/intel/rsd-phys-device-mapping.h>
#include <opendmi/entity/intel/rsd-processor-cpuid.h>
#include <opendmi/entity/intel/rsd-storage-device.h>
#include <opendmi/entity/intel/rsd-tpm.h>
#include <opendmi/entity/intel/rsd-txt.h>

/**
 * @brief Intel extension module.
 */
const dmi_module_t dmi_intel_module =
{
    .code     = "intel",
    .name     = "Intel extensions",
    .entities = (const dmi_entity_spec_t *[]){
        &dmi_intel_rsd_cabled_pcie_spec,
        &dmi_intel_rsd_fpga_spec,
        &dmi_intel_rsd_memory_device_spec,
        &dmi_intel_rsd_network_card_spec,
        &dmi_intel_rsd_pcie_spec,
        &dmi_intel_rsd_phys_device_mapping_spec,
        &dmi_intel_rsd_processor_cpuid_spec,
        &dmi_intel_rsd_storage_device_spec,
        &dmi_intel_rsd_tpm_spec,
        &dmi_intel_rsd_txt_spec,
        nullptr
    }
};
