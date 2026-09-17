//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_COOLING_DEVICE_H
#define OPENDMI_ENTITY_COOLING_DEVICE_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/common.h>

typedef struct dmi_cooling_device      dmi_cooling_device_t;
typedef struct dmi_cooling_device_data dmi_cooling_device_data_t;

/**
 * @brief Cooling device types.
 */
typedef enum dmi_cooling_device_type
{
    DMI_COOLING_DEVICE_TYPE_UNSPEC                   = 0x00, ///< Unspecified
    DMI_COOLING_DEVICE_TYPE_OTHER                    = 0x01, ///< Other
    DMI_COOLING_DEVICE_TYPE_UNKNOWN                  = 0x02, ///< Unknown
    DMI_COOLING_DEVICE_TYPE_FAN                      = 0x03, ///< Fan
    DMI_COOLING_DEVICE_TYPE_CENTRIFUGAL_BLOWER       = 0x04, ///< Centrifugal blower
    DMI_COOLING_DEVICE_TYPE_CHIP_FAN                 = 0x05, ///< Chip fan
    DMI_COOLING_DEVICE_TYPE_CABINET_FAN              = 0x06, ///< Cabinet fan
    DMI_COOLING_DEVICE_TYPE_POWER_SUPPLY_FAN         = 0x07, ///< Power supply fan
    DMI_COOLING_DEVICE_TYPE_HEAT_PIPE                = 0x08, ///< Heat pipe
    DMI_COOLING_DEVICE_TYPE_INTEGRATED_REFRIGERATION = 0x09, ///< Integrated refrigeration
    DMI_COOLING_DEVICE_TYPE_ACTIVE_COOLING           = 0x10, ///< Active cooling
    DMI_COOLING_DEVICE_TYPE_PASSIVE_COOLING          = 0x11, ///< Passive cooling
    __DMI_COOLING_DEVICE_TYPE_COUNT
} dmi_cooling_device_type_t;

/**
 * @brief Cooling device type and status details.
 */
dmi_packed_union(dmi_cooling_device_details)
{
    /**
     * @brief Raw value.
     */
    dmi_byte_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Cooling device type.
         *
         * @since SMBIOS 2.2
         */
        dmi_byte_t type : 5;

        /**
         * @brief Cooling device status.
         *
         * @since SMBIOS 2.2
         */
        dmi_byte_t status : 3;
    };
};

dmi_static_assert_value_union(dmi_cooling_device_details);


typedef union dmi_cooling_device_details dmi_cooling_device_details_t;

struct dmi_cooling_device
{
    /**
     * @brief Handle, or instance number, of the temperature probe monitoring
     * this cooling device.
     */
    dmi_handle_t probe_handle;

    /**
     * @brief Reference to the temperature probe monitoring this cooling device.
     */
    dmi_entity_t *probe;

    /**
     * @brief Cooling device type.
     */
    dmi_cooling_device_type_t type;

    /**
     * @brief Cooling device status.
     */
    dmi_status_t status;

    /**
     * @brief Cooling unit group to which this cooling device is associated.
     *
     * Having multiple cooling devices in the same cooling unit implies a
     * redundant configuration. The value is `0` if the cooling device is
     * not a member of a redundant cooling unit. Non-zero values imply
     * redundancy and that at least one other cooling device will be enumerated
     * with the same value.
     */
    unsigned short group;

    /**
     * @brief OEM- or firmware vendor-specific information.
     */
    uint32_t oem_defined;

    /**
     * @brief Nominal value for the cooling device’s rotational speed, in
     * revolutions-per-minute (rpm). If the value is unknown or the cooling
     * device is non-rotating, the field is set to `SHRT_MIN`
     */
    short nominal_speed;

    /**
     * @brief Additional descriptive information about the cooling device or
     * its location.
     */
    const char *description;
};

/**
 * @brief Cooling device entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_cooling_device_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_cooling_device_type_name(dmi_cooling_device_type_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_COOLING_DEVICE_H
