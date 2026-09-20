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
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/battery.h>

static bool dmi_battery_decode(dmi_entity_t *entity);

static const dmi_name_set_t dmi_battery_chemistry_names =
{
    .code  = "battery-chemistry",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_BATTERY_CHEMISTRY_UNSPEC),
        DMI_NAME_OTHER(DMI_BATTERY_CHEMISTRY_OTHER),
        DMI_NAME_UNKNOWN(DMI_BATTERY_CHEMISTRY_UNKNOWN),
        {
            .id   = DMI_BATTERY_CHEMISTRY_LEAD_ACID,
            .code = "lead-acid",
            .name = "Lead acid"
        },
        {
            .id   = DMI_BATTERY_CHEMISTRY_NI_CD,
            .code = "ni-cd",
            .name = "Nickel cadmium"
        },
        {
            .id   = DMI_BATTERY_CHEMISTRY_NI_MH,
            .code = "ni-mh",
            .name = "Nickel metal hyrdide"
        },
        {
            .id   = DMI_BATTERY_CHEMISTRY_LI_ION,
            .code = "li-ion",
            .name = "Lithium-ion"
        },
        {
            .id   = DMI_BATTERY_CHEMISTRY_ZN_AIR,
            .code = "zn-air",
            .name = "Zinc air"
        },
        {
            .id   = DMI_BATTERY_CHEMISTRY_LI_PO,
            .code = "li-po",
            .name = "Lithium polymer"
        },
        DMI_NAME_NULL
    }
};

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
    .minimum_version = DMI_VERSION(2, 1, 0),
    .minimum_length  = 0x10,
    .decoded_length  = sizeof(dmi_battery_t),
    .attributes      = (const dmi_attribute_t[]){
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
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_battery_decode
    }
};

const char *dmi_battery_chemistry_name(dmi_battery_chemistry_t value)
{
    return dmi_name_lookup(&dmi_battery_chemistry_names, (int)value);
}

static bool dmi_battery_decode(dmi_entity_t *entity)
{
    dmi_battery_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(PORTABLE_BATTERY));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t  *stream  = dmi_entity_stream(entity);

    // SMBIOS 2.1 fields
    const char *manufacture_date = nullptr;
    dmi_word_t capacity = 0;
    dmi_byte_t maximum_error = 0;

    bool status =
        dmi_stream_decode_str(stream, &info->location) and
        dmi_stream_decode_str(stream, &info->vendor) and
        dmi_stream_decode_str(stream, &manufacture_date) and
        dmi_stream_decode_str(stream, &info->serial_number) and
        dmi_stream_decode_str(stream, &info->name) and
        dmi_stream_decode(stream, dmi_byte_t, &info->chemistry) and
        dmi_stream_decode(stream, dmi_word_t, &capacity) and
        dmi_stream_decode(stream, dmi_word_t, &info->voltage) and
        dmi_stream_decode_str(stream, &info->sbds_version) and
        dmi_stream_decode(stream, dmi_byte_t, &maximum_error);
    if (not status)
        return false;

    info->capacity      = capacity;
    info->maximum_error = (maximum_error != 0xFFu) ? maximum_error : USHRT_MAX;

    if (manufacture_date != nullptr) {
        info->manufacture_date = dmi_date_parse(manufacture_date);
        if (info->manufacture_date == DMI_DATE_NONE) {
            dmi_log_warning(context,
                            "Invalid battery manufacture date format: '%s'", manufacture_date);
        }
    }

    // SMBIOS 2.2 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 2, 0);

    // Missing values do not affect decoded date and capacity
    dmi_word_t sbds_manufacture_date = 0;
    dmi_byte_t capacity_factor = 1;

    status =
        dmi_stream_decode(stream, dmi_word_t, &info->sbds_serial_number) and
        dmi_stream_decode(stream, dmi_word_t, &sbds_manufacture_date) and
        dmi_stream_decode_str(stream, &info->sbds_chemistry) and
        dmi_stream_decode(stream, dmi_byte_t, &capacity_factor) and
        dmi_stream_decode(stream, dmi_dword_t, &info->oem_defined);

    // Manufacture date in SBDS format is used if the string is not set
    if ((manufacture_date == nullptr) and (sbds_manufacture_date != 0)) {
        info->manufacture_date = dmi_date(
            ((sbds_manufacture_date >> 9) & 0x7Fu) + 1980,
            (sbds_manufacture_date >> 5) & 0x0Fu,
            sbds_manufacture_date & 0x1Fu
        );
    }

    info->capacity *= capacity_factor;

    if (not status)
        return dmi_entity_incomplete(entity);

    return true;
}
