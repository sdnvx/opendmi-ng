//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/group-assoc-internal.h>

bool dmi_group_assoc_link(dmi_entity_t *entity)
{
    dmi_group_assoc_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(GROUP_ASSOC));
    if (info == nullptr)
        return false;

    dmi_context_t  *context  = dmi_entity_context(entity);
    dmi_registry_t *registry = dmi_get_registry(context);

    bool success = true;
    for (size_t i = 0; i < info->item_count; i++) {
        dmi_group_assoc_item_t *item = &info->items[i];

        if (not dmi_registry_resolve(registry, item->handle, item->type, &item->entity))
            success = false;
    }

    return success;
}

void dmi_group_assoc_cleanup(dmi_entity_t *entity)
{
    dmi_group_assoc_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(GROUP_ASSOC));
    if (info == nullptr)
        return;

    dmi_free(info->items);
}
