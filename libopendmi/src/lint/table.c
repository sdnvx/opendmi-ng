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

#include <opendmi/lint/table.h>

/**
 * @internal
 * @brief First handle of the range reserved by the specification.
 */
#define DMI_LINT_RESERVED_HANDLE 0xFF00

static void dmi_lint_table_truncated(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_table_terminator(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_table_trailing_data(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_table_required(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_table_recommended(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_table_singleton(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_table_reserved_handle(dmi_lint_t *lint, const dmi_entity_t *entity);

const dmi_lint_rule_t dmi_lint_table_truncated_rule =
{
    .code   = "table.truncated",
    .scope  = DMI_LINT_SCOPE_TABLE,
    .check  = dmi_lint_table_truncated,
    .params = {
        .name              = "Table holds every structure completely",
        .severity          = DMI_LINT_SEVERITY_ERROR,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_table_terminator_rule =
{
    .code   = "table.terminator",
    .scope  = DMI_LINT_SCOPE_TABLE,
    .check  = dmi_lint_table_terminator,
    .params = {
        .name              = "Table ends with an end-of-table structure",
        .severity          = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_table_trailing_data_rule =
{
    .code   = "table.trailing-data",
    .scope  = DMI_LINT_SCOPE_TABLE,
    .check  = dmi_lint_table_trailing_data,
    .params = {
        .name              = "Table has no data past the end-of-table structure",
        .severity          = DMI_LINT_SEVERITY_NOTE,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_table_required_rule =
{
    .code   = "table.missing-required",
    .scope  = DMI_LINT_SCOPE_TABLE,
    .check  = dmi_lint_table_required,
    .params = {
        .name              = "Table has the structures required by the specification",
        .severity          = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_table_recommended_rule =
{
    .code   = "table.missing-recommended",
    .scope  = DMI_LINT_SCOPE_TABLE,
    .check  = dmi_lint_table_recommended,
    .params = {
        .name              = "Table has the structures recommended for the platform",
        .severity          = DMI_LINT_SEVERITY_NOTE,
        .producer_severity = DMI_LINT_SEVERITY_WARNING,
        .optional          = true
    }
};

const dmi_lint_rule_t dmi_lint_table_singleton_rule =
{
    .code   = "table.duplicate-singleton",
    .scope  = DMI_LINT_SCOPE_TABLE,
    .check  = dmi_lint_table_singleton,
    .params = {
        .name              = "Structures which have to be unique are not repeated",
        .severity          = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_table_reserved_handle_rule =
{
    .code   = "table.reserved-handle",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_table_reserved_handle,
    .params = {
        .name              = "Handles are outside of the range reserved by the specification",
        .severity          = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

static void dmi_lint_table_truncated(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    const dmi_registry_t *registry = dmi_get_registry(dmi_lint_context(lint));

    if (dmi_registry_status(registry) & DMI_REGISTRY_STATUS_TRUNCATED) {
        dmi_lint_issue(lint, nullptr, nullptr, DMI_LINT_NO_OFFSET,
                       "table ends in the middle of a structure");
    }
}

static void dmi_lint_table_terminator(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    if (dmi_lint_totals(lint)->terminator == nullptr) {
        dmi_lint_issue(lint, nullptr, nullptr, DMI_LINT_NO_OFFSET,
                       "table has no end-of-table structure");
    }
}

static void dmi_lint_table_trailing_data(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    const dmi_entity_t *terminator = dmi_lint_totals(lint)->terminator;
    if (terminator == nullptr)
        return;

    size_t offset = dmi_lint_entity_offset(lint, terminator);
    if (offset == DMI_LINT_NO_OFFSET)
        return;

    size_t end  = offset + terminator->total_length;
    size_t size = dmi_lint_context(lint)->state.table->length;

    if (size > end) {
        dmi_lint_issue(lint, nullptr, nullptr, end,
                       "table has %zu bytes past the end-of-table structure", size - end);
    }
}

//
// Which versions require a structure of a type is part of the specification
// of that type, so the rule asks the types rather than keeping a list of its
// own, which would have to be kept in step with them.
//
static void dmi_lint_table_required(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    dmi_context_t *context = dmi_lint_context(lint);
    const dmi_lint_totals_t *totals = dmi_lint_totals(lint);
    dmi_version_t version = dmi_lint_version(lint);

    for (dmi_type_t type = 0; type <= DMI_TYPE_MAX; type++) {
        const dmi_entity_spec_t *spec = dmi_type_spec(context, type);

        if ((spec == nullptr) or (spec->params.required_from == DMI_VERSION_NONE))
            continue;

        // A type is required from the version which introduced the
        // requirement up to the one which withdrew it, if any
        if (version < spec->params.required_from)
            continue;

        if ((spec->params.required_till != DMI_VERSION_NONE) and
            (version > spec->params.required_till))
            continue;

        if (totals->type_counts[type] == 0) {
            dmi_lint_issue(lint, nullptr, nullptr, DMI_LINT_NO_OFFSET,
                           "table has no structure of type %d (%s)",
                           (int)type, dmi_type_name(context, type));
        }
    }
}

//
// Which versions expect a structure of a type is part of the specification of
// that type, the same way the versions which require it are.
//
static void dmi_lint_table_recommended(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    dmi_context_t *context = dmi_lint_context(lint);
    const dmi_lint_totals_t *totals = dmi_lint_totals(lint);
    dmi_version_t version = dmi_lint_version(lint);

    for (dmi_type_t type = 0; type <= DMI_TYPE_MAX; type++) {
        const dmi_entity_spec_t *spec = dmi_type_spec(context, type);

        if ((spec == nullptr) or (spec->params.recommended_from == DMI_VERSION_NONE))
            continue;

        if (version < spec->params.recommended_from)
            continue;

        if (totals->type_counts[type] == 0) {
            dmi_lint_issue(lint, nullptr, nullptr, DMI_LINT_NO_OFFSET,
                           "table has no structure of type %d (%s)",
                           (int)type, dmi_type_name(context, type));
        }
    }
}

static void dmi_lint_table_singleton(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    dmi_context_t *context = dmi_lint_context(lint);
    const dmi_lint_totals_t *totals = dmi_lint_totals(lint);

    // Uniqueness belongs to the type, the same way its required versions do
    for (dmi_type_t type = 0; type <= DMI_TYPE_MAX; type++) {
        const dmi_entity_spec_t *spec = dmi_type_spec(context, type);

        if ((spec == nullptr) or not spec->params.unique)
            continue;

        if (totals->type_counts[type] > 1) {
            dmi_lint_issue(lint, nullptr, nullptr, DMI_LINT_NO_OFFSET,
                           "table has %zu structures of type %d (%s), expected one",
                           totals->type_counts[type], (int)type, dmi_type_name(context, type));
        }
    }
}

static void dmi_lint_table_reserved_handle(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    // Firmware commonly gives the end-of-table structure a handle of the
    // reserved range, so it is not worth reporting
    if (dmi_entity_type(entity) == DMI_TYPE_END_OF_TABLE)
        return;

    dmi_handle_t handle = dmi_entity_handle(entity);

    // Handles of the reserved range are used by the specification itself, and
    // are not to be assigned to structures
    if (handle >= DMI_LINT_RESERVED_HANDLE) {
        dmi_lint_issue(lint, entity, nullptr, dmi_lint_entity_offset(lint, entity),
                       "handle 0x%04X belongs to the range 0x%04X-0xFFFF reserved "
                       "by the specification", (unsigned)handle, DMI_LINT_RESERVED_HANDLE);
    }
}
