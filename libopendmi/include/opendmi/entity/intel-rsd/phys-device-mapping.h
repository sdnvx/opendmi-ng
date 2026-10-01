//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_RSD_PHYS_DEVICE_MAPPING_H
#define OPENDMI_ENTITY_INTEL_RSD_PHYS_DEVICE_MAPPING_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_INTEL_RSD_PHYS_DEVICE_MAPPING_T
#   define DMI_INTEL_RSD_PHYS_DEVICE_MAPPING_T
    typedef struct dmi_intel_rsd_phys_device_mapping dmi_intel_rsd_phys_device_mapping_t;
#endif // !DMI_INTEL_RSD_PHYS_DEVICE_MAPPING_T

#ifndef DMI_INTEL_RSD_PHYS_DEVICE_T
#   define DMI_INTEL_RSD_PHYS_DEVICE_T
    typedef struct dmi_intel_rsd_phys_device dmi_intel_rsd_phys_device_t;
#endif // !DMI_INTEL_RSD_PHYS_DEVICE_T

/**
 * @brief Intel RSD physical device types.
 */
typedef enum dmi_intel_rsd_phys_device_type
{
    DMI_INTEL_RSD_PHYS_DEVICE_TYPE_INVALID   = 0x00, ///< Undefined or invalid
    DMI_INTEL_RSD_PHYS_DEVICE_TYPE_PROCESSOR = 0x01, ///< Processor (type 4)
    DMI_INTEL_RSD_PHYS_DEVICE_TYPE_PCIE_SLOT = 0x02, ///< PCIe system slot (type 9)
    DMI_INTEL_RSD_PHYS_DEVICE_TYPE_MEMORY    = 0x03  ///< Memory device (type 17)
} dmi_intel_rsd_phys_device_type_t;

/**
 * @brief Physical location of the device.
 */
struct dmi_intel_rsd_phys_device
{
    /**
     * @brief Handle of the structure describing the device.
     */
    dmi_handle_t handle;

    /**
     * @brief Device type, the same for all devices of the structure. Selects
     * meaning of the location numbers.
     */
    dmi_intel_rsd_phys_device_type_t type;

    /**
     * @brief Zero-based number of the processor socket for processors and
     * memory devices, or of the riser for PCIe system slots.
     */
    uint8_t primary_number;

    /**
     * @brief Zero-based number of the slot within the riser for PCIe system
     * slots, or of the DIMM slot within the processor for memory devices.
     * Reserved for processors.
     */
    uint8_t secondary_number;

    /**
     * @brief Set if the device type is unknown, so that the location data
     * is shown as stored.
     */
    bool is_raw;

    /**
     * @brief Location data, as stored.
     */
    dmi_binary_t data;
};

/**
 * @brief Intel RSD physical device mapping information (type 200).
 */
struct dmi_intel_rsd_phys_device_mapping
{
    /**
     * @brief Type of the devices described by the structure.
     */
    dmi_intel_rsd_phys_device_type_t device_type;

    /**
     * @brief Number of devices.
     */
    size_t device_count;

    /**
     * @brief Physical locations of the devices.
     */
    dmi_intel_rsd_phys_device_t *devices;
};

/**
 * @brief Intel RSD physical device types.
 */
extern __dmi_api const dmi_name_set_t dmi_intel_rsd_phys_device_type_names;

/**
 * @brief Intel RSD physical device mapping information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_rsd_phys_device_mapping_spec;

__BEGIN_DECLS

/**
 * @brief Get Intel RSD physical device type name.
 *
 * Returns the name of the Intel RSD physical device type, as the command line
 * tool shows it, translated into the locale when the library is built with the
 * translations.
 *
 * @param[in] value Intel RSD physical device type value.
 *
 * @return The name of the value, or @c nullptr if @p value has no name.
 */
__dmi_api const char *dmi_intel_rsd_phys_device_type_name(dmi_intel_rsd_phys_device_type_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_INTEL_RSD_PHYS_DEVICE_MAPPING_H
