//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>

#include <opendmi/attribute.h>
#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/error.h>
#include <opendmi/registry.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>

/**
 * @internal
 * @brief Attributes declare links wherever a handle names the member its
 * structure goes into, including the handles of the nested structures.
 */
static bool dmi_attributes_have_links(const dmi_attribute_t *attrs);

/**
 * @internal
 * @brief Walk the attributes of a structure, descending into the nested
 * structures and the arrays of them, since the handles of an element are
 * linked into the members of that element.
 */
static bool dmi_attributes_link_list(
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attrs,
        dmi_data_t            *info);

/**
 * @internal
 * @brief Resolve a handle, or every handle of an array of them, into the
 * member the attribute names. The structures of an array are held by an array
 * of their own, one pointer per handle, which is allocated here.
 */
static bool dmi_attributes_link_handle(
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attr,
        dmi_data_t            *info);

bool dmi_attributes_link(dmi_entity_t *entity)
{
    if (entity == nullptr) {
        dmi_error_raise_ex(nullptr, DMI_ERROR_NULL_ARGUMENT, "entity");
        return false;
    }

    const dmi_entity_spec_t *spec = entity->spec;

    if ((spec == nullptr) or (spec->attributes == nullptr))
        return true;
    if (entity->info == nullptr)
        return true;

    return dmi_attributes_link_list(entity, spec->attributes, entity->info);
}

bool dmi_entity_is_linkable(const dmi_entity_t *entity)
{
    if ((entity == nullptr) or (entity->spec == nullptr))
        return false;

    if (entity->spec->handlers.link != nullptr)
        return true;

    return dmi_attributes_have_links(entity->spec->attributes);
}

static bool dmi_attributes_have_links(const dmi_attribute_t *attrs)
{
    if (attrs == nullptr)
        return false;

    for (const dmi_attribute_t *attr = attrs; attr->type != DMI_ATTRIBUTE_TYPE_NONE; attr++) {
        if (attr->type == DMI_ATTRIBUTE_TYPE_HANDLE) {
            if (dmi_member_is_present(attr->params.link))
                return true;
        } else if (attr->type == DMI_ATTRIBUTE_TYPE_STRUCT) {
            if (dmi_attributes_have_links(attr->params.attrs))
                return true;
        }
    }

    return false;
}

static bool dmi_attributes_link_list(
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attrs,
        dmi_data_t            *info)
{
    assert(entity != nullptr);

    if ((attrs == nullptr) or (info == nullptr))
        return true;

    bool success = true;

    for (const dmi_attribute_t *attr = attrs; attr->type != DMI_ATTRIBUTE_TYPE_NONE; attr++) {
        const dmi_attribute_t *resolved = dmi_attribute_resolve(attr, info);

        if (resolved == nullptr)
            continue;

        if (resolved->type == DMI_ATTRIBUTE_TYPE_HANDLE) {
            if (not dmi_attributes_link_handle(entity, resolved, info))
                success = false;
            continue;
        }

        if (resolved->type != DMI_ATTRIBUTE_TYPE_STRUCT)
            continue;

        dmi_data_t *ptr = info + resolved->value.offset;

        if (not dmi_member_is_present(resolved->counter)) {
            if (not dmi_attributes_link_list(entity, resolved->params.attrs, ptr))
                success = false;
            continue;
        }

        dmi_data_t *element = *(dmi_data_t **)ptr;

        size_t count = 0;
        if (element != nullptr)
            count = dmi_attribute_get_count(resolved, info);

        for (size_t i = 0; i < count; i++, element += resolved->value.size) {
            if (not dmi_attributes_link_list(entity, resolved->params.attrs, element))
                success = false;
        }
    }

    return success;
}

static bool dmi_attributes_link_handle(
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attr,
        dmi_data_t            *info)
{
    assert(entity != nullptr);
    assert(attr != nullptr);

    if (not dmi_member_is_present(attr->params.link))
        return true;

    dmi_context_t  *context  = dmi_entity_context(entity);
    dmi_registry_t *registry = dmi_get_registry(context);

    const dmi_data_t *ptr    = info + attr->value.offset;
    dmi_entity_t    **target = (dmi_entity_t **)(info + attr->params.link.offset);

    if (not dmi_member_is_present(attr->counter)) {
        return dmi_registry_resolve_any(registry, dmi_deref(dmi_handle_t, ptr),
                                        attr->params.targets, target);
    }

    const dmi_handle_t *handles = *(const dmi_handle_t * const *)ptr;

    size_t count = 0;
    if (handles != nullptr)
        count = dmi_attribute_get_count(attr, info);
    if (count == 0)
        return true;

    dmi_entity_t **targets = dmi_alloc_array(context, sizeof(*targets), count);
    if (targets == nullptr)
        return false;

    *(dmi_entity_t ***)target = targets;

    bool success = true;

    for (size_t i = 0; i < count; i++) {
        if (not dmi_registry_resolve_any(registry, handles[i], attr->params.targets, &targets[i]))
            success = false;
    }

    return success;
}
