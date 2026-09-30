//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/intel-rsd.h>

#include <opendmi/entity/intel-rsd/cabled-pcie.h>
#include <opendmi/entity/intel-rsd/fpga.h>
#include <opendmi/entity/intel-rsd/memory-device.h>
#include <opendmi/entity/intel-rsd/network-card.h>
#include <opendmi/entity/intel-rsd/pcie.h>
#include <opendmi/entity/intel-rsd/phys-device-mapping.h>
#include <opendmi/entity/intel-rsd/processor-cpuid.h>
#include <opendmi/entity/intel-rsd/storage-device.h>
#include <opendmi/entity/intel-rsd/tpm.h>
#include <opendmi/entity/intel-rsd/txt.h>

// Structure types of the module
const dmi_type_t dmi_type_intel_rsd_network_card        = { .id = DMI_TYPE_ID(INTEL_RSD_NETWORK_CARD)        };
const dmi_type_t dmi_type_intel_rsd_pcie                = { .id = DMI_TYPE_ID(INTEL_RSD_PCIE)                };
const dmi_type_t dmi_type_intel_rsd_processor_cpuid     = { .id = DMI_TYPE_ID(INTEL_RSD_PROCESSOR_CPUID)     };
const dmi_type_t dmi_type_intel_rsd_storage_device      = { .id = DMI_TYPE_ID(INTEL_RSD_STORAGE_DEVICE)      };
const dmi_type_t dmi_type_intel_rsd_tpm                 = { .id = DMI_TYPE_ID(INTEL_RSD_TPM)                 };
const dmi_type_t dmi_type_intel_rsd_txt                 = { .id = DMI_TYPE_ID(INTEL_RSD_TXT)                 };
const dmi_type_t dmi_type_intel_rsd_memory_device       = { .id = DMI_TYPE_ID(INTEL_RSD_MEMORY_DEVICE)       };
const dmi_type_t dmi_type_intel_rsd_fpga                = { .id = DMI_TYPE_ID(INTEL_RSD_FPGA)                };
const dmi_type_t dmi_type_intel_rsd_cabled_pcie         = { .id = DMI_TYPE_ID(INTEL_RSD_CABLED_PCIE)         };
const dmi_type_t dmi_type_intel_rsd_phys_device_mapping = { .id = DMI_TYPE_ID(INTEL_RSD_PHYS_DEVICE_MAPPING) };

/**
 * @brief Intel Rack Scale Design (RSD) extension module.
 *
 * Structures of the RSD firmware extension describe the devices of a rack for
 * its management software. They are only found on the platforms built to the
 * RSD specification, and take type numbers which vendors give to structures of
 * their own, so the module is only enabled explicitly.
 */
const dmi_module_t dmi_intel_rsd_module =
{
    .code     = "intel-rsd",
    .name     = "Intel RSD extensions",
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
