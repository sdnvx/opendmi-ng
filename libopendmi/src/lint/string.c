//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <ctype.h>

#include <opendmi/context.h>
#include <opendmi/lint.h>
#include <opendmi/internal.h>

#include <opendmi/lint/string.h>

/**
 * @internal
 * @brief Maximum length of a string, as the specification puts it.
 */
#define DMI_LINT_STRING_MAX 64

static void dmi_lint_string_too_long(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_string_non_printable(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_string_blank(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_string_padded(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_string_unreferenced(dmi_lint_t *lint, const dmi_entity_t *entity);


const dmi_lint_rule_t dmi_lint_string_too_long_rule =
{
    .code              = "string.too-long",
    .name              = "Strings are no longer than the specification allows",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_string_too_long
};

const dmi_lint_rule_t dmi_lint_string_non_printable_rule =
{
    .code              = "string.non-printable",
    .name              = "Strings hold printable text",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_string_non_printable
};

const dmi_lint_rule_t dmi_lint_string_blank_rule =
{
    .code              = "string.blank",
    .name              = "Strings are not blank",
    .severity          = DMI_LINT_SEVERITY_NOTE,
    .producer_severity = DMI_LINT_SEVERITY_WARNING,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_string_blank
};

const dmi_lint_rule_t dmi_lint_string_padded_rule =
{
    .code              = "string.padded",
    .name              = "Strings have no leading or trailing whitespace",
    .severity          = DMI_LINT_SEVERITY_NOTE,
    .producer_severity = DMI_LINT_SEVERITY_WARNING,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_string_padded,
    .optional          = true
};

const dmi_lint_rule_t dmi_lint_string_unreferenced_rule =
{
    .code              = "string.unreferenced",
    .name              = "Every string is referenced by a field of its structure",
    .severity          = DMI_LINT_SEVERITY_NOTE,
    .producer_severity = DMI_LINT_SEVERITY_WARNING,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_string_unreferenced,
    .optional          = true
};

static void dmi_lint_string_too_long(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    for (size_t i = 0; i < entity->string_count; i++) {
        const char *text = entity->strings[i].raw;
        if (text == nullptr)
            continue;

        size_t length = strlen(text);

        if (length > DMI_LINT_STRING_MAX) {
            dmi_lint_issue(lint, entity, nullptr, dmi_lint_string_offset(lint, entity, i + 1),
                           "string %zu is %zu characters long, expected at most %d",
                           i + 1, length, DMI_LINT_STRING_MAX);
        }
    }
}

static void dmi_lint_string_non_printable(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    for (size_t i = 0; i < entity->string_count; i++) {
        const char *text = entity->strings[i].raw;
        if (text == nullptr)
            continue;

        for (const char *ptr = text; *ptr != 0; ptr++) {
            // Text of the strings is printable, while anything else is data
            // which has ended up in the string table
            if (isprint((unsigned char)*ptr) or (*ptr & 0x80))
                continue;

            dmi_lint_issue(lint, entity, nullptr, dmi_lint_string_offset(lint, entity, i + 1),
                           "string %zu holds a non-printable character 0x%02X at position %zu",
                           i + 1, (unsigned char)*ptr, (size_t)(ptr - text));
            break;
        }
    }
}

static void dmi_lint_string_blank(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    for (size_t i = 0; i < entity->string_count; i++) {
        const char *raw = entity->strings[i].raw;
        const char *pretty = entity->strings[i].pretty;

        // Strings are trimmed when they are read, so a blank string is left
        // empty while its raw value is not
        if ((raw == nullptr) or (*raw == 0) or (pretty == nullptr) or (*pretty != 0))
            continue;

        dmi_lint_issue(lint, entity, nullptr, dmi_lint_string_offset(lint, entity, i + 1),
                       "string %zu holds whitespace only", i + 1);
    }
}

static void dmi_lint_string_padded(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    for (size_t i = 0; i < entity->string_count; i++) {
        const char *raw = entity->strings[i].raw;
        const char *pretty = entity->strings[i].pretty;

        if ((raw == nullptr) or (pretty == nullptr) or (*pretty == 0))
            continue;

        if (strcmp(raw, pretty) != 0) {
            dmi_lint_issue(lint, entity, nullptr, dmi_lint_string_offset(lint, entity, i + 1),
                           "string %zu is padded with whitespace", i + 1);
        }
    }
}

static void dmi_lint_string_unreferenced(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    // Strings of a structure which has not been decoded are referenced by
    // nothing at all
    if ((entity->info == nullptr) or (entity->spec == nullptr))
        return;

    for (size_t i = 0; i < entity->string_count; i++) {
        if (entity->strings[i].used)
            continue;

        dmi_lint_issue(lint, entity, nullptr, dmi_lint_string_offset(lint, entity, i + 1),
                       "string %zu is referenced by no field of the structure", i + 1);
    }
}
