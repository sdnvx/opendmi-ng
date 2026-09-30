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
typedef enum dmi_lenovo_type_id
{
    DMI_TYPE_ID_LENOVO_TVT        = 131, ///< ThinkVantage Technologies enablement
    DMI_TYPE_ID_LENOVO_DATE       = 134, ///< Date record
    DMI_TYPE_ID_LENOVO_MOBILE_OEM = 135, ///< Mobile PC OEM data, signed "TP"
    DMI_TYPE_ID_LENOVO_OEM        = 140, ///< OEM data, signed "LENOVO"
    DMI_TYPE_ID_LENOVO_MTM        = 200  ///< Machine type model
} dmi_lenovo_type_id_t;

__BEGIN_DECLS

/** @brief ThinkVantage Technologies enablement */
extern __dmi_api const dmi_type_t dmi_type_lenovo_tvt;

/** @brief Date record */
extern __dmi_api const dmi_type_t dmi_type_lenovo_date;

/** @brief TPM information */
extern __dmi_api const dmi_type_t dmi_type_lenovo_tpm_info;

/** @brief Mobile PC OEM data */
extern __dmi_api const dmi_type_t dmi_type_lenovo_mobile_oem;

/** @brief Device presence detection */
extern __dmi_api const dmi_type_t dmi_type_lenovo_device_presence;

/** @brief Bay I/O */
extern __dmi_api const dmi_type_t dmi_type_lenovo_bay_io;

/** @brief OEM data */
extern __dmi_api const dmi_type_t dmi_type_lenovo_oem;

/** @brief ThinkPad embedded controller program */
extern __dmi_api const dmi_type_t dmi_type_lenovo_ecp;

/** @brief Machine type model */
extern __dmi_api const dmi_type_t dmi_type_lenovo_mtm;

extern __dmi_api const dmi_module_t dmi_lenovo_module;

__END_DECLS

#endif // !OPENDMI_MODULE_LENOVO_H
