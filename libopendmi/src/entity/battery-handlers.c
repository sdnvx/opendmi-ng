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

//
// SBDS date packs the year counted from 1980, the month and the day into one
// word, and zero stands for no date.
//
bool dmi_battery_decode_sbds_date(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    dmi_unused(field);

    dmi_date_t *date = value;
    uintmax_t   raw  = data->number;

    *date = DMI_DATE_NONE;

    if (raw != 0)
        *date = dmi_date(((raw >> 9) & 0x7Fu) + 1980, (raw >> 5) & 0x0Fu, raw & 0x1Fu);

    return true;
}

bool dmi_battery_encode_sbds_date(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    dmi_unused(field);

    dmi_date_t date = *(const dmi_date_t *)value;

    if (date == DMI_DATE_NONE) {
        data->number = 0;
        return true;
    }

    data->number = (((dmi_date_year(date) - 1980u) & 0x7Fu) << 9) |
                   ((dmi_date_month(date) & 0x0Fu) << 5) |
                   (dmi_date_day(date) & 0x1Fu);

    return true;
}

//
// Manufacture date is the one the string spells, or the SBDS one when the
// string spells none, and the capacity is the design one times its
// multiplier.
//
bool dmi_battery_derive(dmi_entity_t *entity)
{
    dmi_battery_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(PORTABLE_BATTERY));
    if (info == nullptr)
        return false;

    info->manufacture_date = DMI_DATE_NONE;

    if (info->manufacture_date_string != nullptr) {
        info->manufacture_date = dmi_date_parse(info->manufacture_date_string);

        if (info->manufacture_date == DMI_DATE_NONE) {
            dmi_log_warning(dmi_entity_context(entity),
                            "Invalid battery manufacture date format: '%s'",
                            info->manufacture_date_string);
        }
    }

    if (info->manufacture_date == DMI_DATE_NONE)
        info->manufacture_date = info->sbds_manufacture_date;

    info->capacity = (unsigned int)info->design_capacity * info->design_capacity_multiplier;

    return true;
}
