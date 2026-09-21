//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/baseboard-internal.h>

bool dmi_baseboard_link(dmi_entity_t *entity)
{
    dmi_baseboard_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(BASEBOARD));
    if (info == nullptr)
        return false;

    dmi_context_t  *context  = dmi_entity_context(entity);
    dmi_registry_t *registry = dmi_get_registry(context);
    bool success = true;

    if (not dmi_registry_resolve(registry, info->chassis_handle, DMI_TYPE(CHASSIS), &info->chassis))
        success = false;

    if (info->object_count > 0) {
        info->objects = dmi_alloc_array(context, sizeof(dmi_entity_t *), info->object_count);
        if (info->objects == nullptr) {
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
            return false;
        }

        for (size_t i = 0; i < info->object_count; i++) {
            if (not dmi_registry_resolve(registry, info->object_handles[i], DMI_TYPE_INVALID, &info->objects[i]))
                success = false;
        }
    }

    return success;
}

void dmi_baseboard_cleanup(dmi_entity_t *entity)
{
    dmi_baseboard_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(BASEBOARD));
    if (info == nullptr)
        return;

    dmi_free(info->object_handles);
    dmi_free(info->objects);
}
