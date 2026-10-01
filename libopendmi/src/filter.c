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

/**
 * @internal
 * @brief Check a structure against the mask of a filter, which tells the
 * categories of the structures to match: common, OEM-specific, inactive and
 * unknown ones.
 *
 * @param[in] filter Filter to check against.
 * @param[in] entity Structure to check.
 *
 * @return `true` if every category of the structure is in the mask, `false`
 *         otherwise.
 */
static bool dmi_filter_match_mask(const dmi_filter_t *filter, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check whether the handle of a structure is one of the handles of a
 * filter.
 *
 * @param[in] filter Filter to check against.
 * @param[in] entity Structure to check.
 *
 * @return `true` if the handle is in the filter, `false` otherwise.
 */
static bool dmi_filter_match_handle(const dmi_filter_t *filter, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check whether the type of a structure is one of the types of a
 * filter.
 *
 * @param[in] filter Filter to check against.
 * @param[in] entity Structure to check.
 *
 * @return `true` if the type is in the filter, `false` otherwise.
 */
static bool dmi_filter_match_type(const dmi_filter_t *filter, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check whether a structure is decoded by a specification of one of
 * the modules of a filter.
 *
 * @details Structures are told by their specifications rather than by their
 * type numbers, since the type number of an OEM-specific structure depends
 * on the platform.
 *
 * @param[in] filter Filter to check against.
 * @param[in] entity Structure to check.
 *
 * @return `true` if a module of the filter has the specification of the
 *         structure, `false` otherwise.
 */
static bool dmi_filter_match_module(const dmi_filter_t *filter, const dmi_entity_t *entity);

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

bool dmi_filter_add_type(dmi_filter_t *filter, dmi_type_id_t type)
{
    if (filter == nullptr)
        return false;

    return dmi_vector_push(&filter->types, (uintptr_t)type);
}

bool dmi_filter_add_module(dmi_filter_t *filter, const dmi_module_t *module)
{
    if ((filter == nullptr) or (module == nullptr))
        return false;

    return dmi_vector_push(&filter->modules, (uintptr_t)module);
}

bool dmi_filter_is_empty(const dmi_filter_t *filter)
{
    if (filter == nullptr)
        return true;

    return
        (filter->handles.length == 0) and
        (filter->types.length == 0) and
        (filter->modules.length == 0);
}

bool dmi_filter_match(const dmi_filter_t *filter, const dmi_entity_t *entity)
{
    if ((filter == nullptr) or (entity == nullptr))
        return false;

    if (not dmi_filter_match_mask(filter, entity))
        return false;

    if (dmi_filter_is_empty(filter))
        return true;

    return
        dmi_filter_match_handle(filter, entity) or
        dmi_filter_match_type(filter, entity) or
        dmi_filter_match_module(filter, entity);
}

void dmi_filter_destroy(dmi_filter_t *filter)
{
    if (filter == nullptr)
        return;

    dmi_vector_clear(&filter->handles);
    dmi_vector_clear(&filter->types);
    dmi_vector_clear(&filter->modules);

    dmi_free(filter);
}

static bool dmi_filter_match_mask(const dmi_filter_t *filter, const dmi_entity_t *entity)
{
    unsigned mask = filter->mask;

    // Filter entities by category
    if (entity->type_id < __DMI_TYPE_ID_OEM_START) {
        if ((mask & DMI_FILTER_MASK_COMMON) == 0)
            return false;
    } else {
        if ((mask & DMI_FILTER_MASK_OEM) == 0)
            return false;
    }

    // Filter inactive entities
    if ((entity->type_id == DMI_TYPE_ID(INACTIVE)) and ((mask & DMI_FILTER_MASK_INACTIVE) == 0))
        return false;

    // Filter unknown entities
    if ((entity->spec == nullptr) and ((mask & DMI_FILTER_MASK_UNKNOWN) == 0))
        return false;

    return true;
}

static bool dmi_filter_match_handle(const dmi_filter_t *filter, const dmi_entity_t *entity)
{
    dmi_handle_t handle = dmi_entity_handle(entity);

    for (size_t i = 0; i < filter->handles.length; i++) {
        if (handle == (dmi_handle_t)filter->handles.data[i])
            return true;
    }

    return false;
}

static bool dmi_filter_match_type(const dmi_filter_t *filter, const dmi_entity_t *entity)
{
    dmi_type_id_t type = dmi_entity_type_id(entity);

    for (size_t i = 0; i < filter->types.length; i++) {
        if (type == (dmi_type_id_t)filter->types.data[i])
            return true;
    }

    return false;
}

static bool dmi_filter_match_module(const dmi_filter_t *filter, const dmi_entity_t *entity)
{
    if (entity->spec == nullptr)
        return false;

    for (size_t i = 0; i < filter->modules.length; i++) {
        const dmi_module_t *module = (const dmi_module_t *)filter->modules.data[i];

        if (module->entities == nullptr)
            continue;

        for (const dmi_entity_spec_t **pspec = module->entities; *pspec != nullptr; pspec++) {
            if (*pspec == entity->spec)
                return true;
        }
    }

    return false;
}
