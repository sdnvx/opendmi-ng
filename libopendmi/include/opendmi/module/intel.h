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
 * @brief Intel reference code structure type identifiers.
 *
 * These are the type numbers of the Intel reference code, which some vendors
 * relocate, see `dmi_relocation_t`.
 */
typedef enum dmi_intel_type
{
    DMI_TYPE_INTEL_AMT  = 130, ///< Intel Active Management Technology information
    DMI_TYPE_INTEL_VPRO = 131, ///< Intel vPro information
    DMI_TYPE_INTEL_MEI  = 219, ///< Intel Management Engine interface information
    DMI_TYPE_INTEL_FVI  = 221, ///< Intel firmware version information
    DMI_TYPE_INTEL_SVT  = 222  ///< Intel Silicon View Technology milestones
} dmi_intel_type_t;

__BEGIN_DECLS

extern __dmi_api const dmi_module_t dmi_intel_module;

__END_DECLS

#endif // !OPENDMI_MODULE_INTEL_H
