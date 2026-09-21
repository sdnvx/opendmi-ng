//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-phys-device-mapping-internal.h>

const dmi_name_set_t dmi_intel_rsd_phys_device_type_names =
{
    .code  = "intel-rsd-phys-device-type",
    .names = DMI_NAMES({
        {
            .id   = DMI_INTEL_RSD_PHYS_DEVICE_TYPE_INVALID,
            .code = "invalid",
            .name = "Undefined or invalid"
        },
        {
            .id   = DMI_INTEL_RSD_PHYS_DEVICE_TYPE_PROCESSOR,
            .code = "processor",
            .name = "Processor"
        },
        {
            .id   = DMI_INTEL_RSD_PHYS_DEVICE_TYPE_PCIE_SLOT,
            .code = "pcie-slot",
            .name = "PCIe system slot"
        },
        {
            .id   = DMI_INTEL_RSD_PHYS_DEVICE_TYPE_MEMORY,
            .code = "memory-device",
            .name = "Memory device"
        },
        {}
    })
};
