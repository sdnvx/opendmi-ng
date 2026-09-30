//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/amt.h>
#include <opendmi/entity/intel/fvi.h>
#include <opendmi/entity/intel/mei.h>
#include <opendmi/entity/intel/svt.h>
#include <opendmi/entity/intel/vpro.h>

/**
 * @brief Intel extension module.
 *
 * Structures of the Intel reference code, which the firmware of Intel
 * platforms carries whatever its vendor is. Vendors give the same type numbers
 * to structures of their own, so the module is enabled for any platform with
 * Intel processors, and yields the types the modules of the vendors take.
 */
const dmi_module_t dmi_intel_module =
{
    .code      = "intel",
    .name      = "Intel extensions",
    .entities  = (const dmi_entity_spec_t *[]){
        &dmi_intel_amt_spec,
        &dmi_intel_vpro_spec,
        &dmi_intel_mei_spec,
        &dmi_intel_fvi_spec,
        &dmi_intel_svt_spec,
        nullptr
    },
    .flags     = DMI_MODULE_FLAG_YIELD,
    .platforms = DMI_PLATFORMS({
        { .processor_vendor = DMI_VENDOR_INTEL },
        DMI_PLATFORM_NULL
    }),
    .groups    = DMI_GROUPS({
        { "$MEI",                             &dmi_intel_mei_spec },
        { "Firmware Version Info",            &dmi_intel_fvi_spec },
        { "Intel(R) Silicon View Technology", &dmi_intel_svt_spec },
        {}
    })
};
