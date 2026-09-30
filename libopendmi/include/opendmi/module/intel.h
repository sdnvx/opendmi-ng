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
typedef enum dmi_intel_type_id
{
    DMI_TYPE_ID_INTEL_AMT  = 130, ///< Intel Active Management Technology information
    DMI_TYPE_ID_INTEL_VPRO = 131, ///< Intel vPro information
    DMI_TYPE_ID_INTEL_MEI  = 219, ///< Intel Management Engine interface information
    DMI_TYPE_ID_INTEL_FVI  = 221, ///< Intel firmware version information
    DMI_TYPE_ID_INTEL_SVT  = 222  ///< Intel Silicon View Technology milestones
} dmi_intel_type_id_t;

__BEGIN_DECLS

/** @brief Intel Active Management Technology information */
extern __dmi_api const dmi_type_t dmi_type_intel_amt;

/** @brief Intel vPro information */
extern __dmi_api const dmi_type_t dmi_type_intel_vpro;

/** @brief Intel Management Engine interface information */
extern __dmi_api const dmi_type_t dmi_type_intel_mei;

/** @brief Intel firmware version information */
extern __dmi_api const dmi_type_t dmi_type_intel_fvi;

/** @brief Intel Silicon View Technology milestones */
extern __dmi_api const dmi_type_t dmi_type_intel_svt;

extern __dmi_api const dmi_module_t dmi_intel_module;

__END_DECLS

#endif // !OPENDMI_MODULE_INTEL_H
