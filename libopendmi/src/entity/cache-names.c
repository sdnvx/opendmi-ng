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

const dmi_name_set_t dmi_cache_type_names =
{
    .code  = "cache-type",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_CACHE_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_CACHE_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_CACHE_TYPE_UNKNOWN),
        {
            .id   = DMI_CACHE_TYPE_INSTRUCTION,
            .code = "instruction",
            .name = "Instruction"
        },
        {
            .id   = DMI_CACHE_TYPE_DATA,
            .code = "data",
            .name = "Data"
        },
        {
            .id   = DMI_CACHE_TYPE_UNIFIED,
            .code = "unified",
            .name = "Unified"
        },
        {}
    })
};

const dmi_name_set_t dmi_cache_mode_names =
{
    .code  = "cache-mode",
    .names = DMI_NAMES({
        {
            .id   = DMI_CACHE_MODE_WRITE_THROUGH,
            .code = "write-through",
            .name = "Write-through"
        },
        {
            .id   = DMI_CACHE_MODE_WRITE_BACK,
            .code = "write-back",
            .name = "Write-back"
        },
        {
            .id   = DMI_CACHE_MODE_VARIABLE,
            .code = "variable",
            .name = "Variable"
        },
        DMI_NAME_UNKNOWN(DMI_CACHE_MODE_UNKNOWN),
        {}
    })
};

const dmi_name_set_t dmi_cache_assoc_names =
{
    .code  = "cache-assoc",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_CACHE_ASSOC_UNSPEC),
        DMI_NAME_OTHER(DMI_CACHE_ASSOC_OTHER),
        DMI_NAME_UNKNOWN(DMI_CACHE_ASSOC_UNKNOWN),
        {
            .id   = DMI_CACHE_ASSOC_DIRECT,
            .code = "direct",
            .name = "Direct mapped"
        },
        {
            .id   = DMI_CACHE_ASSOC_2WAY,
            .code = "2-way",
            .name = "2-way set-associative"
        },
        {
            .id   = DMI_CACHE_ASSOC_4WAY,
            .code = "4-way",
            .name = "4-way set-associative"
        },
        {
            .id   = DMI_CACHE_ASSOC_FULL,
            .code = "full",
            .name = "Fully associative"
        },
        {
            .id   = DMI_CACHE_ASSOC_8WAY,
            .code = "8-way",
            .name = "8-way set-associative"
        },
        {
            .id   = DMI_CACHE_ASSOC_16WAY,
            .code = "16-way",
            .name = "16-way set-associative"
        },
        {
            .id   = DMI_CACHE_ASSOC_12WAY,
            .code = "12-way",
            .name = "12-way set-associative"
        },
        {
            .id   = DMI_CACHE_ASSOC_24WAY,
            .code = "24-way",
            .name = "24-way set-associative"
        },
        {
            .id   = DMI_CACHE_ASSOC_32WAY,
            .code = "32-way",
            .name = "32-way set-associative"
        },
        {
            .id   = DMI_CACHE_ASSOC_48WAY,
            .code = "48-way",
            .name = "48-way set-associative"
        },
        {
            .id   = DMI_CACHE_ASSOC_64WAY,
            .code = "64-way",
            .name = "64-way set-associative"
        },
        {
            .id   = DMI_CACHE_ASSOC_20WAY,
            .code = "20-way",
            .name = "20-way set-associative"
        },
        {}
    })
};

const dmi_name_set_t dmi_cache_location_names =
{
    .code  = "cache-location",
    .names = DMI_NAMES({
        {
            .id   = DMI_CACHE_LOCATION_INTERNAL,
            .code = "internal",
            .name = "Internal"
        },
        {
            .id   = DMI_CACHE_LOCATION_EXTERNAL,
            .code = "external",
            .name = "External"
        },
        DMI_NAME_RESERVED(DMI_CACHE_LOCATION_RESERVED),
        DMI_NAME_UNKNOWN(DMI_CACHE_LOCATION_UNKNOWN),
        {}
    })
};

const dmi_name_set_t dmi_cache_sram_type_names =
{
    .code  = "cache-sram-type",
    .names = DMI_NAMES({
        DMI_NAME_OTHER(0),
        DMI_NAME_UNKNOWN(1),
        {
            .id   = 2,
            .code = "non-burst",
            .name = "Non-burst"
        },
        {
            .id   = 3,
            .code = "burst",
            .name = "Burst"
        },
        {
            .id   = 4,
            .code = "pipeline-burst",
            .name = "Pipeline burst"
        },
        {
            .id   = 5,
            .code = "synchronous",
            .name = "Synchronous"
        },
        {
            .id   = 6,
            .code = "asynchronous",
            .name = "Asynchronous"
        },
        {}
    })
};

const char *dmi_cache_type_name(dmi_cache_type_t value)
{
    return dmi_name_lookup(&dmi_cache_type_names, (int)value);
}

const char *dmi_cache_mode_name(dmi_cache_mode_t value)
{
    return dmi_name_lookup(&dmi_cache_mode_names, (int)value);
}

const char *dmi_cache_assoc_name(dmi_cache_assoc_t value)
{
    return dmi_name_lookup(&dmi_cache_assoc_names, (int)value);
}

const char *dmi_cache_location_name(dmi_cache_location_t value)
{
    return dmi_name_lookup(&dmi_cache_location_names, (int)value);
}
