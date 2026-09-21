//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <stdio.h>
#include <string.h>
#include <opendmi/stream.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/firmware-internal.h>

dmi_size_t dmi_firmware_rom_size(dmi_byte_t value)
{
    return ((dmi_size_t)value + 1) << 16;
}

dmi_size_t dmi_firmware_rom_size_ex(dmi_word_t value)
{
    dmi_size_t size = (dmi_size_t)(value & 0x3FFF);
    dmi_word_t scale = (value & 0xC000) >> 14;

    if (scale == 0)
        size <<= 20;
    else if (scale == 1)
        size <<= 30;
    else
        return 0;

    return size;
}

//
// Release date is written as a string, and the structures whose string is
// missing or malformed carry no date at all.
//
bool dmi_firmware_decode_date(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    dmi_unused(field);

    dmi_date_t *date = value;

    *date = DMI_DATE_NONE;

    if (data->string == nullptr)
        return true;

    *date = dmi_date_parse(data->string);

    if ((*date == DMI_DATE_NONE) and (data->entity != nullptr)) {
        dmi_log_warning(dmi_entity_context(data->entity),
                        "Invalid firmware release date format: '%s'", data->string);
    }

    return true;
}

//
// ROM size is carried as the number of the granules it takes.
//
bool dmi_firmware_decode_rom_size(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    return dmi_field_set(field, value, dmi_firmware_rom_size((dmi_byte_t)data->number));
}

bool dmi_firmware_decode_rom_size_ex(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    return dmi_field_set(field, value, dmi_firmware_rom_size_ex((dmi_word_t)data->number));
}

//
// Versions are one byte of major and one of minor, and the major number of
// 0xFF says that the platform carries no version at all.
//
bool dmi_firmware_decode_version(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    unsigned int major = (unsigned int)(data->number & 0xFFu);

    if (major == 0xFFu)
        return dmi_field_set(field, value, DMI_VERSION_NONE);

    return dmi_field_set(field, value, dmi_version(major, (unsigned int)((data->number >> 8) & 0xFFu), 0));
}

//
// Release date is written the way the specification spells it, and no date
// is written as no string.
//
bool dmi_firmware_encode_date(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    dmi_unused(field);

    dmi_date_t date = *(const dmi_date_t *)value;

    if (date == DMI_DATE_NONE)
        return true;

    snprintf((char *)data->buffer, sizeof(data->buffer), "%02u/%02u/%04u",
             dmi_date_month(date) % 100u, dmi_date_day(date) % 100u, dmi_date_year(date) % 10000u);

    data->string = (const char *)data->buffer;

    return true;
}

bool dmi_firmware_encode_rom_size(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    uintmax_t size = dmi_field_get(field, value);

    data->number = (size >= ((uintmax_t)1 << 16)) ? (size >> 16) - 1 : 0;

    return true;
}

//
// Extended size is written in megabytes whenever it fits, and in gigabytes
// otherwise, the way the two most significant bits of the field tell them
// apart.
//
bool dmi_firmware_encode_rom_size_ex(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    uintmax_t size = dmi_field_get(field, value);

    if (((size & 0xFFFFFu) == 0) and ((size >> 20) <= 0x3FFFu))
        data->number = size >> 20;
    else
        data->number = 0x4000u | ((size >> 30) & 0x3FFFu);

    return true;
}

//
// Platform which carries no version says so by the major number of 0xFF.
//
bool dmi_firmware_encode_version(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    dmi_version_t version = (dmi_version_t)dmi_field_get(field, value);

    if (version == DMI_VERSION_NONE)
        data->number = 0xFFFFu;
    else
        data->number = dmi_version_major(version) | (dmi_version_minor(version) << 8);

    return true;
}
