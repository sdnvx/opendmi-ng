//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_MODULE_LENOVO_H
#define OPENDMI_MODULE_LENOVO_H

#pragma once

#include <opendmi/module.h>

/**
 * @brief Lenovo structure type identifiers.
 */
typedef enum dmi_lenovo_type
{
    DMI_TYPE_LENOVO_TVT        = 131, ///< ThinkVantage Technologies enablement
    DMI_TYPE_LENOVO_DATE       = 134, ///< Date record
    DMI_TYPE_LENOVO_MOBILE_OEM = 135, ///< Mobile PC OEM data, signed "TP"
    DMI_TYPE_LENOVO_OEM        = 140, ///< OEM data, signed "LENOVO"
    DMI_TYPE_LENOVO_MTM        = 200  ///< Machine type model
} dmi_lenovo_type_t;

__BEGIN_DECLS

extern __dmi_api const dmi_module_t dmi_lenovo_module;

__END_DECLS

#endif // !OPENDMI_MODULE_LENOVO_H
