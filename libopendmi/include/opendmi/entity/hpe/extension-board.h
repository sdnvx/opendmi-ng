//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_EXTENSION_BOARD_H
#define OPENDMI_ENTITY_HPE_EXTENSION_BOARD_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_HPE_RISER_T
#   define DMI_HPE_RISER_T
    typedef struct dmi_hpe_riser dmi_hpe_riser_t;
#endif // !DMI_HPE_RISER_T

#ifndef DMI_HPE_MHS_RISER_T
#   define DMI_HPE_MHS_RISER_T
    typedef struct dmi_hpe_mhs_riser dmi_hpe_mhs_riser_t;
#endif // !DMI_HPE_MHS_RISER_T

/**
 * @brief Type of an extension board, which selects the layout of the rest of
 * the structure.
 */
typedef enum dmi_hpe_board_type
{
    DMI_HPE_BOARD_TYPE_RISER     = 0x00, ///< PCIe riser
    DMI_HPE_BOARD_TYPE_MHS_RISER = 0x01  ///< PCIe riser of an MHS platform
} dmi_hpe_board_type_t;

/**
 * @brief Position of a PCIe riser.
 */
typedef enum dmi_hpe_riser_position
{
    DMI_HPE_RISER_POSITION_PRIMARY    = 0x01, ///< Primary
    DMI_HPE_RISER_POSITION_SECONDARY  = 0x02, ///< Secondary
    DMI_HPE_RISER_POSITION_TERTIARY   = 0x03, ///< Tertiary
    DMI_HPE_RISER_POSITION_QUATERNARY = 0x04, ///< Quaternary
    DMI_HPE_RISER_POSITION_FRONT      = 0x0A  ///< Front
} dmi_hpe_riser_position_t;

/**
 * @brief HP/HPE extension board inventory record (type 245) of a PCIe riser.
 *
 * Describes a PCIe riser installed in the server, as ML, DL and Alletra
 * servers do from Gen11 onwards.
 */
struct dmi_hpe_riser
{
    /**
     * @brief Type of the board, `DMI_HPE_BOARD_TYPE_RISER`.
     */
    dmi_hpe_board_type_t board_type;

    /**
     * @brief Position of the riser.
     */
    dmi_hpe_riser_position_t position;

    /**
     * @brief ID of the riser.
     */
    uint8_t riser_id;

    /**
     * @brief Version of the CPLD of the riser, zero if the riser has no
     * CPLD.
     */
    uint8_t cpld_version;

    /**
     * @brief Whether the version of the CPLD is a `B.` release.
     */
    bool is_cpld_b_release;

    /**
     * @brief Name of the riser.
     */
    const char *name;
};

/**
 * @brief HP/HPE extension board inventory record (type 245) of a PCIe riser
 * of an MHS platform.
 */
struct dmi_hpe_mhs_riser
{
    /**
     * @brief Type of the board, `DMI_HPE_BOARD_TYPE_MHS_RISER`.
     */
    dmi_hpe_board_type_t board_type;

    /**
     * @brief ID of the riser.
     */
    uint8_t riser_id;

    /**
     * @brief Major version of the firmware of the riser, zero if the riser
     * has no firmware.
     */
    uint8_t firmware_major;

    /**
     * @brief Minor version of the firmware of the riser.
     */
    uint8_t firmware_minor;

    /**
     * @brief Whether the firmware of the riser may be downgraded.
     */
    bool is_downgradable;

    /**
     * @brief Name of the riser.
     */
    const char *name;

    /**
     * @brief Number of the slots the structure declares.
     */
    uint8_t slot_total;

    /**
     * @brief Number of elements of `slot_ids`, which is smaller than
     * `slot_total` when the data ends before the last slot.
     */
    size_t slot_count;

    /**
     * @brief IDs of the slots of the riser. May be @c nullptr when
     * `slot_count` is 0.
     */
    uint8_t *slot_ids;
};

/**
 * @brief HP/HPE extension board inventory record entity specification of
 * PCIe risers.
 *
 * The board type at offset `0x04` selects the layout of the rest of the
 * structure, which each specification tells by its signature.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_riser_spec;

/**
 * @brief HP/HPE extension board inventory record entity specification of
 * PCIe risers of MHS platforms.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_mhs_riser_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_hpe_board_type_name(dmi_hpe_board_type_t value);
__dmi_api const char *dmi_hpe_riser_position_name(dmi_hpe_riser_position_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_HPE_EXTENSION_BOARD_H
