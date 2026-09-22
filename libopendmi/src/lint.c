//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/lint.h>
#include <opendmi/registry.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>

#include <opendmi/lint/entity.h>
#include <opendmi/lint/entry.h>
#include <opendmi/lint/link.h>
#include <opendmi/lint/overlay.h>
#include <opendmi/lint/quality.h>
#include <opendmi/lint/string.h>
#include <opendmi/lint/table.h>
#include <opendmi/lint/value.h>

/**
 * @internal
 * @brief Name of the resource table holding the names of the rules.
 */
#define DMI_LINT_RULE_TABLE "lint-rule"

/**
 * @internal
 * @brief Maximum length of the details of an issue.
 */
#define DMI_LINT_MESSAGE_MAX 256

/**
 * @internal
 * @brief State of a single check.
 */
struct dmi_lint
{
    dmi_context_t      *context;
    dmi_lint_options_t  options;
    dmi_lint_issue_fn  *handler;
    void               *data;
    dmi_version_t       version;

    // Rule being checked, which the issues are reported on behalf of
    const dmi_lint_rule_t *rule;

    dmi_lint_totals_t totals;
};

const dmi_name_set_t dmi_lint_severity_names =
{
    .code  = "lint-severity",
    .name  = "Issue severities",
    .names = DMI_NAMES({
        { DMI_LINT_SEVERITY_NONE,    "none",    "None"    },
        { DMI_LINT_SEVERITY_NOTE,    "note",    "note"    },
        { DMI_LINT_SEVERITY_WARNING, "warning", "warning" },
        { DMI_LINT_SEVERITY_ERROR,   "error",   "error"   },
        {}
    })
};

//
// Rules are checked in the order they are registered, so the ones of the
// entry point come before the ones of the table.
//
static const dmi_lint_rule_t *const dmi_lint_rule_list[] =
{
    &dmi_lint_entry_checksum_rule,
    &dmi_lint_entry_length_rule,
    &dmi_lint_entry_table_address_rule,
    &dmi_lint_entry_table_size_rule,
    &dmi_lint_entry_entity_count_rule,
    &dmi_lint_entry_entity_max_size_rule,
    &dmi_lint_table_truncated_rule,
    &dmi_lint_table_terminator_rule,
    &dmi_lint_table_trailing_data_rule,
    &dmi_lint_table_required_rule,
    &dmi_lint_table_recommended_rule,
    &dmi_lint_table_singleton_rule,
    &dmi_lint_table_reserved_handle_rule,
    &dmi_lint_entity_below_minimum_rule,
    &dmi_lint_entity_unknown_length_rule,
    &dmi_lint_entity_undecoded_rule,
    &dmi_lint_entity_newer_fields_rule,
    &dmi_lint_entity_newer_type_rule,
    &dmi_lint_entity_unknown_type_rule,
    &dmi_lint_entity_obsolete_rule,
    &dmi_lint_string_too_long_rule,
    &dmi_lint_string_non_printable_rule,
    &dmi_lint_string_blank_rule,
    &dmi_lint_string_padded_rule,
    &dmi_lint_string_unreferenced_rule,
    &dmi_lint_value_invalid_enum_rule,
    &dmi_lint_value_reserved_rule,
    &dmi_lint_value_reserved_bits_rule,
    &dmi_lint_value_bcd_rule,
    &dmi_lint_value_range_rule,
    &dmi_lint_value_jep106_rule,
    &dmi_lint_value_uuid_rule,
    &dmi_lint_link_dangling_rule,
    &dmi_lint_link_wrong_type_rule,
    &dmi_lint_link_self_rule,
    &dmi_lint_link_orphan_rule,
    &dmi_lint_overlay_dangling_rule,
    &dmi_lint_overlay_out_of_bounds_rule,
    &dmi_lint_overlay_empty_rule,
    &dmi_lint_quality_placeholder_rule,
    nullptr
};

/**
 * @internal
 * @brief Check whether a rule is checked at all: optional rules are left out
 * unless all checks are enabled, and the caller has the last word.
 */
static bool dmi_lint_enabled(
        const dmi_lint_t      *lint,
        const dmi_lint_rule_t *rule);

/**
 * @internal
 * @brief Check every enabled rule of the scope, keeping the rule being
 * checked, so that the issues are reported on behalf of it.
 */
static void dmi_lint_check_scope(
        dmi_lint_t         *lint,
        dmi_lint_scope_t    scope,
        const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check a list of rules, which the specification of a type provides.
 */
static void dmi_lint_check_rules(
        dmi_lint_t            *lint,
        const dmi_lint_rule_t *rules,
        const dmi_entity_t    *entity);

/**
 * @internal
 * @brief Check a single rule, keeping it as the one the issues are reported
 * on behalf of.
 */
static void dmi_lint_check_rule(
        dmi_lint_t            *lint,
        const dmi_lint_rule_t *rule,
        const dmi_entity_t    *entity);

/**
 * @internal
 * @brief Gather the totals of the table, which every rule is free to use,
 * whichever part of the data it belongs to.
 */
static void dmi_lint_collect(dmi_lint_t *lint);

/**
 * @internal
 * @brief Check the rules of every structure of the table.
 */
static void dmi_lint_check_entities(dmi_lint_t *lint);

bool dmi_lint(
        dmi_context_t            *context,
        const dmi_lint_options_t *options,
        dmi_lint_issue_fn        *handler,
        void                     *data)
{
    if (context == nullptr)
        return false;

    if (context->state.table_data == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_STATE, "Context is not open");
        return false;
    }

    dmi_lint_t lint =
    {
        .context = context,
        .handler = handler,
        .data    = data
    };

    if (options != nullptr)
        lint.options = *options;

    // Data is checked against the version of the entry point, unless the
    // caller asks for another one
    lint.version = (lint.options.version != DMI_VERSION_NONE)
            ? lint.options.version
            : context->state.smbios_version;

    // Totals of the table are gathered before anything is checked, since the
    // rules of the entry point are checked against them as well
    dmi_lint_collect(&lint);

    dmi_lint_check_scope(&lint, DMI_LINT_SCOPE_ENTRY, nullptr);
    dmi_lint_check_entities(&lint);
    dmi_lint_check_scope(&lint, DMI_LINT_SCOPE_TABLE, nullptr);

    return true;
}

void dmi_lint_issue(
        dmi_lint_t         *lint,
        const dmi_entity_t *entity,
        const char         *attribute,
        size_t              offset,
        const char         *format,
        ...)
{
    assert(lint != nullptr);
    assert(lint->rule != nullptr);

    if (lint->handler == nullptr)
        return;

    char message[DMI_LINT_MESSAGE_MAX];
    va_list args;

    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    dmi_lint_issue_t issue =
    {
        .rule      = lint->rule,
        .severity  = dmi_lint_rule_severity(lint->rule, lint->options.profile),
        .handle    = (entity != nullptr) ? dmi_entity_handle(entity) : DMI_HANDLE_INVALID,
        .type      = (entity != nullptr) ? dmi_entity_type(entity) : DMI_TYPE_INVALID,
        .attribute = attribute,
        .offset    = offset,
        .message   = message
    };

    lint->handler(lint->data, &issue);
}

dmi_context_t *dmi_lint_context(dmi_lint_t *lint)
{
    return (lint != nullptr) ? lint->context : nullptr;
}

const dmi_lint_totals_t *dmi_lint_totals(const dmi_lint_t *lint)
{
    return (lint != nullptr) ? &lint->totals : nullptr;
}

dmi_version_t dmi_lint_version(const dmi_lint_t *lint)
{
    return (lint != nullptr) ? lint->version : DMI_VERSION_NONE;
}

size_t dmi_lint_entity_offset(const dmi_lint_t *lint, const dmi_entity_t *entity)
{
    if ((lint == nullptr) or (entity == nullptr))
        return DMI_LINT_NO_OFFSET;

    const dmi_data_t *table = lint->context->state.table_data;

    if ((table == nullptr) or (entity->data == nullptr) or (entity->data < table))
        return DMI_LINT_NO_OFFSET;

    return (size_t)(entity->data - table);
}

size_t dmi_lint_string_offset(const dmi_lint_t *lint, const dmi_entity_t *entity, size_t num)
{
    size_t offset = dmi_lint_entity_offset(lint, entity);

    if ((offset == DMI_LINT_NO_OFFSET) or (num == 0) or (num > entity->string_count))
        return DMI_LINT_NO_OFFSET;

    // Strings follow the formatted section, each one terminated with a zero
    offset += entity->body_length;

    for (size_t i = 0; i + 1 < num; i++) {
        const char *text = entity->strings[i].raw;

        offset += ((text != nullptr) ? strlen(text) : 0) + 1;
    }

    return offset;
}

const dmi_lint_rule_t *const *dmi_lint_rules(void)
{
    return dmi_lint_rule_list;
}

const char *dmi_lint_rule_name(const dmi_lint_rule_t *rule)
{
    if (rule == nullptr)
        return nullptr;

    // Names of the rules are translated, if the locale has a translation for
    // the rule, while codes are machine-readable and are never translated
    const char *translated = dmi_locale_string(DMI_LINT_RULE_TABLE, rule->code);

    return (translated != nullptr) ? translated : rule->params.name;
}

const dmi_lint_rule_t *dmi_lint_rule_find(dmi_context_t *context, const char *code)
{
    if (code == nullptr)
        return nullptr;

    for (const dmi_lint_rule_t *const *rule = dmi_lint_rule_list; *rule != nullptr; rule++) {
        if (strcmp((*rule)->code, code) == 0)
            return *rule;
    }

    if (context == nullptr)
        return nullptr;

    // Rules of the types are provided by their specifications, and the types
    // the context knows depend on the modules it has enabled
    for (dmi_type_t type = 0; type <= DMI_TYPE_MAX; type++) {
        const dmi_entity_spec_t *spec = dmi_type_spec(context, type);

        if ((spec == nullptr) or (spec->lint_rules == nullptr))
            continue;

        for (const dmi_lint_rule_t *rule = spec->lint_rules; rule->code != nullptr; rule++) {
            if (strcmp(rule->code, code) == 0)
                return rule;
        }
    }

    return nullptr;
}

dmi_lint_severity_t dmi_lint_rule_severity(const dmi_lint_rule_t *rule, dmi_lint_profile_t profile)
{
    if (rule == nullptr)
        return DMI_LINT_SEVERITY_NONE;

    return (profile == DMI_LINT_PROFILE_PRODUCER) ? rule->params.producer_severity : rule->params.severity;
}

static bool dmi_lint_enabled(const dmi_lint_t *lint, const dmi_lint_rule_t *rule)
{
    if (rule->check == nullptr)
        return false;

    if (rule->params.optional and not lint->options.all)
        return false;

    if (dmi_lint_rule_severity(rule, lint->options.profile) == DMI_LINT_SEVERITY_NONE)
        return false;

    if (lint->options.rule_filter != nullptr)
        return lint->options.rule_filter(lint->options.rule_filter_data, rule);

    return true;
}

static void dmi_lint_check_scope(dmi_lint_t *lint, dmi_lint_scope_t scope, const dmi_entity_t *entity)
{
    for (const dmi_lint_rule_t *const *rule = dmi_lint_rule_list; *rule != nullptr; rule++) {
        if ((*rule)->scope != scope)
            continue;

        dmi_lint_check_rule(lint, *rule, entity);
    }
}

static void dmi_lint_check_rules(
        dmi_lint_t            *lint,
        const dmi_lint_rule_t *rules,
        const dmi_entity_t    *entity)
{
    if (rules == nullptr)
        return;

    for (const dmi_lint_rule_t *rule = rules; rule->code != nullptr; rule++)
        dmi_lint_check_rule(lint, rule, entity);
}

static void dmi_lint_check_rule(
        dmi_lint_t            *lint,
        const dmi_lint_rule_t *rule,
        const dmi_entity_t    *entity)
{
    if (not dmi_lint_enabled(lint, rule))
        return;

    lint->rule = rule;
    rule->check(lint, entity);
    lint->rule = nullptr;
}

static void dmi_lint_collect(dmi_lint_t *lint)
{
    dmi_registry_t *registry = dmi_get_registry(lint->context);
    if (registry == nullptr)
        return;

    dmi_registry_iter_t iter;
    const dmi_entity_t *entity;

    if (not dmi_registry_iter_init(&iter, registry, nullptr))
        return;

    while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
        dmi_type_t type = dmi_entity_type(entity);

        lint->totals.entity_count++;
        lint->totals.type_counts[type]++;

        if (entity->total_length > lint->totals.entity_max_size)
            lint->totals.entity_max_size = entity->total_length;

        if ((type == DMI_TYPE_END_OF_TABLE) and (lint->totals.terminator == nullptr))
            lint->totals.terminator = entity;
    }
}

static void dmi_lint_check_entities(dmi_lint_t *lint)
{
    dmi_registry_t *registry = dmi_get_registry(lint->context);
    if (registry == nullptr)
        return;

    dmi_registry_iter_t iter;
    const dmi_entity_t *entity;

    if (not dmi_registry_iter_init(&iter, registry, nullptr))
        return;

    while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
        dmi_lint_check_scope(lint, DMI_LINT_SCOPE_ENTITY, entity);

        // Rules of the type know the structure itself, so they follow the
        // ones which apply to any structure
        if (entity->spec != nullptr)
            dmi_lint_check_rules(lint, entity->spec->lint_rules, entity);
    }
}
