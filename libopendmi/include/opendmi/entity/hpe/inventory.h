//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_INVENTORY_H
#define OPENDMI_ENTITY_HPE_INVENTORY_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/hpe/common.h>

#ifndef DMI_HPE_INVENTORY_T
#   define DMI_HPE_INVENTORY_T
    typedef struct dmi_hpe_inventory dmi_hpe_inventory_t;
#endif // !DMI_HPE_INVENTORY_T

/**
 * @brief Attribute of a firmware image, as a bit of the attribute masks.
 */
typedef enum dmi_hpe_inventory_attr
{
    DMI_HPE_INVENTORY_ATTR_UPDATABLE     = 0, ///< Updatable
    DMI_HPE_INVENTORY_ATTR_RESET         = 1, ///< Reset required
    DMI_HPE_INVENTORY_ATTR_AUTHENTICATED = 2, ///< Authentication required
    DMI_HPE_INVENTORY_ATTR_IN_USE        = 3, ///< In use
    DMI_HPE_INVENTORY_ATTR_UEFI_IMAGE    = 4  ///< UEFI image
} dmi_hpe_inventory_attr_t;

/**
 * @brief HP/HPE firmware inventory record (type 240).
 *
 * Tells the version of the firmware of a device which reports it through its
 * UEFI driver, and the device correlation record (type 203) of the device.
 */
struct dmi_hpe_inventory
{
    /**
     * @brief Handle of the device correlation record (type 203).
     */
    dmi_handle_t correlation_handle;

    /**
     * @brief Version of the package of all the firmware of the device.
     */
    uint32_t package_version;

    /**
     * @brief Version of the firmware, as a string.
     */
    const char *version_string;

    /**
     * @brief Size of the firmware image in bytes, zero if not available.
     */
    uint64_t image_size;

    /**
     * @brief Mask of the attributes the record defines, whose bits are the
     * values of `dmi_hpe_inventory_attr_t`.
     */
    uint64_t attrs_defined;

    /**
     * @brief Mask of the defined attributes which are set.
     */
    uint64_t attrs_set;

    /**
     * @brief Whether the firmware is updatable.
     */
    dmi_hpe_flag_t is_updatable;

    /**
     * @brief Whether a reset is required after an update.
     */
    dmi_hpe_flag_t is_reset_required;

    /**
     * @brief Whether an update requires authentication.
     */
    dmi_hpe_flag_t is_auth_required;

    /**
     * @brief Whether the firmware image is in use.
     */
    dmi_hpe_flag_t is_in_use;

    /**
     * @brief Whether the firmware image is a UEFI one.
     */
    dmi_hpe_flag_t is_uefi_image;

    /**
     * @brief Lowest version the firmware may be downgraded to, zero if not
     * available.
     */
    uint32_t lowest_version;
};

/**
 * @brief HP/HPE firmware inventory record entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_inventory_spec;

#endif // !OPENDMI_ENTITY_HPE_INVENTORY_H
