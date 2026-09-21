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

bool dmi_memory_device_link(dmi_entity_t *entity)
{
    dmi_memory_device_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_DEVICE));
    if (info == nullptr)
        return false;

    dmi_context_t  *context  = dmi_entity_context(entity);
    dmi_registry_t *registry = dmi_get_registry(context);

    static const dmi_type_t error_types[] = {
        DMI_TYPE(MEMORY_ERROR_32),
        DMI_TYPE(MEMORY_ERROR_64),
        DMI_TYPE_INVALID
    };

    bool success = true;
    if (not dmi_registry_resolve(registry, info->array_handle, DMI_TYPE(MEMORY_ARRAY), &info->array))
        success = false;
    if (not dmi_registry_resolve_any(registry, info->error_info_handle, error_types, &info->error_info))
        success = false;

    return success;
}
