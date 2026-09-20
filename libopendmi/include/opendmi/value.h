//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_VALUE_H
#define OPENDMI_VALUE_H

#pragma once

#include <opendmi/defs.h>
#include <opendmi/utils/name.h>

/**
 * @brief Measurement units of attribute values.
 */
typedef enum dmi_unit
{
    DMI_UNIT_NONE,           ///< Value has no measurement units

    DMI_UNIT_HOUR,           ///< Hours
    DMI_UNIT_MINUTE,         ///< Minutes
    DMI_UNIT_SECOND,         ///< Seconds
    DMI_UNIT_MILLISECOND,    ///< Milliseconds
    DMI_UNIT_MICROSECOND,    ///< Microseconds
    DMI_UNIT_NANOSECOND,     ///< Nanoseconds

    DMI_UNIT_AMPERE,         ///< Amperes
    DMI_UNIT_MILLIAMPERE,    ///< Milliamperes
    DMI_UNIT_VOLT,           ///< Volts
    DMI_UNIT_MILLIVOLT,      ///< Millivolts
    DMI_UNIT_WATT,           ///< Watts
    DMI_UNIT_MILLIWATT,      ///< Milliwatts
    DMI_UNIT_WATT_HOUR,      ///< Watt-hours

    DMI_UNIT_CELSIUS,        ///< Degrees Celsius

    DMI_UNIT_BIT,            ///< Bits
    DMI_UNIT_BYTE,           ///< Bytes

    DMI_UNIT_KILOBYTE,       ///< Kilobytes, 1000 bytes
    DMI_UNIT_MEGABYTE,       ///< Megabytes, 1000 kilobytes
    DMI_UNIT_GIGABYTE,       ///< Gigabytes, 1000 megabytes

    DMI_UNIT_KIBIBYTE,       ///< Kibibytes, 1024 bytes
    DMI_UNIT_MEBIBYTE,       ///< Mebibytes, 1024 kibibytes
    DMI_UNIT_GIBIBYTE,       ///< Gibibytes, 1024 mebibytes
    DMI_UNIT_TEBIBYTE,       ///< Tebibytes, 1024 gibibytes
    DMI_UNIT_PEBIBYTE,       ///< Pebibytes, 1024 tebibytes
    DMI_UNIT_EXBIBYTE,       ///< Exbibytes, 1024 pebibytes
    DMI_UNIT_ZEBIBYTE,       ///< Zebibytes, 1024 exbibytes
    DMI_UNIT_YOBIBYTE,       ///< Yobibytes, 1024 zebibytes
    DMI_UNIT_ROBIBYTE,       ///< Robibytes, 1024 yobibytes
    DMI_UNIT_QUEBIBYTE,      ///< Quebibytes, 1024 robibytes

    DMI_UNIT_BAUD,           ///< Symbols per second
    DMI_UNIT_MEGAXA_SECOND,  ///< Megatransfers per second
    DMI_UNIT_KILOBIT_SECOND, ///< Kilobits per second
    DMI_UNIT_MEGABIT_SECOND, ///< Megabits per second
    DMI_UNIT_GIGABIT_SECOND, ///< Gigabits per second

    DMI_UNIT_PERCENT,        ///< Percents
    DMI_UNIT_REVOLUTION,     ///< Revolutions per minute
    DMI_UNIT_RACK,           ///< Rack units
    DMI_UNIT_MHZ,            ///< Megahertz

    DMI_UNIT_MILLIMETER      ///< Millimeters
} dmi_unit_t;

__BEGIN_DECLS

/**
 * @brief Names of measurement units.
 */
extern __dmi_api const dmi_name_set_t dmi_unit_names;

/**
 * @brief Names of boolean values.
 */
extern __dmi_api const dmi_name_set_t dmi_bool_names;

__END_DECLS

#endif // !OPENDMI_VALUE_H
