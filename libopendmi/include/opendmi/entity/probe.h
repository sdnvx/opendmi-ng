//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_PROBE_H
#define OPENDMI_ENTITY_PROBE_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/common.h>

typedef struct dmi_probe dmi_probe_t;
typedef union dmi_probe_details dmi_probe_details_t;

#define DMI_PROBE_VALUE_UNKNOWN ((dmi_word_t)0x8000U)

/**
 * @brief Probe locations.
 */
typedef enum dmi_probe_location
{
    DMI_PROBE_LOCATION_UNSPEC             = 0x00, ///< Unspecified
    DMI_PROBE_LOCATION_OTHER              = 0x01, ///< Other
    DMI_PROBE_LOCATION_UNKNOWN            = 0x02, ///< Unknown
    DMI_PROBE_LOCATION_PROCESSOR          = 0x03, ///< Processor
    DMI_PROBE_LOCATION_DISK               = 0x04, ///< Disk
    DMI_PROBE_LOCATION_PERIPHERAL_BAY     = 0x05, ///< Peripheral bay
    DMI_PROBE_LOCATION_SYSTEM_MGMT_MODULE = 0x06, ///< System management module
    DMI_PROBE_LOCATION_MOTHERBOARD        = 0x07, ///< Motherboard
    DMI_PROBE_LOCATION_MEMORY_MODULE      = 0x08, ///< Memory module
    DMI_PROBE_LOCATION_PROCESSOR_MODULE   = 0x09, ///< Processor module
    DMI_PROBE_LOCATION_POWER_UNIT         = 0x0A, ///< Power unit
    DMI_PROBE_LOCATION_ADDIN_CARD         = 0x0B, ///< Add-in card
    DMI_PROBE_LOCATION_FRONT_PANEL_BOARD  = 0x0C, ///< Front panel board
    DMI_PROBE_LOCATION_BACK_PANEL_BOARD   = 0x0D, ///< Back panel board
    DMI_PROBE_LOCATION_POWER_SYSTEM_BOARD = 0x0E, ///< Power system board
    DMI_PROBE_LOCATION_DRIVE_BACK_PLANE   = 0x0F, ///< Drive back plane
    __DMI_PROBE_LOCATION_COUNT
} dmi_probe_location_t;

/**
 * @brief Probe location and status details.
 */
dmi_packed_union(dmi_probe_details)
{
    /**
     * @brief Raw value.
     */
    dmi_byte_t __value;

    dmi_packed_struct() {
        /**
         * @brief Physical location.
         */
        dmi_byte_t location : 5;

        /**
         * @brief Status.
         */
        dmi_byte_t status : 3;
    };
};

dmi_static_assert_value_union(dmi_probe_details);

struct dmi_probe
{
    /**
     * @brief Additional descriptive information about the probe or its
     * location.
     */
    const char *description;

    /**
     * @brief Physical location.
     */
    dmi_probe_location_t location;

    /**
     * @brief Status.
     */
    dmi_status_t status;

    /**
     * @brief Maximum value readable by this probe, in millivolts, milliamps,
     * or 1/10th degrees Celsius. If the value is unknown, the field is set to
     * `SHRT_MIN`.
     */
    short maximum_value;

    /**
     * @brief Minimum value readable by this probe, in millivolts, milliamps,
     * or 1/10th degrees Celsius. If the value is unknown, the field is set to
     * `SHRT_MIN`.
     */
    short minimum_value;

    /**
     * @brief Resolution for the probe's reading, in 1/10th millivolts,
     * 1/10th milliamps, or 1/1000th degrees Celsius. If the value is unknown,
     * the field is set to `SHRT_MIN`.
     */
    short resolution;

    /**
     * @brief Tolerance for reading from this probe, in plus or minus
     * millivolts, milliamps, or 1/10th degrees Celsius. If the value is
     * unknown, the field is set to `SHRT_MIN`.
     */
    short tolerance;

    /**
     * @brief Accuracy for reading from this probe, in plus or minus 1/100th
     * of a percent. If the value is unknown, the field is set to `SHRT_MIN`.
     */
    short accuracy;

    /**
     * @brief OEM- or firmware vendor-specific information.
     */
    uint32_t oem_defined;

    /**
     * @brief Nominal value for the probe’s reading, in millivolts, milliamps,
     * or 1/10th degrees Celsius. If the value is unknown, the field is set to
     * `SHRT_MIN`.
     */
    short nominal_value;
};

extern __dmi_api const dmi_name_set_t dmi_probe_location_names;

__BEGIN_DECLS

__dmi_api const char *dmi_probe_location_name(dmi_probe_location_t value);

/**
 * @internal
 * @brief Decode common probe fields from an SMBIOS entity.
 *
 * Reads the probe description, location, status, value range (maximum,
 * minimum, resolution, tolerance, accuracy), OEM-defined data, and optional
 * nominal value from the entity's data stream into a @ref dmi_probe_t
 * structure. This function is shared by type-specific probe decoders
 * (voltage, temperature, current).
 *
 * @param[in] entity Entity descriptor.
 *
 * @return `true` on success, `false` on decoding failure.
 */
__dmi_api bool dmi_probe_decode(dmi_entity_t *entity);

__END_DECLS

#endif // !OPENDMI_ENTITY_PROBE_H
