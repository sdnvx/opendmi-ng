//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdarg.h>
#include <stdio.h>

#include <opendmi/attribute.h>
#include <opendmi/context.h>
#include <opendmi/lint.h>
#include <opendmi/registry.h>
#include <opendmi/internal.h>

#include <opendmi/lint/link.h>

typedef void dmi_lint_link_fn(
        dmi_lint_t            *lint,
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attr,
        dmi_handle_t           handle);

static void dmi_lint_link_dangling(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_link_wrong_type(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_link_self(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_link_orphan(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Walk the references of a structure, descending into the nested
 * structures and the arrays of handles, and give every handle to the handler.
 */
static void dmi_lint_link_walk(dmi_lint_t *lint, const dmi_entity_t *entity, dmi_lint_link_fn *handler);

/**
 * @internal
 * @brief Give a value the walk of the references reaches to the handler of
 * the check, if it is a handle.
 */
static bool dmi_lint_link_visit(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Tell whether a structure references a handle, descending into the
 * nested structures and the arrays of handles the same way the walk does.
 */
static bool dmi_lint_link_references(const dmi_entity_t *entity, dmi_handle_t handle);

/**
 * @internal
 * @brief Stop the walk of the references at the handle being looked for.
 */
static bool dmi_lint_link_match(void *context, const dmi_attribute_node_t *node);

static bool dmi_lint_link_is_set(dmi_handle_t handle);

/**
 * @internal
 * @brief State of a walk of the references a check of the links makes.
 */
typedef struct dmi_lint_link_walk
{
    dmi_lint_t         *lint;
    const dmi_entity_t *entity;
    dmi_lint_link_fn   *handler;
} dmi_lint_link_walk_t;

const dmi_lint_rule_t dmi_lint_link_dangling_rule =
{
    .code   = "link.dangling",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_link_dangling,
    .params = {
        .name              = "References point to the structures of the table",
        .reader_severity   = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_link_self_rule =
{
    .code   = "link.self",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_link_self,
    .params = {
        .name              = "Structures do not reference themselves",
        .reader_severity   = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_link_wrong_type_rule =
{
    .code   = "link.wrong-type",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_link_wrong_type,
    .params = {
        .name              = "References point to the structures of the types they expect",
        .reader_severity   = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_link_orphan_rule =
{
    .code   = "link.orphan",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_link_orphan,
    .params = {
        .name              = "Structures are referenced by the rest of the table",
        .reader_severity   = DMI_LINT_SEVERITY_NOTE,
        .producer_severity = DMI_LINT_SEVERITY_NOTE,
        .optional          = true
    }
};

//
// Types which describe a part of another structure, and are meaningless
// unless something references them. The rest of the types are standalone:
// a processor or a mapped address is referenced by nothing by design.
//
static const dmi_type_id_t dmi_lint_referenced_types[] =
{
    DMI_TYPE_ID_CACHE,
    DMI_TYPE_ID_VOLTAGE_PROBE,
    DMI_TYPE_ID_TEMPERATURE_PROBE,
    DMI_TYPE_ID_CURRENT_PROBE,
    DMI_TYPE_ID_MGMT_DEVICE,
    DMI_TYPE_ID_MGMT_DEVICE_THRESHOLD
};

//
// Handles which stand for "no reference" rather than for a structure.
//
static bool dmi_lint_link_is_set(dmi_handle_t handle)
{
    return (handle != DMI_HANDLE_INVALID) and (handle != DMI_HANDLE_UNSUPPORTED);
}

static void dmi_lint_link_walk(dmi_lint_t *lint, const dmi_entity_t *entity, dmi_lint_link_fn *handler)
{
    static const dmi_attribute_visitor_t visitor = {
        .value = dmi_lint_link_visit
    };

    if (entity->spec == nullptr)
        return;

    dmi_lint_link_walk_t walk = {
        .lint    = lint,
        .entity  = entity,
        .handler = handler
    };

    dmi_attributes_walk(entity->spec->attributes, entity->info, &visitor, &walk);
}

static bool dmi_lint_link_visit(void *context, const dmi_attribute_node_t *node)
{
    const dmi_lint_link_walk_t *walk = context;

    if (node->attr->type == DMI_ATTRIBUTE_TYPE_HANDLE)
        walk->handler(walk->lint, walk->entity, node->attr, dmi_deref(dmi_handle_t, node->value));

    return true;
}

//
// Attributes which name the types they refer to describe the shape of the
// table, so a reference to a structure of another type is a broken one.
//
static void dmi_lint_link_check_type(
        dmi_lint_t            *lint,
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attr,
        dmi_handle_t           handle)
{
    if (not dmi_lint_link_is_set(handle) or (attr->params.targets == nullptr))
        return;

    dmi_registry_t *registry = dmi_get_registry(dmi_lint_context(lint));

    const dmi_entity_t *target = dmi_registry_lookup(registry, handle, DMI_TYPE_ANY, true);
    if (target == nullptr)
        return;

    const dmi_type_t *type = dmi_entity_type(target);

    for (const dmi_type_t *const *expected = attr->params.targets; *expected != nullptr; expected++) {
        if (*expected == type)
            return;
    }

    dmi_lint_issue(lint, entity, attr->params.code, dmi_lint_entity_offset(lint, entity),
                   "handle 0x%04X refers to a structure of type %d (%s)",
                   (unsigned)handle, (int)dmi_entity_type_id(target),
                   dmi_entity_name(target));
}

static void dmi_lint_link_wrong_type(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_lint_link_walk(lint, entity, dmi_lint_link_check_type);
}

static void dmi_lint_link_dangling(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_link_self(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_link_orphan(dmi_lint_t *lint, const dmi_entity_t *entity);

static void dmi_lint_link_check_dangling(
        dmi_lint_t            *lint,
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attr,
        dmi_handle_t           handle)
{
    if (not dmi_lint_link_is_set(handle))
        return;

    dmi_registry_t *registry = dmi_get_registry(dmi_lint_context(lint));

    if (dmi_registry_lookup(registry, handle, DMI_TYPE_ANY, true) == nullptr) {
        dmi_lint_issue(lint, entity, attr->params.code, dmi_lint_entity_offset(lint, entity),
                       "handle 0x%04X belongs to no structure of the table", (unsigned)handle);
    }
}

static void dmi_lint_link_check_self(
        dmi_lint_t            *lint,
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attr,
        dmi_handle_t           handle)
{
    if (dmi_lint_link_is_set(handle) and (handle == dmi_entity_handle(entity))) {
        dmi_lint_issue(lint, entity, attr->params.code, dmi_lint_entity_offset(lint, entity),
                       "structure references itself");
    }
}

static void dmi_lint_link_dangling(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_lint_link_walk(lint, entity, dmi_lint_link_check_dangling);
}

static void dmi_lint_link_self(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_lint_link_walk(lint, entity, dmi_lint_link_check_self);
}

static bool dmi_lint_link_references(const dmi_entity_t *entity, dmi_handle_t handle)
{
    static const dmi_attribute_visitor_t visitor = {
        .value = dmi_lint_link_match
    };

    if (entity->spec == nullptr)
        return false;

    // Walk is stopped as soon as the handle is found
    return not dmi_attributes_walk(entity->spec->attributes, entity->info, &visitor, &handle);
}

static bool dmi_lint_link_match(void *context, const dmi_attribute_node_t *node)
{
    const dmi_handle_t *handle = context;

    return (node->attr->type != DMI_ATTRIBUTE_TYPE_HANDLE) or
           (dmi_deref(dmi_handle_t, node->value) != *handle);
}

static void dmi_lint_link_orphan(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_type_id_t type = dmi_entity_type_id(entity);
    bool referenced = false;

    for (size_t i = 0; i < countof(dmi_lint_referenced_types); i++) {
        if (type == dmi_lint_referenced_types[i]) {
            referenced = true;
            break;
        }
    }

    if (not referenced or (entity->spec == nullptr))
        return;

    dmi_registry_t *registry = dmi_get_registry(dmi_lint_context(lint));
    dmi_registry_iter_t iter;
    const dmi_entity_t *other;

    if (not dmi_registry_iter_init(&iter, registry, nullptr))
        return;

    dmi_handle_t handle = dmi_entity_handle(entity);

    while ((other = dmi_registry_iter_next(&iter)) != nullptr) {
        if (other == entity)
            continue;

        if (dmi_lint_link_references(other, handle))
            return;
    }

    dmi_lint_issue(lint, entity, nullptr, dmi_lint_entity_offset(lint, entity),
                   "structure describes a part of another one, but is referenced by none");
}
