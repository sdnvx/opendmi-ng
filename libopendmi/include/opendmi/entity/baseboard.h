//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_BASEBOARD_H
#define OPENDMI_ENTITY_BASEBOARD_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_baseboard          dmi_baseboard_t;
typedef union  dmi_baseboard_features dmi_baseboard_features_t;

/**
 * @brief Baseboard type identifiers.
 */
typedef enum dmi_baseboard_type
{
    DMI_BASEBOARD_TYPE_UNSPEC                   = 0x00, ///< Unspecified
    DMI_BASEBOARD_TYPE_UNKNOWN                  = 0x01, ///< Unknown
    DMI_BASEBOARD_TYPE_OTHER                    = 0x02, ///< Other
    DMI_BASEBOARD_TYPE_SERVER_BLADE             = 0x03, ///< Server blade
    DMI_BASEBOARD_TYPE_CONNECTIVITY_SWITCH      = 0x04, ///< Connectivity switch
    DMI_BASEBOARD_TYPE_SYSTEM_MANAGEMENT_MODULE = 0x05, ///< System management module
    DMI_BASEBOARD_TYPE_PROCESSOR_MODULE         = 0x06, ///< Processor module
    DMI_BASEBOARD_TYPE_IO_MODULE                = 0x07, ///< IO module
    DMI_BASEBOARD_TYPE_MEMORY_MODULE            = 0x08, ///< Memory module
    DMI_BASEBOARD_TYPE_DAUGHTERBOARD            = 0x09, ///< Daughterboard
    DMI_BASEBOARD_TYPE_MOTHERBOARD              = 0x0A, ///< Motherboard
    DMI_BASEBOARD_TYPE_PROCESSOR_MEMORY_MODULE  = 0x0B, ///< Processor/memory module
    DMI_BASEBOARD_TYPE_PROCESSOR_IO_MODULE      = 0x0C, ///< Processor/IO module
    DMI_BASEBOARD_TYPE_INTERCONNECT_BOARD       = 0x0D, ///< Interconnect board
    __DMI_BASEBOARD_TYPE_COUNT
} dmi_baseboard_type_t;

/**
 * @brief Baseboard feature flags.
 */
dmi_packed_union(dmi_baseboard_features)
{
    /**
     * @brief Raw value.
     */
    dmi_byte_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Set to `true` if the board is a hosting board (for example, a
         * motherboard).
         */
        dmi_byte_t is_hosting_board : 1;

        /**
         * @brief Set to `true` if the board requires at least one daughter board
         * or auxiliary card to function properly.
         */
        dmi_byte_t require_daughter_board : 1;

        /**
         * @brief Set to `true` if the board is removable. It is designed to be
         * taken in and out of the chassis without impairing the function of the
         * chassis.
         */
        dmi_byte_t is_removable : 1;

        /**
         * @brief Set to `true` if the board is replaceable. It is possible to
         * replace (either as a field repair or as an upgrade) the board with a
         * physically different board. The board is inherently removable.
         */
        dmi_byte_t is_replaceable : 1;

        /**
         * @brief Set to `true` if the board is hot swappable; it is possible to
         * replace the board with a physically different but equivalent board while
         * power is applied to the board. The board is inherently replaceable and
         * removable.
         */
        dmi_byte_t is_hot_swappable : 1;

        /**
         * @brief Reserved for future definition by SMBIOS specification, set to 0.
         */
        dmi_byte_t __reserved : 3;
    };
};

dmi_static_assert_value_union(dmi_baseboard_features);

/**
 * @brief Baseboard or module information structure (type 2).
 *
 * The information in this structure defines attributes of a system baseboard
 * (for example, a motherboard, planar, server blade, or other standard system
 * module).
 *
 * @note
 * If more than one type 2 structure is provided by an SMBIOS implementation,
 * each structure shall include the Number of Contained Object Handles and
 * Contained Object Handles fields to specify which system elements are
 * contained on which boards. If a single type 2 structure is provided and the
 * contained object information is not present, or if no type 2 structure is
 * provided, all system elements identified by the SMBIOS implementation are
 * associated with a single motherboard.
 */
struct dmi_baseboard
{
    /**
     * @brief Manufacturer name.
     */
    const char *vendor;

    /**
     * @brief Product name.
     */
    const char *product;

    /**
     * @brief Product version.
     */
    const char *version;

    /**
     * @brief Serial number.
     */
    const char *serial_number;

    /**
     * @brief Asset tag.
     */
    const char *asset_tag;

    /**
     * @brief Collection of flags that identify features of this baseboard.
     */
    dmi_baseboard_features_t features;

    /**
     * @brief String that describes this board's location within the chassis
     * referenced by the chassis handle (described below in this structure).
     */
    const char *location;

    /**
     * @brief Handle, or instance number, associated with the chassis in which
     * this board resides.
     */
    dmi_handle_t chassis_handle;

    /**
     * @brief The chassis in which this board resides.
     */
    dmi_entity_t *chassis;

    /**
     * @brief Type of board.
     */
    dmi_baseboard_type_t type;

    /**
     * @brief Number of contained object handles.
     */
    size_t object_count;

    /**
     * @brief List of handles of other structures (for example, baseboard,
     * processor, port, system slots, memory device) that are contained by
     * this baseboard.
     */
    dmi_handle_t *object_handles;

    /**
     * @brief Contained object entities.
     */
    dmi_entity_t **objects;
};

extern const dmi_name_set_t dmi_baseboard_type_names;

/**
 * @brief Baseboard or module information entity specification.
 */
extern const dmi_entity_spec_t dmi_baseboard_spec;

__BEGIN_DECLS

/**
 * @brief Get baseboard type name.
 *
 * Returns the human-readable name of the baseboard type.
 *
 * @param[in] value Baseboard type value.
 * @return The baseboard type name string, or @c nullptr if @p value is out of
 * range.
 */
const char *dmi_baseboard_type_name(dmi_baseboard_type_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_BASEBOARD_H
