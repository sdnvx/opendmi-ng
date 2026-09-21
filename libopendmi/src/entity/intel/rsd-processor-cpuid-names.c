//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-processor-cpuid-internal.h>

const dmi_name_set_t dmi_intel_rsd_cpuid_subtype_names =
{
    .code  = "intel-rsd-cpuid-subtype",
    .names = DMI_NAMES({
        {
            .id   = DMI_INTEL_RSD_CPUID_SUBTYPE_BASIC,
            .code = "basic",
            .name = "Leaves 00h to 10h"
        },
        {
            .id   = DMI_INTEL_RSD_CPUID_SUBTYPE_EXTENDED,
            .code = "extended",
            .name = "Leaves 14h to 80000008h"
        },
        {}
    })
};
