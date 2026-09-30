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

/**
 * @internal
 * @brief Header which stops the walk of the table before its end.
 */
typedef enum dmi_lint_table_stop
{
    DMI_LINT_TABLE_STOP_NONE = 0,   ///< Table is walked to its end
    DMI_LINT_TABLE_STOP_ZERO_LENGTH, ///< Header of no length and of another type than end-of-table
    DMI_LINT_TABLE_STOP_SHORT_LENGTH ///< Header of a length shorter than itself
} dmi_lint_table_stop_t;

static dmi_lint_table_stop_t dmi_lint_table_find_stop(dmi_lint_t *lint, size_t *poffset,
                                                      const dmi_data_t **pheader);

static void dmi_lint_table_truncated(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_table_invalid_header(dmi_lint_t *lint, const dmi_entity_t *entity);
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

const dmi_lint_rule_t dmi_lint_table_invalid_header_rule =
{
    .code   = "table.invalid-header",
    .scope  = DMI_LINT_SCOPE_TABLE,
    .check  = dmi_lint_table_invalid_header,
    .params = {
        .name              = "Structure headers are valid, so that the table is read to its end",
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

    // Header whose length is shorter than itself stops the walk as the end of
    // the data does, and is reported on its own
    size_t offset;
    const dmi_data_t *header;

    if (dmi_lint_table_find_stop(lint, &offset, &header) == DMI_LINT_TABLE_STOP_SHORT_LENGTH)
        return;

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

    // Data past a header taken for the end of the table by mistake is the
    // rest of the table, which is reported on its own
    size_t stop;
    const dmi_data_t *header;

    if (dmi_lint_table_find_stop(lint, &stop, &header) == DMI_LINT_TABLE_STOP_ZERO_LENGTH)
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

static void dmi_lint_table_invalid_header(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    size_t offset;
    const dmi_data_t *header;
    size_t size = dmi_lint_context(lint)->state.table->length;

    switch (dmi_lint_table_find_stop(lint, &offset, &header)) {
    case DMI_LINT_TABLE_STOP_ZERO_LENGTH:
        dmi_lint_issue(lint, nullptr, nullptr, offset,
                       "header of type %u and of no length is taken for the end of the table, "
                       "while %zu bytes of the table follow it",
                       (unsigned)header[0], size - offset - sizeof(dmi_header_t));
        break;

    case DMI_LINT_TABLE_STOP_SHORT_LENGTH:
        dmi_lint_issue(lint, nullptr, nullptr, offset,
                       "header of type %u declares a length of %u bytes, shorter than the header, "
                       "so the %zu bytes of the table from it on cannot be read",
                       (unsigned)header[0], (unsigned)header[1], size - offset);
        break;

    default:
        break;
    }
}

//
// Structures are located by the lengths of the ones before them, so a header
// of an invalid length stops the walk of the table. A header of no length
// stands for the end of the table whatever type it names, which firmware
// relies on, e.g. by padding the table with zeros, so it is taken for a broken
// one only when the bytes past it are not all zero. A header of a length
// shorter than itself is found past the last structure read.
//
static dmi_lint_table_stop_t dmi_lint_table_find_stop(dmi_lint_t *lint, size_t *poffset,
                                                      const dmi_data_t **pheader)
{
    const dmi_context_t *context = dmi_lint_context(lint);
    const dmi_buffer_t  *table   = context->state.table;
    const dmi_entity_t  *terminator = dmi_lint_totals(lint)->terminator;

    if (table == nullptr)
        return DMI_LINT_TABLE_STOP_NONE;

    if (terminator != nullptr) {
        size_t offset = dmi_lint_entity_offset(lint, terminator);
        if (offset == DMI_LINT_NO_OFFSET)
            return DMI_LINT_TABLE_STOP_NONE;

        const dmi_data_t *header = dmi_buffer_at(table, offset, table->length - offset);
        if ((header == nullptr) or (header[1] != 0) or (header[0] == DMI_TYPE_ID(END_OF_TABLE)))
            return DMI_LINT_TABLE_STOP_NONE;

        for (size_t i = sizeof(dmi_header_t); i < table->length - offset; i++) {
            if (header[i] != 0) {
                *poffset = offset;
                *pheader = header;
                return DMI_LINT_TABLE_STOP_ZERO_LENGTH;
            }
        }

        return DMI_LINT_TABLE_STOP_NONE;
    }

    // Walk stops past the last structure it has read
    dmi_registry_iter_t iter;
    size_t offset = 0;

    if (not dmi_registry_iter_init(&iter, dmi_get_registry((dmi_context_t *)context), nullptr))
        return DMI_LINT_TABLE_STOP_NONE;

    for (dmi_entity_t *entity; (entity = dmi_registry_iter_next(&iter)) != nullptr; ) {
        size_t start = dmi_lint_entity_offset(lint, entity);

        if ((start != DMI_LINT_NO_OFFSET) and (start + entity->total_length > offset))
            offset = start + entity->total_length;
    }

    if (table->length - offset < sizeof(dmi_header_t))
        return DMI_LINT_TABLE_STOP_NONE;

    const dmi_data_t *header = dmi_buffer_at(table, offset, sizeof(dmi_header_t));
    if ((header == nullptr) or (header[1] == 0) or (header[1] >= sizeof(dmi_header_t)))
        return DMI_LINT_TABLE_STOP_NONE;

    *poffset = offset;
    *pheader = header;
    return DMI_LINT_TABLE_STOP_SHORT_LENGTH;
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

    for (dmi_type_id_t type = 0; type <= DMI_TYPE_ID_MAX; type++) {
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

    for (dmi_type_id_t type = 0; type <= DMI_TYPE_ID_MAX; type++) {
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
    for (dmi_type_id_t type = 0; type <= DMI_TYPE_ID_MAX; type++) {
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
    if (dmi_entity_type(entity) == DMI_TYPE(end_of_table))
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
