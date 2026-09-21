//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <inttypes.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/stream.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>
#include <opendmi/entity/memory-device.h>

#include <opendmi/entity/memory-array-internal.h>

//
// Sum of the sizes of the devices of an array, along with their number, which
// both rules of the array are checked against.
//
size_t dmi_memory_array_devices(
        dmi_lint_t         *lint,
        const dmi_entity_t *entity,
        dmi_size_t         *capacity)
{
    dmi_registry_t *registry = dmi_get_registry(dmi_lint_context(lint));
    dmi_registry_iter_t iter;
    dmi_entity_t *device;

    size_t count = 0;

    if (not dmi_registry_iter_init(&iter, registry, nullptr))
        return 0;

    while ((device = dmi_registry_iter_next(&iter)) != nullptr) {
        if (dmi_entity_type(device) != DMI_TYPE_MEMORY_DEVICE)
            continue;

        const dmi_memory_device_t *info = dmi_entity_info(device, DMI_TYPE(MEMORY_DEVICE));

        if ((info == nullptr) or (info->array_handle != dmi_entity_handle(entity)))
            continue;

        count++;

        if ((capacity != nullptr) and (info->size != DMI_SIZE_MAX))
            *capacity += info->size;
    }

    return count;
}
