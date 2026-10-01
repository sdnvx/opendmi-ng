//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <strings.h>

#include <opendmi/context.h>
#include <opendmi/lint.h>
#include <opendmi/internal.h>
#include <opendmi/utils/string.h>

#include <opendmi/lint/quality.h>

/**
 * @internal
 * @brief Check that the strings of a structure hold data rather than the
 * placeholders of the firmware vendor, such as "To Be Filled By O.E.M.".
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
static void dmi_lint_quality_placeholder(dmi_lint_t *lint, const dmi_entity_t *entity);

const dmi_lint_rule_t dmi_lint_quality_placeholder_rule =
{
    .code   = "quality.placeholder",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_quality_placeholder,
    .params = {
        .name              = "Strings hold data rather than the placeholders of the firmware vendor",
        .reader_severity   = DMI_LINT_SEVERITY_NOTE,
        .producer_severity = DMI_LINT_SEVERITY_WARNING,
        .optional          = true
    }
};

static void dmi_lint_quality_placeholder(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    for (size_t i = 0; i < entity->string_count; i++) {
        const char *text = entity->strings[i].pretty;

        if ((text == nullptr) or (*text == 0))
            continue;

        if (dmi_string_is_placeholder(text)) {
            dmi_lint_issue(lint, entity, nullptr, dmi_lint_string_offset(lint, entity, i + 1),
                           "string %zu holds a placeholder: \"%s\"", i + 1, text);
        }
    }
}
