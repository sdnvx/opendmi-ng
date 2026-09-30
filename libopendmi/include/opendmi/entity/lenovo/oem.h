//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_LENOVO_OEM_H
#define OPENDMI_ENTITY_LENOVO_OEM_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_lenovo_oem               dmi_lenovo_oem_t;
typedef struct dmi_lenovo_device_presence   dmi_lenovo_device_presence_t;
typedef struct dmi_lenovo_ecp               dmi_lenovo_ecp_t;
typedef struct dmi_lenovo_bay_io            dmi_lenovo_bay_io_t;

/**
 * @brief Lenovo OEM data structure (types 135 and 140) whose layout is not
 * known.
 *
 * Lenovo firmware carries OEM data in structures of type 135, signed `TP`,
 * and of type 140, signed `LENOVO`. Each structure holds one OEM structure,
 * told by its number and revision, which follow the signature.
 */
struct dmi_lenovo_oem
{
    /**
     * @brief Signature, `TP` for type 135 and `LENOVO` for type 140.
     */
    dmi_binary_t signature;

    /**
     * @brief Offset of the OEM structure within the structure, which is the
     * offset of `number`.
     */
    uint8_t offset;

    /**
     * @brief Number of the OEM structure.
     */
    uint8_t number;

    /**
     * @brief Revision of the format of the OEM structure.
     */
    uint8_t revision;

    /**
     * @brief Data of the OEM structure, which runs to the end of the
     * structure.
     */
    dmi_binary_t data;
};

/**
 * @brief Lenovo device presence detection structure (type 135, OEM structure
 * 3, revision 1).
 *
 * Tells which devices of a ThinkPad are present, of which only the
 * fingerprint reader is known.
 */
struct dmi_lenovo_device_presence
{
    /**
     * @brief Signature, `TP`.
     */
    dmi_binary_t signature;

    /**
     * @brief Offset of the OEM structure, `0x07`.
     */
    uint8_t offset;

    /**
     * @brief Number of the OEM structure, `3`.
     */
    uint8_t number;

    /**
     * @brief Revision of the format of the OEM structure, `1`.
     */
    uint8_t revision;

    /**
     * @brief Device presence bits, of which only bit 0 is known.
     */
    uint8_t devices;

    /**
     * @brief Whether a fingerprint reader is present, as bit 0 of the device
     * presence bits tells.
     */
    bool is_fingerprint_reader;
};

/**
 * @brief Lenovo bay I/O structure (type 135, OEM structure 2).
 *
 * Describes the bays of a laptop, e.g. the UltraBay of older ThinkPads. The
 * OEM structure carries a signature of its own, `BAY I/O `, in place of the
 * revision, followed by its version, and the layout of the rest is not
 * known.
 */
struct dmi_lenovo_bay_io
{
    /**
     * @brief Signature, `TP`.
     */
    dmi_binary_t signature;

    /**
     * @brief Offset of the OEM structure, `0x07`.
     */
    uint8_t offset;

    /**
     * @brief Number of the OEM structure, `2`.
     */
    uint8_t number;

    /**
     * @brief Signature of the OEM structure, `BAY I/O `.
     */
    dmi_binary_t bay_signature;

    /**
     * @brief Version of the OEM structure.
     */
    uint8_t version;

    /**
     * @brief Data of the OEM structure, which runs to the end of the
     * structure.
     */
    dmi_binary_t data;
};

/**
 * @brief Lenovo ThinkPad embedded controller program structure (type 140,
 * OEM structure 7, revision 1).
 *
 * Tells the version of the program of the embedded controller, which the
 * README files of the firmware updates of Lenovo name.
 */
struct dmi_lenovo_ecp
{
    /**
     * @brief Signature, `LENOVO`.
     */
    dmi_binary_t signature;

    /**
     * @brief Offset of the OEM structure, `0x0B`.
     */
    uint8_t offset;

    /**
     * @brief Number of the OEM structure, `7`.
     */
    uint8_t number;

    /**
     * @brief Revision of the format of the OEM structure, `1`.
     */
    uint8_t revision;

    /**
     * @brief Version ID of the program, e.g. `N20HT28W`.
     */
    const char *version;

    /**
     * @brief Release date of the program, as the firmware writes it, e.g.
     * `10/08/2020`.
     */
    const char *release_date;
};

/**
 * @brief Lenovo mobile PC OEM data entity specification (type 135).
 */
extern __dmi_api const dmi_entity_spec_t dmi_lenovo_mobile_oem_spec;

/**
 * @brief Lenovo OEM data entity specification (type 140).
 */
extern __dmi_api const dmi_entity_spec_t dmi_lenovo_oem_spec;

/**
 * @brief Lenovo device presence detection entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_lenovo_device_presence_spec;

/**
 * @brief Lenovo bay I/O entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_lenovo_bay_io_spec;

/**
 * @brief Lenovo ThinkPad embedded controller program entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_lenovo_ecp_spec;

#endif // !OPENDMI_ENTITY_LENOVO_OEM_H
