//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_DIMM_ATTRS_H
#define OPENDMI_ENTITY_HPE_DIMM_ATTRS_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/hpe/common.h>

#ifndef DMI_HPE_DIMM_ATTRS_T
#   define DMI_HPE_DIMM_ATTRS_T
    typedef struct dmi_hpe_dimm_attrs dmi_hpe_dimm_attrs_t;
#endif // !DMI_HPE_DIMM_ATTRS_T

/**
 * @brief HP/HPE DIMM attributes record (type 232), from Gen9 onwards.
 *
 * Tells what the memory device structure (type 17) of a DIMM socket does
 * not: whether the module is an HPE one, and the voltages it runs at. There
 * is a record for each socket.
 */
struct dmi_hpe_dimm_attrs
{
    /**
     * @brief Handle of the memory device structure (type 17).
     */
    dmi_handle_t device_handle;

    /**
     * @brief Attributes of the module, as the structure holds them.
     */
    uint32_t attributes;

    /**
     * @brief Whether the module is HPE SmartMemory, as bits 0 and 1 of the
     * attributes tell.
     */
    dmi_hpe_flag_t smart_memory;

    /**
     * @brief Whether the module is a load reduced one (LRDIMM), as bits 2
     * and 3 of the attributes tell.
     */
    dmi_hpe_flag_t load_reduced;

    /**
     * @brief Whether the module is HPE standard memory, as bits 4 and 5 of
     * the attributes tell.
     */
    dmi_hpe_flag_t standard_memory;

    /**
     * @brief Minimum operating voltage in millivolts, zero if unknown.
     */
    uint16_t minimum_voltage;

    /**
     * @brief Configured operating voltage in millivolts, zero if unknown.
     */
    uint16_t configured_voltage;

    /**
     * @brief Whether the module is mapped out because of a configuration
     * error.
     */
    bool is_config_error;

    /**
     * @brief Whether the module is mapped out because of a training error.
     */
    bool is_training_error;

    /**
     * @brief Encryption status of the module. Set to `UINT8_MAX` when the
     * structure holds none.
     */
    dmi_hpe_encryption_t encryption;
};

/**
 * @brief HP/HPE DIMM attributes record entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_dimm_attrs_spec;

#endif // !OPENDMI_ENTITY_HPE_DIMM_ATTRS_H
