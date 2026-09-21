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
uintmax_t dmi_memory_device_convert_size(uintmax_t raw)
{
    return dmi_memory_device_size((uint16_t)raw);
}

uintmax_t dmi_memory_device_convert_size_ex(uintmax_t raw)
{
    return dmi_memory_device_size_ex((uint32_t)raw);
}
