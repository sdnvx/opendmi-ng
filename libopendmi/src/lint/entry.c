//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>

#include <opendmi/context.h>
#include <opendmi/entry.h>
#include <opendmi/lint.h>
#include <opendmi/internal.h>

#include <opendmi/lint/entry.h>

static void dmi_lint_entry_checksum(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_entry_length(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_entry_table_address(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_entry_table_size(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_entry_entity_count(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_entry_entity_max_size(dmi_lint_t *lint, const dmi_entity_t *entity);

static bool dmi_lint_entry_verify(const dmi_data_t *data, size_t length);

const dmi_lint_rule_t dmi_lint_entry_checksum_rule =
{
    .code   = "entry.checksum",
    .scope  = DMI_LINT_SCOPE_ENTRY,
    .check  = dmi_lint_entry_checksum,
    .params = {
        .name              = "Checksum of the entry point matches its data",
        .severity          = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_entry_length_rule =
{
    .code   = "entry.length",
    .scope  = DMI_LINT_SCOPE_ENTRY,
    .check  = dmi_lint_entry_length,
    .params = {
        .name              = "Length of the entry point matches its format",
        .severity          = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_entry_table_address_rule =
{
    .code   = "entry.table-address",
    .scope  = DMI_LINT_SCOPE_ENTRY,
    .check  = dmi_lint_entry_table_address,
    .params = {
        .name              = "Entry point points to the table",
        .severity          = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_entry_table_size_rule =
{
    .code   = "entry.table-size",
    .scope  = DMI_LINT_SCOPE_ENTRY,
    .check  = dmi_lint_entry_table_size,
    .params = {
        .name              = "Size of the table matches the one declared by the entry point",
        .severity          = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_entry_entity_count_rule =
{
    .code   = "entry.entity-count",
    .scope  = DMI_LINT_SCOPE_ENTRY,
    .check  = dmi_lint_entry_entity_count,
    .params = {
        .name              = "Number of the structures matches the one declared by the entry point",
        .severity          = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_entry_entity_max_size_rule =
{
    .code   = "entry.entity-max-size",
    .scope  = DMI_LINT_SCOPE_ENTRY,
    .check  = dmi_lint_entry_entity_max_size,
    .params = {
        .name              = "Structures fit the maximum size declared by the entry point",
        .severity          = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

//
// Sum of the bytes of an entry point, which is zero for a valid checksum.
//
static bool dmi_lint_entry_verify(const dmi_data_t *data, size_t length)
{
    uint8_t sum = 0;

    for (size_t i = 0; i < length; i++)
        sum += data[i];

    return sum == 0;
}

static void dmi_lint_entry_checksum(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    const dmi_context_state_t *state = &dmi_lint_context(lint)->state;
    if ((state->entry_data == nullptr) or (state->entry_spec == nullptr))
        return;

    const dmi_data_t *data = state->entry_data;
    size_t length = (state->entry_length < state->entry_data_size)
            ? state->entry_length : state->entry_data_size;

    if ((length > 0) and not dmi_lint_entry_verify(data, length)) {
        dmi_lint_issue(lint, nullptr, nullptr, 0,
                       "checksum of the entry point does not match its %zu bytes", length);
    }

    // Entry point of SMBIOS 2.1 carries the intermediate anchor of the legacy
    // entry point, which has a checksum of its own
    if ((strcmp(state->entry_spec->anchor, DMI_ANCHOR_V21) == 0) and
        (state->entry_data_size >= sizeof(dmi_entry_v21_t)) and
        not dmi_lint_entry_verify(data + 0x10, 0x0F)) {
        dmi_lint_issue(lint, nullptr, nullptr, 0x10,
                       "checksum of the intermediate anchor does not match its data");
    }
}

static void dmi_lint_entry_length(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    const dmi_context_state_t *state = &dmi_lint_context(lint)->state;
    if (state->entry_spec == nullptr)
        return;

    if (state->entry_length < state->entry_spec->min_length) {
        dmi_lint_issue(lint, nullptr, "length", 0,
                       "entry point of format \"%s\" is %zu bytes long, expected at least %zu",
                       state->entry_spec->name, state->entry_length, state->entry_spec->min_length);
    } else if (state->entry_length > state->entry_data_size) {
        dmi_lint_issue(lint, nullptr, "length", 0,
                       "entry point declares %zu bytes, but only %zu are available",
                       state->entry_length, state->entry_data_size);
    }
}

static void dmi_lint_entry_table_address(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    if (dmi_lint_context(lint)->state.table_area_addr == 0) {
        dmi_lint_issue(lint, nullptr, "table-area-addr", 0,
                       "entry point declares no address of the table");
    }
}

static void dmi_lint_entry_table_size(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    const dmi_context_state_t *state = &dmi_lint_context(lint)->state;

    // Entry point of SMBIOS 3.0 declares the maximum size of the table rather
    // than its size, so the table is allowed to be smaller
    if ((state->table_area_size > 0) and (state->table_area_size != state->table_size)) {
        dmi_lint_issue(lint, nullptr, "table-area-size", 0,
                       "entry point declares a table of %zu bytes, but it is %zu bytes long",
                       state->table_area_size, state->table_size);
    } else if ((state->table_area_max_size > 0) and
               (state->table_size > state->table_area_max_size)) {
        dmi_lint_issue(lint, nullptr, "table-area-max-size", 0,
                       "table of %zu bytes is longer than the maximum of %zu bytes",
                       state->table_size, state->table_area_max_size);
    }
}

static void dmi_lint_entry_entity_count(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    const dmi_context_state_t *state = &dmi_lint_context(lint)->state;
    const dmi_lint_totals_t *totals = dmi_lint_totals(lint);

    // Number of the structures is declared by the entry points of SMBIOS 2.1
    // and earlier only
    if ((state->entity_count > 0) and (state->entity_count != totals->entity_count)) {
        dmi_lint_issue(lint, nullptr, "entity-count", 0,
                       "entry point declares %zu structures, but the table has %zu",
                       state->entity_count, totals->entity_count);
    }
}

static void dmi_lint_entry_entity_max_size(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_unused(entity);

    const dmi_context_state_t *state = &dmi_lint_context(lint)->state;
    const dmi_lint_totals_t *totals = dmi_lint_totals(lint);

    if ((state->entity_max_size > 0) and (totals->entity_max_size > state->entity_max_size)) {
        dmi_lint_issue(lint, nullptr, "entity-max-size", 0,
                       "table has a structure of %zu bytes, but the entry point declares "
                       "a maximum of %zu bytes", totals->entity_max_size, state->entity_max_size);
    }
}
