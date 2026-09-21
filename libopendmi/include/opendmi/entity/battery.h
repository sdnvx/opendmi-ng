//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_BATTERY_H
#define OPENDMI_ENTITY_BATTERY_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/utils/datetime.h>

typedef struct dmi_battery dmi_battery_t;

/**
 * @brief Battery chemistry types.
 */
typedef enum dmi_battery_chemistry
{
    DMI_BATTERY_CHEMISTRY_UNSPEC    = 0x00, ///< Unspecified
    DMI_BATTERY_CHEMISTRY_OTHER     = 0x01, ///< Other
    DMI_BATTERY_CHEMISTRY_UNKNOWN   = 0x02, ///< Unknown
    DMI_BATTERY_CHEMISTRY_LEAD_ACID = 0x03, ///< Lead acid
    DMI_BATTERY_CHEMISTRY_NI_CD     = 0x04, ///< Nickel cadmium
    DMI_BATTERY_CHEMISTRY_NI_MH     = 0x05, ///< Nickel metal hyrdide
    DMI_BATTERY_CHEMISTRY_LI_ION    = 0x06, ///< Lithium-ion
    DMI_BATTERY_CHEMISTRY_ZN_AIR    = 0x07, ///< Zinc air
    DMI_BATTERY_CHEMISTRY_LI_PO     = 0x08, ///< Lithium polymer
    __DMI_BATTERY_CHEMISTRY_COUNT
} dmi_battery_chemistry_t;

/**
 * @brief Portable battery structure.
 *
 * This structure describes the attributes of the portable battery or batteries
 * for the system. The structure contains the static attributes for the group.
 * Each structure describes attributes for a single battery pack.
 *
 * @since SMBIOS 2.1
 */
struct dmi_battery
{
    /**
     * @brief String that identifies the location of the battery. Example: "in
     * the back, on the left-hand side".
     */
    const char *location;

    /**
     * @brief String that names the company that manufactured the battery.
     */
    const char *vendor;

    /**
     * @brief The date on which the battery was manufactured: the one the
     * string of the structure spells, or the one of the SBDS field when the
     * string spells none.
     */
    dmi_date_t manufacture_date;

    /**
     * @brief String that holds the manufacture date as the structure carries
     * it, which is expected to read as `MM/DD/YY` or `MM/DD/YYYY`.
     */
    const char *manufacture_date_string;

    /**
     * @brief Number of the string that contains the serial number for the
     * battery.
     */
    const char *serial_number;

    /**
     * @brief String that names the battery device. Example: "DR-36".
     */
    const char *name;

    /**
     * @brief The battery chemistry.
     */
    dmi_battery_chemistry_t chemistry;

    /**
     * @brief Design capacity of the battery in mWatt-hours, which is the
     * design capacity the structure carries times its multiplier. If the
     * value is unknown, the field contains `0`.
     */
    unsigned int capacity;

    /**
     * @brief Design capacity as the structure carries it, in the units of
     * `design_capacity_multiplier` mWatt-hours.
     */
    unsigned short design_capacity;

    /**
     * @brief Design voltage of the battery in mVolts. If the value is unknown,
     * the field contains `0`.
     */
    unsigned short voltage;

    /**
     * @brief String that contains the Smart Battery Data Specification version
     * number supported by this battery. If the battery does not support the
     * function, no string is supplied.
     */
    const char *sbds_version;

    /**
     * @brief Maximum error (as a percentage in the range 0 to 100) in the
     * Watt-hour data reported by the battery, indicating an upper bound on how
     * much additional energy the battery might have above the energy it
     * reports having. If the value is unknown, the field contains `USHRT_MAX`.
     */
    unsigned short maximum_error;

    /**
     * @brief 16-bit value that identifies the battery’s serial number.
     *
     * This value, when combined with the manufacturer, device name, and
     * manufacture date, uniquely identifies the battery. The serial number
     * field must be set to `0` (no string) for this field to be valid.
     */
    uint16_t sbds_serial_number;

    /**
     * @brief Date the battery was manufactured on, in the packed format of
     * the Smart Battery Data Specification, which is used when the
     * manufacture date string is not set.
     */
    dmi_date_t sbds_manufacture_date;

    /**
     * @brief String that identifies the battery chemistry (for example,
     * "PbAc"). The device chemistry field must be set to `0x02` (Unknown) for
     * this field to be valid.
     */
    const char *sbds_chemistry;

    /**
     * @brief Multiplication factor of the design capacity, which is `1` for
     * the structures carrying none.
     */
    uint8_t design_capacity_multiplier;

    /**
     * @brief Contains OEM- or firmware vendor-specific information.
     */
    uint32_t oem_defined;
};

/**
 * @brief Portable battery entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_battery_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_battery_chemistry_name(dmi_battery_chemistry_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_BATTERY_H
