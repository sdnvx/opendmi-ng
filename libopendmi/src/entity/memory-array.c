//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/memory-array.h>

static bool dmi_memory_array_decode(dmi_entity_t *entity);
static bool dmi_memory_array_link(dmi_entity_t *entity);

static const dmi_name_set_t dmi_memory_array_location_names =
{
    .code  = "memory-array-locations",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_MEMORY_ARRAY_LOCATION_UNSPEC),
        DMI_NAME_OTHER(DMI_MEMORY_ARRAY_LOCATION_OTHER),
        DMI_NAME_UNKNOWN(DMI_MEMORY_ARRAY_LOCATION_UNKNOWN),
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_MOTHERBOARD,
            .code = "motherboard",
            .name = "System board or motherboard"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_ISA,
            .code = "isa",
            .name = "ISA add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_EISA,
            .code = "eisa",
            .name = "EISA add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_PCI,
            .code = "pci",
            .name = "PCI add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_MCA,
            .code = "mca",
            .name = "MCA add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_PCMCIA,
            .code = "pcmcia",
            .name = "PCMCIA add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_PROPRIETARY,
            .code = "proprietary",
            .name = "Proprietary add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_NUBUS,
            .code = "nubus",
            .name = "NuBus"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_PC_98_C20,
            .code = "pc-98-c20",
            .name = "PC-98/C20 add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_PC_98_C24,
            .code = "pc-98-c24",
            .name = "PC-98/C24 add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_PC_98_E,
            .code = "pc-98-e",
            .name = "PC-98/E add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_PC_98_LOCAL_BUS,
            .code = "pc-98-local-bus",
            .name = "PC-98/Local bus add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_CXL,
            .code = "cxl",
            .name = "CXL add-on card"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_memory_array_usage_names =
{
    .code  = "memory-array-usages",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_MEMORY_ARRAY_USAGE_UNSPEC),
        DMI_NAME_OTHER(DMI_MEMORY_ARRAY_USAGE_OTHER),
        DMI_NAME_UNKNOWN(DMI_MEMORY_ARRAY_USAGE_UNKNOWN),
        {
            .id   = DMI_MEMORY_ARRAY_USAGE_SYSTEM,
            .code = "system",
            .name = "System memory"
        },
        {
            .id   = DMI_MEMORY_ARRAY_USAGE_VIDEO,
            .code = "video",
            .name = "Video memory"
        },
        {
            .id   = DMI_MEMORY_ARRAY_USAGE_FLASH,
            .code = "flash",
            .name = "Flash memory"
        },
        {
            .id   = DMI_MEMORY_ARRAY_USAGE_NVRAM,
            .code = "nvram",
            .name = "Non-volatile RAM"
        },
        {
            .id   = DMI_MEMORY_ARRAY_USAGE_CACHE,
            .code = "cache",
            .name = "Cache memory"
        },
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_memory_array_spec =
{
    .code            = "memory-array",
    .name            = "Physical memory array",
    .description     = (const char *[]){
        "This structure describes a collection of memory devices that operate "
        "together to form a memory address space.",
        //
        nullptr
    },
    .type            = DMI_TYPE(MEMORY_ARRAY),
    .minimum_version = DMI_VERSION(2, 1, 0),
    .minimum_length  = 0x0F,
    .decoded_length  = sizeof(dmi_memory_array_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_memory_array_t, location, ENUM, {
            .code    = "location",
            .name    = "Location",
            .unspec  = dmi_value_ptr(DMI_MEMORY_ARRAY_LOCATION_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MEMORY_ARRAY_LOCATION_UNKNOWN),
            .values  = &dmi_memory_array_location_names
        }),
        DMI_ATTRIBUTE(dmi_memory_array_t, usage, ENUM, {
            .code    = "use",
            .name    = "Use",
            .unspec  = dmi_value_ptr(DMI_MEMORY_ARRAY_USAGE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MEMORY_ARRAY_USAGE_UNKNOWN),
            .values  = &dmi_memory_array_usage_names
        }),
        DMI_ATTRIBUTE(dmi_memory_array_t, error_correction, ENUM, {
            .code    = "error-correction",
            .name    = "Memory error correction",
            .unspec  = dmi_value_ptr(DMI_ERROR_CORRECT_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_ERROR_CORRECT_TYPE_UNSPEC),
            .values  = &dmi_error_correct_type_names
        }),
        DMI_ATTRIBUTE(dmi_memory_array_t, maximum_capacity, SIZE, {
            .code    = "maximum-capacity",
            .name    = "Maximum capacity",
            .unknown = dmi_value_ptr(DMI_SIZE_MAX)
        }),
        DMI_ATTRIBUTE(dmi_memory_array_t, error_info_handle, HANDLE, {
            .code    = "error-handle",
            .name    = "Memory error information handle"
        }),
        DMI_ATTRIBUTE(dmi_memory_array_t, device_count, INTEGER, {
            .code    = "device-count",
            .name    = "Number of memory devices"
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_memory_array_decode,
        .link   = dmi_memory_array_link
    }
};

const char *dmi_memory_array_location_name(dmi_memory_array_location_t value)
{
    return dmi_name_lookup(&dmi_memory_array_location_names, (int)value);
}

const char *dmi_memory_array_usage_name(dmi_memory_array_usage_t value)
{
    return dmi_name_lookup(&dmi_memory_array_usage_names, (int)value);
}

static bool dmi_memory_array_decode(dmi_entity_t *entity)
{
    bool status;
    dmi_memory_array_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_ARRAY));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    status =
        dmi_stream_decode(stream, dmi_byte_t, &info->location) and
        dmi_stream_decode(stream, dmi_byte_t, &info->usage) and
        dmi_stream_decode(stream, dmi_byte_t, &info->error_correction);
    if (not status)
        return false;

    dmi_dword_t maximum_capacity = 0;
    if (not dmi_stream_decode(stream, dmi_dword_t, &maximum_capacity))
        return false;

    // Maximum capacity is specified in kilobytes. If it is unknown, or 2 TiB
    // or more, the field value is 0x80000000, and actual capacity is stored
    // in extended maximum capacity field (in bytes).
    bool has_capacity_ex = (maximum_capacity == 0x80000000u);

    if (has_capacity_ex)
        info->maximum_capacity = DMI_SIZE_MAX;
    else
        info->maximum_capacity = (dmi_size_t)maximum_capacity << 10;

    status =
        dmi_stream_decode(stream, dmi_handle_t, &info->error_info_handle) and
        dmi_stream_decode(stream, dmi_word_t, &info->device_count);
    if (not status)
        return false;

    // SMBIOS 2.7 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 7, 0);

    dmi_qword_t maximum_capacity_ex = 0;
    if (not dmi_stream_decode(stream, dmi_qword_t, &maximum_capacity_ex))
        return dmi_entity_incomplete(entity);

    if (has_capacity_ex)
        info->maximum_capacity = maximum_capacity_ex;

    return true;
}

static bool dmi_memory_array_link(dmi_entity_t *entity)
{
    dmi_memory_array_t *info;

    static const dmi_type_t error_types[] = {
        DMI_TYPE(MEMORY_ERROR_32),
        DMI_TYPE(MEMORY_ERROR_64),
        DMI_TYPE_INVALID
    };

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_ARRAY));
    if (info == nullptr)
        return false;

    dmi_registry_t *registry = entity->context->state.registry;

    if ((info->error_info_handle != DMI_HANDLE_INVALID) and
        (info->error_info_handle != DMI_HANDLE_UNSUPPORTED))
    {
        info->error_info = dmi_registry_get_any(registry, info->error_info_handle,
                                                error_types, false);
    }

    return true;
}
