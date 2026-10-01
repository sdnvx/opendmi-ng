//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_DIMM_LOCATION_H
#define OPENDMI_ENTITY_HPE_DIMM_LOCATION_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_HPE_DIMM_LOCATION_T
#   define DMI_HPE_DIMM_LOCATION_T
    typedef struct dmi_hpe_dimm_location dmi_hpe_dimm_location_t;
#endif // !DMI_HPE_DIMM_LOCATION_T

/**
 * @brief HP/HPE DIMM location record (type 202).
 *
 * Tells where the DIMM socket of a memory device (type 17) is: the memory
 * board, the processor and the channel it belongs to. There is a record for
 * each socket, including the ones of the memory boards which are not
 * installed.
 */
struct dmi_hpe_dimm_location
{
    /**
     * @brief Handle of the memory device structure (type 17).
     */
    dmi_handle_t device_handle;

    /**
     * @brief 1-based number of the memory board. Set to `UINT8_MAX` for the
     * sockets of the system board.
     */
    uint8_t board;

    /**
     * @brief 1-based number of the DIMM.
     */
    uint8_t dimm;

    /**
     * @brief 1-based number of the processor. Set to `UINT8_MAX` when not
     * shown.
     */
    uint8_t processor;

    /**
     * @brief 1-based logical number of the DIMM, as ACPI numbers it, zero if
     * unspecified.
     */
    uint8_t logical_dimm;

    /**
     * @brief UEFI device path.
     */
    const char *uefi_device_path;

    /**
     * @brief UEFI device structured name.
     */
    const char *uefi_device_name;

    /**
     * @brief Device name.
     */
    const char *device_name;

    /**
     * @brief 1-based number of the memory controller, zero if unknown.
     */
    uint8_t controller;

    /**
     * @brief 1-based number of the memory channel, as the silk screen names
     * it, zero if unknown.
     */
    uint8_t channel;

    /**
     * @brief 0-based number of the DIMM, as the Innovation Engine reports it.
     * Set to `UINT8_MAX` when not supported. Reserved from Gen12 onwards.
     */
    uint8_t ie_dimm;

    /**
     * @brief Sensor ID of the Innovation Engine. Set to `UINT8_MAX` when not
     * supported. Reserved from Gen12 onwards.
     */
    uint8_t ie_pldm_id;

    /**
     * @brief Manufacturer ID code of the module, as the SPD gives it.
     */
    uint16_t vendor_id;

    /**
     * @brief Product ID code of the module, NVDIMMs only.
     */
    uint16_t device_id;

    /**
     * @brief Manufacturer ID code of the controller, NVDIMMs only, as the
     * SPD gives it: the number of continuation codes in the low byte, whose
     * bit 7 is a parity bit, and the JEP106 code in the high byte.
     */
    uint16_t controller_vendor_id;

    /**
     * @brief Product ID code of the controller, NVDIMMs only.
     */
    uint16_t controller_device_id;

    /**
     * @brief 1-based interleave set within the processor, zero if unknown.
     */
    uint8_t interleave;

    /**
     * @brief HPE part number, from the OEM area of the SPD.
     */
    const char *part_number;

    /**
     * @brief 0-based index of the DIMM within its channel. Set to
     * `UINT8_MAX` when the structure holds none.
     */
    uint8_t channel_index;

    /**
     * @brief Whether the socket is one of the system board, which the board
     * number of `UINT8_MAX` tells. The board number is not shown then.
     */
    bool is_system_board;

    /**
     * @brief Whether the fields of the Innovation Engine are shown, which
     * they are up to Gen11.
     */
    bool has_ie;

    /**
     * @brief Whether the structure holds the index of the DIMM within its
     * channel, which the structures shorter than 28 bytes leave out.
     */
    bool has_channel_index;
};

/**
 * @brief HP/HPE DIMM location record entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_dimm_location_spec;

#endif // !OPENDMI_ENTITY_HPE_DIMM_LOCATION_H
