//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/cache.h>

static bool dmi_cache_decode(dmi_entity_t *entity);

static const dmi_name_set_t dmi_cache_type_names =
{
    .code  = "cache-type",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_cache_mode_names =
{
    .code  = "cache-mode",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_cache_assoc_names =
{
    .code  = "cache-assoc",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_cache_location_names =
{
    .code  = "cache-location",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_cache_sram_type_names =
{
    .code  = "cache-sram-type",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_cache_spec =
{
    .code            = "cache",
    .name            = "Cache information",
    .description     = (const char *[]){
        "The information in this structure defines the attributes of CPU cache "
        "device in the system. One structure is specified for each such "
        "device, whether the device is internal to or external to the CPU "
        "module. Cache modules can be associated with a processor structure in "
        "one or two ways depending on the SMBIOS version.",
        //
        nullptr
    },
    .type            = DMI_TYPE(CACHE),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x0F,
    .decoded_length  = sizeof(dmi_cache_t),
    .attributes      = (const dmi_attribute_t[]) {
        DMI_ATTRIBUTE(dmi_cache_t, socket_designator, STRING, {
            .code    = "socket-designator",
            .name    = "Socket designator"
        }),
        DMI_ATTRIBUTE(dmi_cache_t, level, INTEGER, {
            .code    = "level",
            .name    = "Cache level"
        }),
        DMI_ATTRIBUTE(dmi_cache_t, socketed, BOOL, {
            .code    = "socketed",
            .name    = "Socketed",
        }),
        DMI_ATTRIBUTE(dmi_cache_t, location, ENUM, {
            .code    = "location",
            .name    = "Location",
            .unknown = dmi_value_ptr(DMI_CACHE_LOCATION_UNKNOWN),
            .values  = &dmi_cache_location_names
        }),
        DMI_ATTRIBUTE(dmi_cache_t, enabled, BOOL, {
            .code    = "enabled",
            .name    = "Enabled"
        }),
        DMI_ATTRIBUTE(dmi_cache_t, mode, ENUM, {
            .code    = "mode",
            .name    = "Operational mode",
            .unknown = dmi_value_ptr(DMI_CACHE_MODE_UNKNOWN),
            .values  = &dmi_cache_mode_names
        }),
        DMI_ATTRIBUTE(dmi_cache_t, maximum_size, SIZE, {
            .code    = "maximum-size",
            .name    = "Maximum cache size"
        }),
        DMI_ATTRIBUTE(dmi_cache_t, installed_size, SIZE, {
            .code    = "installed-size",
            .name    = "Installed cache size"
        }),
        DMI_ATTRIBUTE(dmi_cache_t, supported_sram, SET, {
            .code    = "supported-sram",
            .name    = "Supported SRAM type",
            .values  = &dmi_cache_sram_type_names
        }),
        DMI_ATTRIBUTE(dmi_cache_t, current_sram, SET, {
            .code    = "current-sram",
            .name    = "Current SRAM type",
            .values  = &dmi_cache_sram_type_names
        }),
        DMI_ATTRIBUTE(dmi_cache_t, speed, INTEGER, {
            .code    = "speed",
            .name    = "Cache speed",
            .unit    = DMI_UNIT_NANOSECOND,
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_cache_t, error_correction, ENUM, {
            .code    = "error-correction",
            .name    = "Error correction type",
            .unspec  = dmi_value_ptr(DMI_ERROR_CORRECT_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_ERROR_CORRECT_TYPE_UNKNOWN),
            .values  = &dmi_error_correct_type_names,
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_cache_t, type, ENUM, {
            .code    = "type",
            .name    = "System cache type",
            .unspec  = dmi_value_ptr(DMI_CACHE_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_CACHE_TYPE_UNKNOWN),
            .values  = &dmi_cache_type_names,
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_cache_t, associativity, ENUM, {
            .code    = "associativity",
            .name    = "Associativity",
            .unspec  = dmi_value_ptr(DMI_CACHE_ASSOC_UNSPEC),
            .unknown = dmi_value_ptr(DMI_CACHE_ASSOC_UNKNOWN),
            .values  = &dmi_cache_assoc_names,
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_cache_decode
    }
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

static bool dmi_cache_decode(dmi_entity_t *entity)
{
    dmi_cache_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(CACHE));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    // SMBIOS 2.0 fields
    dmi_word_t config_value = 0;
    dmi_word_t maximum_size = 0;
    dmi_word_t installed_size = 0;

    bool status =
        dmi_stream_decode_str(stream, &info->socket_designator) and
        dmi_stream_decode(stream, dmi_word_t, &config_value) and
        dmi_stream_decode(stream, dmi_word_t, &maximum_size) and
        dmi_stream_decode(stream, dmi_word_t, &installed_size) and
        dmi_stream_decode(stream, dmi_word_t, &info->supported_sram.__value) and
        dmi_stream_decode(stream, dmi_word_t, &info->current_sram.__value);
    if (not status)
        return false;

    dmi_cache_config_t config = {
        .__value = config_value
    };

    info->level          = config.level + 1;
    info->mode           = config.mode;
    info->location       = config.location;
    info->socketed       = config.socketed;
    info->enabled        = config.enabled;
    info->maximum_size   = dmi_cache_size(maximum_size);
    info->installed_size = dmi_cache_size(installed_size);

    // SMBIOS 2.1 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 1, 0);

    status =
        dmi_stream_decode(stream, dmi_byte_t, &info->speed) and
        dmi_stream_decode(stream, dmi_byte_t, &info->error_correction) and
        dmi_stream_decode(stream, dmi_byte_t, &info->type) and
        dmi_stream_decode(stream, dmi_byte_t, &info->associativity);
    if (not status)
        return dmi_entity_incomplete(entity);

    // SMBIOS 3.1 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(3, 1, 0);

    // Actual sizes are stored in extended fields
    dmi_dword_t maximum_size_ex = 0;
    dmi_dword_t installed_size_ex = 0;

    bool has_maximum_size_ex = dmi_stream_decode(stream, dmi_dword_t, &maximum_size_ex);

    status =
        has_maximum_size_ex and
        dmi_stream_decode(stream, dmi_dword_t, &installed_size_ex);

    if (has_maximum_size_ex and (maximum_size == 0xFFFFu))
        info->maximum_size = dmi_cache_size_ex(maximum_size_ex);
    if (status and (installed_size == 0xFFFFu))
        info->installed_size = dmi_cache_size_ex(installed_size_ex);

    if (not status)
        return dmi_entity_incomplete(entity);

    return true;
}
