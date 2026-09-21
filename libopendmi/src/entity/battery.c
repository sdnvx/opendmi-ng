//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/value.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/battery-internal.h>

const dmi_entity_spec_t dmi_battery_spec =
{
    .code            = "portable-battery",
    .name            = "Portable battery",
    .description     = (const char *[]){
        "This structure describes the attributes of the portable battery or "
        "batteries for the system. The structure contains the static "
        "attributes for the group. Each structure describes attributes for a "
        "single battery pack.",
        //
        nullptr
    },
    .type            = DMI_TYPE(PORTABLE_BATTERY),
    .params = {
        .minimum_version = DMI_VERSION(2, 1, 0),
        .minimum_length  = 0x10,
        .decoded_length  = sizeof(dmi_battery_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_battery_t, location, STRING),
        DMI_FIELD(dmi_battery_t, vendor,   STRING),

        DMI_FIELD_CUSTOM(dmi_battery_t, manufacture_date,
                         .decode = dmi_battery_decode_date),

        DMI_FIELD(dmi_battery_t, serial_number, STRING),
        DMI_FIELD(dmi_battery_t, name,          STRING),
        DMI_FIELD(dmi_battery_t, chemistry,     BYTE),
        DMI_FIELD(dmi_battery_t, capacity,      WORD),
        DMI_FIELD(dmi_battery_t, voltage,       WORD),
        DMI_FIELD(dmi_battery_t, sbds_version,  STRING),
        DMI_FIELD(dmi_battery_t, maximum_error, BYTE, .unknown_raw = 0xFFu),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 2, 0)),
        DMI_FIELD(dmi_battery_t, sbds_serial_number, WORD),

        // Batteries whose date is not written as a string carry it packed
        DMI_FIELD_CUSTOM(dmi_battery_t, manufacture_date,
                         .decode = dmi_battery_decode_sbds_date),

        DMI_FIELD(dmi_battery_t, sbds_chemistry, STRING),

        // Capacity is carried in the units the factor names
        DMI_FIELD_CUSTOM(dmi_battery_t, capacity,
                         .decode = dmi_battery_decode_capacity_factor),

        DMI_FIELD(dmi_battery_t, oem_defined, DWORD),
        {}
    }),

    .attributes      = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_battery_t, location, STRING, {
            .code    = "location",
            .name    = "Location"
        }),
        DMI_ATTRIBUTE(dmi_battery_t, vendor, STRING, {
            .code    = "vendor",
            .name    = "Vendor"
        }),
        DMI_ATTRIBUTE(dmi_battery_t, manufacture_date, DATE, {
            .code    = "manufacture-date",
            .name    = "Manufacture date",
            .unspec  = dmi_value_ptr(DMI_DATE_NONE)
        }),
        DMI_ATTRIBUTE(dmi_battery_t, serial_number, STRING, {
            .code    = "serial-number",
            .name    = "Serial number"
        }),
        DMI_ATTRIBUTE(dmi_battery_t, name, STRING, {
            .code    = "name",
            .name    = "Name"
        }),
        DMI_ATTRIBUTE(dmi_battery_t, chemistry, ENUM, {
            .code    = "chemistry",
            .name    = "Chemistry",
            .unspec  = dmi_value_ptr(DMI_BATTERY_CHEMISTRY_UNSPEC),
            .unknown = dmi_value_ptr(DMI_BATTERY_CHEMISTRY_UNKNOWN),
            .values  = &dmi_battery_chemistry_names,
        }),
        DMI_ATTRIBUTE(dmi_battery_t, capacity, DECIMAL, {
            .code    = "capacity",
            .name    = "Design capacity",
            .unit    = DMI_UNIT_WATT_HOUR,
            .scale   = 3,
            .unknown = dmi_value_ptr((unsigned int)0)
        }),
        DMI_ATTRIBUTE(dmi_battery_t, voltage, DECIMAL, {
            .code    = "voltage",
            .name    = "Design voltage",
            .unit    = DMI_UNIT_VOLT,
            .scale   = 3,
            .unknown = dmi_value_ptr((unsigned short)0)
        }),
        DMI_ATTRIBUTE(dmi_battery_t, sbds_version, STRING, {
            .code    = "sbds-version",
            .name    = "SBDS version"
        }),
        DMI_ATTRIBUTE(dmi_battery_t, maximum_error, INTEGER, {
            .code    = "maximum-error",
            .name    = "Maximum error",
            .unit    = DMI_UNIT_PERCENT,
            .unknown = dmi_value_ptr((unsigned short)USHRT_MAX)
        }),
        DMI_ATTRIBUTE(dmi_battery_t, sbds_serial_number, INTEGER, {
            .code    = "sbds-serial-number",
            .name    = "SBDS serial number",
            .level   = DMI_VERSION(2, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_battery_t, sbds_chemistry, STRING, {
            .code    = "sbds-chemistry",
            .name    = "SBDS chemistry",
            .level   = DMI_VERSION(2, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_battery_t, oem_defined, INTEGER, {
            .code    = "oem-defined",
            .name    = "OEM-defined",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX,
            .level   = DMI_VERSION(2, 2, 0)
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("portable-battery.sbds", dmi_battery_lint_sbds, {
            .name              = "Battery carries either its own values or the SBDS ones",
            .severity          = DMI_LINT_SEVERITY_NOTE,
            .producer_severity = DMI_LINT_SEVERITY_WARNING
        }),
        {}
    })
};
