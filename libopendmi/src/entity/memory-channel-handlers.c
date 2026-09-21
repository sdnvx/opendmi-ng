//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>
#include <opendmi/entity/memory-device.h>

#include <opendmi/entity/memory-channel-internal.h>

bool dmi_memory_channel_link(dmi_entity_t *entity)
{
    dmi_memory_channel_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_CHANNEL));
    if (info == nullptr)
        return false;

    dmi_context_t  *context  = dmi_entity_context(entity);
    dmi_registry_t *registry = dmi_get_registry(context);

    bool success = true;
    for (size_t i = 0; i < info->device_count; i++) {
        dmi_entity_t *device;

        if (not dmi_registry_resolve(registry, info->devices[i].handle, DMI_TYPE(MEMORY_DEVICE), &device)) {
            success = false;
            continue;
        }

        info->devices[i].device = device;
        if (device == nullptr)
            continue;

        // Memory device may be left undecoded
        dmi_memory_device_t *device_info = dmi_entity_info(device, DMI_TYPE(MEMORY_DEVICE));
        if (device_info != nullptr)
            device_info->channel = entity;
    }

    return success;
}

void dmi_memory_channel_cleanup(dmi_entity_t *entity)
{
    dmi_memory_channel_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_CHANNEL));
    if (info == nullptr)
        return;

    dmi_free(info->devices);
}
