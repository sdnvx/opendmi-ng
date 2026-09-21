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

#include <opendmi/lint/quality.h>

static void dmi_lint_quality_placeholder(dmi_lint_t *lint, const dmi_entity_t *entity);

const dmi_lint_rule_t dmi_lint_quality_placeholder_rule =
{
    .code   = "quality.placeholder",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_quality_placeholder,
    .params = {
        .name              = "Strings hold data rather than the placeholders of the firmware vendor",
        .severity          = DMI_LINT_SEVERITY_NOTE,
        .producer_severity = DMI_LINT_SEVERITY_WARNING,
        .optional          = true
    }
};

//
// Strings which firmware leaves in place of the data it has none of. They are
// valid strings, so they are worth a note rather than an error, but nothing
// is to be made of them.
//
static const char *const dmi_lint_placeholders[] =
{
    "To Be Filled By O.E.M.",
    "To be filled by O.E.M.",
    "Filled By OEM",
    "Default string",
    "Default",
    "System manufacturer",
    "System Product Name",
    "System Version",
    "System Serial Number",
    "System SKU Number",
    "Chassis Manufacture",
    "Chassis Version",
    "Chassis Serial Number",
    "Not Specified",
    "Not Applicable",
    "Not Available",
    "None",
    "Unknown",
    "N/A",
    "NA",
    "TBD",
    "OEM",
    "0123456789",
    "123456789",
    "XXXXXXXX",
    nullptr
};

static void dmi_lint_quality_placeholder(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    for (size_t i = 0; i < entity->string_count; i++) {
        const char *text = entity->strings[i].pretty;

        if ((text == nullptr) or (*text == 0))
            continue;

        for (const char *const *item = dmi_lint_placeholders; *item != nullptr; item++) {
            if (strcasecmp(text, *item) != 0)
                continue;

            dmi_lint_issue(lint, entity, nullptr, dmi_lint_string_offset(lint, entity, i + 1),
                           "string %zu holds a placeholder: \"%s\"", i + 1, text);
            break;
        }
    }
}
