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
uintmax_t dmi_cache_convert_level(uintmax_t raw)
{
    return raw + 1;
}

//
// Sizes are carried in granules, whose width the most significant bit of the
// field says.
//
uintmax_t dmi_cache_convert_size(uintmax_t raw)
{
    return dmi_cache_size((uint16_t)raw);
}

uintmax_t dmi_cache_convert_size_ex(uintmax_t raw)
{
    return dmi_cache_size_ex((uint32_t)raw);
}
