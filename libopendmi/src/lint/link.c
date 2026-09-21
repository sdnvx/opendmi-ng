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
static void dmi_lint_link_self(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_link_orphan(dmi_lint_t *lint, const dmi_entity_t *entity);

static void dmi_lint_link_walk(
        dmi_lint_t            *lint,
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attrs,
        const void            *info,
        dmi_lint_link_fn      *handler);

static bool dmi_lint_link_is_set(dmi_handle_t handle);

const dmi_lint_rule_t dmi_lint_link_dangling_rule =
{
    .code              = "link.dangling",
    .name              = "References point to the structures of the table",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_link_dangling
};

const dmi_lint_rule_t dmi_lint_link_self_rule =
{
    .code              = "link.self",
    .name              = "Structures do not reference themselves",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_link_self
};

const dmi_lint_rule_t dmi_lint_link_orphan_rule =
{
    .code              = "link.orphan",
    .name              = "Structures are referenced by the rest of the table",
    .severity          = DMI_LINT_SEVERITY_NOTE,
    .producer_severity = DMI_LINT_SEVERITY_NOTE,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_link_orphan,
    .optional          = true
};

//
// Types which describe a part of another structure, and are meaningless
// unless something references them. The rest of the types are standalone:
// a processor or a mapped address is referenced by nothing by design.
//
static const dmi_type_t dmi_lint_referenced_types[] =
{
    DMI_TYPE_CACHE,
    DMI_TYPE_VOLTAGE_PROBE,
    DMI_TYPE_TEMPERATURE_PROBE,
    DMI_TYPE_CURRENT_PROBE,
    DMI_TYPE_MGMT_DEVICE,
    DMI_TYPE_MGMT_DEVICE_THRESHOLD
};

//
// Handles which stand for "no reference" rather than for a structure.
//
static bool dmi_lint_link_is_set(dmi_handle_t handle)
{
    return (handle != DMI_HANDLE_INVALID) and (handle != DMI_HANDLE_UNSUPPORTED);
}

//
// Walk the references of a structure, descending into the nested structures
// and the arrays of handles.
//
static void dmi_lint_link_walk(
        dmi_lint_t            *lint,
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attrs,
        const void            *info,
        dmi_lint_link_fn      *handler)
{
    if ((attrs == nullptr) or (info == nullptr))
        return;

    for (const dmi_attribute_t *attr = attrs; attr->params.name != nullptr; attr++) {
        const dmi_attribute_t *resolved = dmi_attribute_resolve(attr, info);
        if (resolved == nullptr)
            continue;

        const dmi_data_t *ptr = dmi_member_ptr(info, resolved->value, dmi_data_t);

        if (dmi_member_is_present(resolved->counter)) {
            const dmi_data_t *element = dmi_deref(dmi_data_t *, ptr);
            size_t count = (element != nullptr) ? dmi_attribute_get_count(resolved, info) : 0;

            for (size_t i = 0; i < count; i++, element += resolved->value.size) {
                if (resolved->type == DMI_ATTRIBUTE_TYPE_STRUCT)
                    dmi_lint_link_walk(lint, entity, resolved->params.attrs, element, handler);
                else if (resolved->type == DMI_ATTRIBUTE_TYPE_HANDLE)
                    handler(lint, entity, resolved, dmi_deref(dmi_handle_t, element));
            }

            continue;
        }

        if (resolved->type == DMI_ATTRIBUTE_TYPE_STRUCT)
            dmi_lint_link_walk(lint, entity, resolved->params.attrs, ptr, handler);
        else if (resolved->type == DMI_ATTRIBUTE_TYPE_HANDLE)
            handler(lint, entity, resolved, dmi_deref(dmi_handle_t, ptr));
    }
}

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
    dmi_lint_link_walk(lint, entity, entity->spec ? entity->spec->attributes : nullptr,
                       entity->info, dmi_lint_link_check_dangling);
}

static void dmi_lint_link_self(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_lint_link_walk(lint, entity, entity->spec ? entity->spec->attributes : nullptr,
                       entity->info, dmi_lint_link_check_self);
}

//
// Tell whether a structure references a handle, descending into the nested
// structures and the arrays of handles the same way the walk does.
//
static bool dmi_lint_link_references(
        const dmi_attribute_t *attrs,
        const void            *info,
        dmi_handle_t           handle)
{
    if ((attrs == nullptr) or (info == nullptr))
        return false;

    for (const dmi_attribute_t *attr = attrs; attr->params.name != nullptr; attr++) {
        const dmi_attribute_t *resolved = dmi_attribute_resolve(attr, info);
        if (resolved == nullptr)
            continue;

        const dmi_data_t *ptr = dmi_member_ptr(info, resolved->value, dmi_data_t);

        if (dmi_member_is_present(resolved->counter)) {
            const dmi_data_t *element = dmi_deref(dmi_data_t *, ptr);
            size_t count = (element != nullptr) ? dmi_attribute_get_count(resolved, info) : 0;

            for (size_t i = 0; i < count; i++, element += resolved->value.size) {
                if (resolved->type == DMI_ATTRIBUTE_TYPE_STRUCT) {
                    if (dmi_lint_link_references(resolved->params.attrs, element, handle))
                        return true;
                } else if ((resolved->type == DMI_ATTRIBUTE_TYPE_HANDLE) and
                           (dmi_deref(dmi_handle_t, element) == handle)) {
                    return true;
                }
            }

            continue;
        }

        if (resolved->type == DMI_ATTRIBUTE_TYPE_STRUCT) {
            if (dmi_lint_link_references(resolved->params.attrs, ptr, handle))
                return true;
        } else if ((resolved->type == DMI_ATTRIBUTE_TYPE_HANDLE) and
                   (dmi_deref(dmi_handle_t, ptr) == handle)) {
            return true;
        }
    }

    return false;
}

static void dmi_lint_link_orphan(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_type_t type = dmi_entity_type(entity);
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
    dmi_entity_t *other;

    if (not dmi_registry_iter_init(&iter, registry, nullptr))
        return;

    dmi_handle_t handle = dmi_entity_handle(entity);

    while ((other = dmi_registry_iter_next(&iter)) != nullptr) {
        if ((other == entity) or (other->spec == nullptr))
            continue;

        if (dmi_lint_link_references(other->spec->attributes, other->info, handle))
            return;
    }

    dmi_lint_issue(lint, entity, nullptr, dmi_lint_entity_offset(lint, entity),
                   "structure describes a part of another one, but is referenced by none");
}
