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
// Manufacture date is written as a string, which the batteries carrying it in
// the packed SBDS field leave unset.
//
bool dmi_battery_decode_date(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value)
{
    dmi_unused(field);

    const char *text = nullptr;

    if (not dmi_stream_decode_str(dmi_entity_stream(entity), &text))
        return false;

    dmi_date_t *date = value;

    *date = DMI_DATE_NONE;

    if (text == nullptr)
        return true;

    *date = dmi_date_parse(text);

    if (*date == DMI_DATE_NONE) {
        dmi_log_warning(dmi_entity_context(entity),
                        "Invalid battery manufacture date format: '%s'", text);
    }

    return true;
}

//
// SBDS date packs the year counted from 1980, the month and the day into one
// word, and is used only by the batteries whose date string is not there.
//
bool dmi_battery_decode_sbds_date(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value)
{
    dmi_unused(field);

    dmi_word_t raw = 0;

    if (not dmi_stream_decode(dmi_entity_stream(entity), dmi_word_t, &raw))
        return false;

    dmi_date_t *date = value;

    if ((*date != DMI_DATE_NONE) or (raw == 0))
        return true;

    *date = dmi_date(((raw >> 9) & 0x7Fu) + 1980, (raw >> 5) & 0x0Fu, raw & 0x1Fu);

    return true;
}

//
// Capacity is carried in the units the factor names, which is how a battery
// too large for a word declares how much it holds.
//
bool dmi_battery_decode_capacity_factor(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value)
{
    dmi_unused(field);

    dmi_byte_t factor = 0;

    if (not dmi_stream_decode(dmi_entity_stream(entity), dmi_byte_t, &factor))
        return false;

    unsigned int *capacity = value;

    *capacity *= factor;

    return true;
}
