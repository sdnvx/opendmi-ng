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

static void dmi_lint_table_missing(dmi_lint_t *lint, const dmi_type_t *types, size_t count);

const dmi_lint_rule_t dmi_lint_table_truncated_rule =
{
    .code              = "table.truncated",
    .name              = "Table holds every structure completely",
    .severity          = DMI_LINT_SEVERITY_ERROR,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_TABLE,
    .check             = dmi_lint_table_truncated
};

const dmi_lint_rule_t dmi_lint_table_terminator_rule =
{
    .code              = "table.terminator",
    .name              = "Table ends with an end-of-table structure",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_TABLE,
    .check             = dmi_lint_table_terminator
};

const dmi_lint_rule_t dmi_lint_table_trailing_data_rule =
{
    .code              = "table.trailing-data",
    .name              = "Table has no data past the end-of-table structure",
    .severity          = DMI_LINT_SEVERITY_NOTE,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_TABLE,
    .check             = dmi_lint_table_trailing_data
};

const dmi_lint_rule_t dmi_lint_table_required_rule =
{
    .code              = "table.missing-required",
    .name              = "Table has the structures required by the specification",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_TABLE,
    .check             = dmi_lint_table_required
};

const dmi_lint_rule_t dmi_lint_table_recommended_rule =
{
    .code              = "table.missing-recommended",
    .name              = "Table has the structures recommended for the platform",
    .severity          = DMI_LINT_SEVERITY_NOTE,
    .producer_severity = DMI_LINT_SEVERITY_WARNING,
    .scope             = DMI_LINT_SCOPE_TABLE,
    .check             = dmi_lint_table_recommended,
    .optional          = true
};

const dmi_lint_rule_t dmi_lint_table_singleton_rule =
{
    .code              = "table.duplicate-singleton",
    .name              = "Structures which have to be unique are not repeated",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_TABLE,
    .check             = dmi_lint_table_singleton
};

const dmi_lint_rule_t dmi_lint_table_reserved_handle_rule =
{
    .code              = "table.reserved-handle",
    .name              = "Handles are outside of the range reserved by the specification",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_table_reserved_handle
};

//
// Structures required by the specification, and the ones expected from a
// platform conforming to SMBIOS 3.0 and later.
//
static const dmi_type_t dmi_lint_required_types[] =
{
    DMI_TYPE_FIRMWARE,
    DMI_TYPE_SYSTEM,
    DMI_TYPE_END_OF_TABLE
};

static const dmi_type_t dmi_lint_recommended_types[] =
{
    DMI_TYPE_PROCESSOR,
    DMI_TYPE_MEMORY_ARRAY,
    DMI_TYPE_MEMORY_DEVICE,
    DMI_TYPE_MEMORY_ARRAY_ADDR
};

static const dmi_type_t dmi_lint_singleton_types[] =
{
    DMI_TYPE_FIRMWARE,
    DMI_TYPE_SYSTEM
};

static void dmi_lint_table_truncated(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    dmi_registry_t *registry = dmi_get_registry(dmi_lint_context(lint));

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
    size_t size = dmi_lint_context(lint)->state.table_size;

    if (size > end) {
        dmi_lint_issue(lint, nullptr, nullptr, end,
                       "table has %zu bytes past the end-of-table structure", size - end);
    }
}

//
// Report the types which the table has no structure of.
//
static void dmi_lint_table_missing(dmi_lint_t *lint, const dmi_type_t *types, size_t count)
{
    dmi_context_t *context = dmi_lint_context(lint);
    const dmi_lint_totals_t *totals = dmi_lint_totals(lint);

    for (size_t i = 0; i < count; i++) {
        dmi_type_t type = types[i];

        if (totals->type_counts[type] == 0) {
            dmi_lint_issue(lint, nullptr, nullptr, DMI_LINT_NO_OFFSET,
                           "table has no structure of type %d (%s)",
                           (int)type, dmi_type_name(context, type));
        }
    }
}

static void dmi_lint_table_required(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    dmi_lint_table_missing(lint, dmi_lint_required_types, countof(dmi_lint_required_types));
}

static void dmi_lint_table_recommended(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    dmi_lint_table_missing(lint, dmi_lint_recommended_types, countof(dmi_lint_recommended_types));
}

static void dmi_lint_table_singleton(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    dmi_context_t *context = dmi_lint_context(lint);
    const dmi_lint_totals_t *totals = dmi_lint_totals(lint);

    for (size_t i = 0; i < countof(dmi_lint_singleton_types); i++) {
        dmi_type_t type = dmi_lint_singleton_types[i];

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
