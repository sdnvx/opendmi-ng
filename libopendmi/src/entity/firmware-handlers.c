//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
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
                        "Invalid firmware release date format: '%s'", text);
    }

    return true;
}

//
// ROM size is carried as the number of the granules it takes.
//
uintmax_t dmi_firmware_convert_rom_size(uintmax_t raw)
{
    return dmi_firmware_rom_size((dmi_byte_t)raw);
}

uintmax_t dmi_firmware_convert_rom_size_ex(uintmax_t raw)
{
    return dmi_firmware_rom_size_ex((dmi_word_t)raw);
}

//
// Versions are one byte of major and one of minor, and the major number of
// 0xFF says that the platform carries no version at all.
//
uintmax_t dmi_firmware_convert_version(uintmax_t raw)
{
    unsigned int major = (unsigned int)(raw & 0xFFu);

    if (major == 0xFFu)
        return DMI_VERSION_NONE;

    return dmi_version(major, (unsigned int)((raw >> 8) & 0xFFu), 0);
}
