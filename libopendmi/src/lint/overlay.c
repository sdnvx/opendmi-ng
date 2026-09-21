//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/lint.h>
#include <opendmi/registry.h>
#include <opendmi/internal.h>

#include <opendmi/entity/additional-info.h>
#include <opendmi/lint/overlay.h>

/**
 * @internal
 * @brief Length of the structure header, which the entries may not refer to.
 */
#define DMI_LINT_HEADER_LENGTH 4

static void dmi_lint_overlay_dangling(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_overlay_out_of_bounds(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_overlay_empty(dmi_lint_t *lint, const dmi_entity_t *entity);

static const dmi_additional_info_t *dmi_lint_overlay_info(const dmi_entity_t *entity);

const dmi_lint_rule_t dmi_lint_overlay_dangling_rule =
{
    .code   = "overlay.dangling",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_overlay_dangling,
    .params = {
        .name              = "Additional information entries refer to the structures of the table",
        .severity          = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_overlay_out_of_bounds_rule =
{
    .code   = "overlay.out-of-bounds",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_overlay_out_of_bounds,
    .params = {
        .name              = "Additional information entries refer to the fields of the structures",
        .severity          = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_overlay_empty_rule =
{
    .code   = "overlay.empty",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_overlay_empty,
    .params = {
        .name              = "Additional information entries carry values",
        .severity          = DMI_LINT_SEVERITY_NOTE,
        .producer_severity = DMI_LINT_SEVERITY_WARNING,
        .optional          = true
    }
};

//
// Decoded data of an additional information structure, which the rules of the
// entries are checked against.
//
static const dmi_additional_info_t *dmi_lint_overlay_info(const dmi_entity_t *entity)
{
    if (dmi_entity_type(entity) != DMI_TYPE_ADDITIONAL_INFO)
        return nullptr;

    return dmi_entity_info(entity, DMI_TYPE(ADDITIONAL_INFO));
}

static void dmi_lint_overlay_dangling(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_additional_info_t *info = dmi_lint_overlay_info(entity);
    if (info == nullptr)
        return;

    dmi_registry_t *registry = dmi_get_registry(dmi_lint_context(lint));

    for (size_t i = 0; i < info->entry_count; i++) {
        const dmi_additional_info_entry_t *entry = &info->entries[i];

        if (dmi_registry_lookup(registry, entry->ref_handle, DMI_TYPE_ANY, true) != nullptr)
            continue;

        dmi_lint_issue(lint, entity, "referenced-handle", dmi_lint_entity_offset(lint, entity),
                       "entry %zu refers to handle 0x%04X, which belongs to no structure",
                       i + 1, (unsigned)entry->ref_handle);
    }
}

static void dmi_lint_overlay_out_of_bounds(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_additional_info_t *info = dmi_lint_overlay_info(entity);
    if (info == nullptr)
        return;

    dmi_registry_t *registry = dmi_get_registry(dmi_lint_context(lint));

    for (size_t i = 0; i < info->entry_count; i++) {
        const dmi_additional_info_entry_t *entry = &info->entries[i];

        const dmi_entity_t *target =
                dmi_registry_lookup(registry, entry->ref_handle, DMI_TYPE_ANY, true);
        if (target == nullptr)
            continue;

        // Entries carry field values, and the header of a structure is not a
        // field of it
        if (entry->ref_offset < DMI_LINT_HEADER_LENGTH) {
            dmi_lint_issue(lint, entity, "referenced-offset", dmi_lint_entity_offset(lint, entity),
                           "entry %zu refers to offset 0x%02X, which belongs to the header of "
                           "handle 0x%04X", i + 1, entry->ref_offset, (unsigned)entry->ref_handle);
            continue;
        }

        if (entry->ref_offset + entry->value.length > target->body_length) {
            dmi_lint_issue(lint, entity, "referenced-offset", dmi_lint_entity_offset(lint, entity),
                           "entry %zu refers to %zu bytes at offset 0x%02X, while handle 0x%04X "
                           "is %zu bytes long", i + 1, entry->value.length, entry->ref_offset,
                           (unsigned)entry->ref_handle, target->body_length);
        }
    }
}

static void dmi_lint_overlay_empty(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_additional_info_t *info = dmi_lint_overlay_info(entity);
    if (info == nullptr)
        return;

    for (size_t i = 0; i < info->entry_count; i++) {
        const dmi_additional_info_entry_t *entry = &info->entries[i];

        if ((entry->value.length > 0) or (entry->string != nullptr))
            continue;

        dmi_lint_issue(lint, entity, nullptr, dmi_lint_entity_offset(lint, entity),
                       "entry %zu carries neither a value nor a string", i + 1);
    }
}
