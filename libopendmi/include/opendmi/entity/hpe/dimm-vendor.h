//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_DIMM_VENDOR_H
#define OPENDMI_ENTITY_HPE_DIMM_VENDOR_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_HPE_DIMM_VENDOR_T
#   define DMI_HPE_DIMM_VENDOR_T
    typedef struct dmi_hpe_dimm_vendor dmi_hpe_dimm_vendor_t;
#endif // !DMI_HPE_DIMM_VENDOR_T

/**
 * @brief HP/HPE DIMM vendor information (type 237), from Gen9 onwards.
 *
 * Tells the actual manufacturer of the module of a memory device (type 17),
 * whose standard fields hold the ones of HPE.
 */
struct dmi_hpe_dimm_vendor
{
    /**
     * @brief Handle of the memory device structure (type 17).
     */
    dmi_handle_t device_handle;

    /**
     * @brief Manufacturer of the module.
     */
    const char *manufacturer;

    /**
     * @brief Part number of the module, as its manufacturer gives it.
     */
    const char *part_number;

    /**
     * @brief Serial number of the module, as its manufacturer gives it.
     */
    const char *serial_number;

    /**
     * @brief Last two digits of the year the module was manufactured in,
     * from 2000, zero if unknown.
     */
    uint8_t manufacture_year;

    /**
     * @brief Week of the year the module was manufactured in, zero if
     * unknown.
     */
    uint8_t manufacture_week;
};

/**
 * @brief HP/HPE DIMM vendor information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_dimm_vendor_spec;

#endif // !OPENDMI_ENTITY_HPE_DIMM_VENDOR_H
