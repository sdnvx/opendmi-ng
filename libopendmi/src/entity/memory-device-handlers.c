//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/utils.h>

#include <opendmi/entity/memory-device-internal.h>

dmi_size_t dmi_memory_device_size(uint16_t value)
{
    dmi_size_t size = value & 0x7FFFu;

    if (value & 0x8000u)
        size <<= 10; // Granularity is 1 Kb
    else
        size <<= 20; // Granularity is 1 Mb

    return size;
}

dmi_size_t dmi_memory_device_size_ex(uint32_t value)
{
    if (value & 0x80000000u)
        return DMI_SIZE_MAX;

    return (dmi_size_t)(value & 0x7FFFFFFFu) << 20; // Granularity is 1 Mb
}

//
// Conversions the field engine applies, which take the values the way the
// data carries them.
//
bool dmi_memory_device_decode_size(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    return dmi_field_set(field, value, dmi_memory_device_size((uint16_t)data->number));
}

bool dmi_memory_device_decode_size_ex(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    return dmi_field_set(field, value, dmi_memory_device_size_ex((uint32_t)data->number));
}

//
// Sizes are written in megabytes whenever they fit, and in kilobytes
// otherwise, the way the most significant bit of the field tells them apart.
//
bool dmi_memory_device_encode_size(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    uintmax_t size = dmi_field_get(field, value);

    if (((size & 0xFFFFFu) == 0) and ((size >> 20) < 0x7FFFu))
        data->number = size >> 20;
    else
        data->number = 0x8000u | ((size >> 10) & 0x7FFFu);

    return true;
}

bool dmi_memory_device_encode_size_ex(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    uintmax_t size = dmi_field_get(field, value);

    if (size == DMI_SIZE_MAX)
        data->number = 0x80000000u;
    else
        data->number = (size >> 20) & 0x7FFFFFFFu;

    return true;
}
