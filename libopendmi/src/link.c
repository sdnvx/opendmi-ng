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
 * @brief Handler of a handle attribute the walk of the attributes reaches,
 * given the structure holding the handle, which is an element of an array or
 * a nested structure for the handles of one.
 */
typedef bool dmi_attributes_link_fn(
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attr,
        dmi_data_t            *info);

/**
 * @internal
 * @brief State of a walk linking or unlinking the handles of a structure.
 */
typedef struct dmi_link_walk
{
    const dmi_entity_t     *entity;
    dmi_attributes_link_fn *handle;
    bool                    success;
} dmi_link_walk_t;

/**
 * @internal
 * @brief Walk the handle attributes of a structure, descending into the nested
 * structures and the arrays of them, since the handles of an element are
 * linked into the members of that element.
 *
 * @return `false` if a handle cannot be linked, `true` otherwise. The walk
 * goes on past a handle which cannot, so that every other one is linked.
 */
static bool dmi_attributes_link_walk(const dmi_entity_t *entity, dmi_attributes_link_fn *handle);

/**
 * @internal
 * @brief Give a member the walk reaches to the handler, if it is a handle or
 * an array of handles, which is linked as a whole.
 */
static dmi_attribute_walk_t dmi_attributes_link_member(void *context, const dmi_attribute_node_t *node);

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

/**
 * @internal
 * @brief Free the array of the structures an array of handles has been
 * resolved into, which is the only thing linking allocates.
 */
static bool dmi_attributes_unlink_handle(
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attr,
        dmi_data_t            *info);

bool dmi_attributes_link(dmi_entity_t *entity)
{
    if (entity == nullptr)
        return dmi_trace_argument_null(nullptr, entity);

    const dmi_entity_spec_t *spec = entity->spec;

    if ((spec == nullptr) or (spec->attributes == nullptr))
        return true;
    if (entity->info == nullptr)
        return true;

    return dmi_attributes_link_walk(entity, dmi_attributes_link_handle);
}

void dmi_attributes_unlink(dmi_entity_t *entity)
{
    if (entity == nullptr)
        return;

    const dmi_entity_spec_t *spec = entity->spec;

    if ((spec == nullptr) or (spec->attributes == nullptr))
        return;
    if (entity->info == nullptr)
        return;

    dmi_attributes_link_walk(entity, dmi_attributes_unlink_handle);
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

static bool dmi_attributes_link_walk(const dmi_entity_t *entity, dmi_attributes_link_fn *handle)
{
    static const dmi_attribute_visitor_t visitor = {
        .member_start = dmi_attributes_link_member
    };

    dmi_link_walk_t walk = {
        .entity  = entity,
        .handle  = handle,
        .success = true
    };

    dmi_attributes_walk(entity->spec->attributes, entity->info, &visitor, &walk);

    return walk.success;
}

static dmi_attribute_walk_t dmi_attributes_link_member(void *context, const dmi_attribute_node_t *node)
{
    dmi_link_walk_t *walk = context;

    if (node->attr->type != DMI_ATTRIBUTE_TYPE_HANDLE)
        return DMI_ATTRIBUTE_WALK_CONTINUE;

    if (not walk->handle(walk->entity, node->attr, node->info))
        walk->success = false;

    return DMI_ATTRIBUTE_WALK_SKIP;
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

    if (not dmi_attribute_is_array(attr)) {
        return dmi_registry_resolve_any(registry, dmi_deref(dmi_handle_t, ptr),
                                        attr->params.targets, target);
    }

    const dmi_handle_t *handles = dmi_attribute_get_elements(attr, ptr);

    size_t count = 0;
    if (handles != nullptr)
        count = dmi_attribute_get_count(attr, info);
    if (count == 0)
        return true;

    // Array left over by an attempt to link the structure which has failed
    // is dropped rather than leaked, since linking starts anew each time
    dmi_entity_t ***slot = (dmi_entity_t ***)target;

    dmi_free(*slot);
    *slot = nullptr;

    dmi_entity_t **targets = dmi_alloc_array(context, sizeof(*targets), count);
    if (targets == nullptr)
        return false;

    *slot = targets;

    bool success = true;

    for (size_t i = 0; i < count; i++) {
        if (not dmi_registry_resolve_any(registry, handles[i], attr->params.targets, &targets[i]))
            success = false;
    }

    return success;
}

static bool dmi_attributes_unlink_handle(
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attr,
        dmi_data_t            *info)
{
    dmi_unused(entity);

    assert(attr != nullptr);

    if (not dmi_member_is_present(attr->params.link) or not dmi_attribute_is_array(attr))
        return true;

    dmi_entity_t ***slot = (dmi_entity_t ***)(info + attr->params.link.offset);

    dmi_free(*slot);
    *slot = nullptr;

    return true;
}
