//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-memory-device-internal.h>

const dmi_name_set_t dmi_intel_rsd_memory_type_names =
{
    .code  = "intel-rsd-memory-type",
    .names = DMI_NAMES({
        {
            .id   = DMI_INTEL_RSD_MEMORY_TYPE_DIMM,
            .code = "dimm",
            .name = "DIMM"
        },
        {
            .id   = DMI_INTEL_RSD_MEMORY_TYPE_NVDIMM_N,
            .code = "nvdimm-n",
            .name = "NVDIMM-N (Byte accessible persistent memory)"
        },
        {
            .id   = DMI_INTEL_RSD_MEMORY_TYPE_NVDIMM_F,
            .code = "nvdimm-f",
            .name = "NVDIMM-F (Block accessible persistent memory)"
        },
        {
            .id   = DMI_INTEL_RSD_MEMORY_TYPE_NVDIMM_P,
            .code = "nvdimm-p",
            .name = "NVDIMM-P"
        },
        {}
    })
};

const dmi_name_set_t dmi_intel_rsd_memory_media_names =
{
    .code  = "intel-rsd-memory-media",
    .names = DMI_NAMES({
        {
            .id   = DMI_INTEL_RSD_MEMORY_MEDIA_DRAM,
            .code = "dram",
            .name = "DRAM"
        },
        {
            .id   = DMI_INTEL_RSD_MEMORY_MEDIA_NAND,
            .code = "nand",
            .name = "NAND"
        },
        {
            .id   = DMI_INTEL_RSD_MEMORY_MEDIA_PROPRIETARY,
            .code = "proprietary",
            .name = "Proprietary"
        },
        {}
    })
};

const char *dmi_intel_rsd_memory_type_name(dmi_intel_rsd_memory_type_t value)
{
    return dmi_name_lookup(&dmi_intel_rsd_memory_type_names, (int)value);
}

const char *dmi_intel_rsd_memory_media_name(dmi_intel_rsd_memory_media_t value)
{
    return dmi_name_lookup(&dmi_intel_rsd_memory_media_names, (int)value);
}
