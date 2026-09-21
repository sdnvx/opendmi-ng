//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <inttypes.h>
#include <opendmi/context.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/cache-internal.h>

dmi_size_t dmi_cache_size(uint16_t value)
{
    dmi_size_t size = value & 0x7FFFU;

    if (value & 0x8000U)
        size <<= 16; // Granularity is 64 Kb
    else
        size <<= 10; // Granularity is 1 Kb

    return size;
}

dmi_size_t dmi_cache_size_ex(uint32_t value)
{
    dmi_size_t size = value & 0x7FFFFFFFU;

    if (value & 0x80000000U)
        size <<= 16; // Granularity is 64 Kb
    else
        size <<= 10; // Granularity is 1 Kb

    return size;
}

//
// Levels are counted from zero in the data and from one everywhere else.
//
bool dmi_cache_decode_level(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    return dmi_field_set(field, value, data->number + 1);
}

//
// Sizes are carried in granules, whose width the most significant bit of the
// field says.
//
bool dmi_cache_decode_size(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    return dmi_field_set(field, value, dmi_cache_size((uint16_t)data->number));
}

bool dmi_cache_decode_size_ex(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    return dmi_field_set(field, value, dmi_cache_size_ex((uint32_t)data->number));
}

bool dmi_cache_encode_level(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    uintmax_t level = dmi_field_get(field, value);

    data->number = (level > 0) ? level - 1 : 0;

    return true;
}

//
// Sizes are written in granules of one kibibyte whenever they fit, and of
// sixty-four kibibytes otherwise.
//
bool dmi_cache_encode_size(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    uintmax_t size = dmi_field_get(field, value);

    if (((size & 0x3FFu) == 0) and ((size >> 10) <= 0x7FFFu))
        data->number = size >> 10;
    else
        data->number = 0x8000u | ((size >> 16) & 0x7FFFu);

    return true;
}

bool dmi_cache_encode_size_ex(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    uintmax_t size = dmi_field_get(field, value);

    if (((size & 0x3FFu) == 0) and ((size >> 10) <= 0x7FFFFFFFu))
        data->number = size >> 10;
    else
        data->number = 0x80000000u | ((size >> 16) & 0x7FFFFFFFu);

    return true;
}
