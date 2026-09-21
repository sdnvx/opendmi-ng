//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/memory-controller-internal.h>

//
// Module size is carried as the power of two it is a number of megabytes of.
//
uintmax_t dmi_memory_controller_convert_size(uintmax_t raw)
{
    return (uintmax_t)1 << (raw + 20);
}

//
// Memory the controller supports is what its slots hold when every one of
// them carries a module of the largest size.
//
bool dmi_memory_controller_derive(dmi_entity_t *entity)
{
    dmi_memory_controller_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_CONTROLLER));
    if (info == nullptr)
        return false;

    info->maximum_memory_size = info->maximum_module_size * info->slot_count;

    return true;
}

//
// Modules of a controller are linked by the attributes, and learn the
// controller they belong to here, along with the largest size it supports,
// which the sizes they declare are checked against.
//
bool dmi_memory_controller_link(dmi_entity_t *entity)
{
    dmi_memory_controller_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_CONTROLLER));
    if (info == nullptr)
        return false;

    if (info->modules == nullptr)
        return true;

    for (size_t i = 0; i < info->slot_count; i++) {
        if (info->modules[i] == nullptr)
            continue;

        // Memory module may be left undecoded
        dmi_memory_module_t *module = dmi_entity_info(info->modules[i], DMI_TYPE(MEMORY_MODULE));
        if (module == nullptr)
            continue;

        // Bind memory controller to module
        module->controller = entity;

        // Check module installed size value
        if (module->installed_size.status == DMI_MEMORY_MODULE_SIZE_STATUS_PRESENT) {
            if (module->installed_size.value > info->maximum_module_size)
                module->installed_size.status = DMI_MEMORY_MODULE_SIZE_STATUS_INVALID;
        }

        // Check module enabled size value
        if (module->enabled_size.status == DMI_MEMORY_MODULE_SIZE_STATUS_PRESENT) {
            if (module->enabled_size.value > info->maximum_module_size)
                module->enabled_size.status = DMI_MEMORY_MODULE_SIZE_STATUS_INVALID;
        }
    }

    return true;
}

void dmi_memory_controller_cleanup(dmi_entity_t *entity)
{
    dmi_memory_controller_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_CONTROLLER));
    if (info == nullptr)
        return;

    dmi_free(info->module_handles);
    dmi_free(info->modules);
}
