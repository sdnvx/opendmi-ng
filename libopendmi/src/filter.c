//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/entity.h>
#include <opendmi/filter.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>

dmi_filter_t *dmi_filter_create(dmi_context_t *context)
{
    dmi_filter_t *filter;

    filter = dmi_alloc(context, sizeof(*filter));
    if (filter == nullptr)
        return nullptr;

    filter->context = context;
    filter->mask    = DMI_FILTER_MASK_ALL;

    return filter;
}

bool dmi_filter_add_handle(dmi_filter_t *filter, dmi_handle_t handle)
{
    if (filter == nullptr)
        return false;

    return dmi_vector_push(&filter->handles, (uintptr_t)handle);
}

bool dmi_filter_add_type(dmi_filter_t *filter, dmi_type_t type)
{
    if (filter == nullptr)
        return false;

    return dmi_vector_push(&filter->types, (uintptr_t)type);
}

bool dmi_filter_is_empty(const dmi_filter_t *filter)
{
    if (filter == nullptr)
        return true;

    return
        (filter->handles.length == 0) and
        (filter->types.length == 0);
}

bool dmi_filter_match(const dmi_filter_t *filter, const dmi_entity_t *entity)
{
    if ((filter == nullptr) or (entity == nullptr))
        return false;

    // Filter entities by category
    if (entity->type < __DMI_TYPE_OEM_START) {
        if ((filter->mask & DMI_FILTER_MASK_COMMON) == 0)
            return false;
    } else {
        if ((filter->mask & DMI_FILTER_MASK_OEM) == 0)
            return false;
    }

    // Filter inactive entities
    if (entity->type == DMI_TYPE(INACTIVE)) {
        if ((filter->mask & DMI_FILTER_MASK_INACTIVE) == 0)
            return false;
    }

    // Filter unknown entities
    if (entity->spec == nullptr) {
        if ((filter->mask & DMI_FILTER_MASK_UNKNOWN) == 0)
            return false;
    }

    if (dmi_filter_is_empty(filter))
        return true;

    // Filter specific handles
    if (filter->handles.length > 0) {
        dmi_handle_t handle = dmi_entity_handle(entity);

        for (size_t i = 0; i < filter->handles.length; i++) {
            if (handle == (dmi_handle_t)filter->handles.data[i])
                return true;
        }
    }

    // Filter specific types
    if (filter->types.length > 0) {
        dmi_type_t type = dmi_entity_type(entity);

        for (size_t i = 0; i < filter->types.length; i++) {
            if (type == (dmi_type_t)filter->types.data[i])
                return true;
        }
    }

    return false;
}

void dmi_filter_destroy(dmi_filter_t *filter)
{
    if (filter == nullptr)
        return;

    dmi_vector_clear(&filter->handles);
    dmi_vector_clear(&filter->types);

    dmi_free(filter);
}
