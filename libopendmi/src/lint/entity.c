//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/lint.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>

#include <stdio.h>
#include <opendmi/utils/version.h>

#include <opendmi/lint/entity.h>

static void dmi_lint_entity_below_minimum(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_entity_unknown_length(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_entity_undecoded(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_entity_newer_fields(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_entity_newer_type(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_entity_unknown_type(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_entity_obsolete(dmi_lint_t *lint, const dmi_entity_t *entity);

static char *dmi_lint_entity_version(dmi_version_t version, char *buffer, size_t size);

const dmi_lint_rule_t dmi_lint_entity_below_minimum_rule =
{
    .code              = "entity.below-minimum",
    .name              = "Structure is long enough for its type",
    .severity          = DMI_LINT_SEVERITY_ERROR,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_entity_below_minimum
};

const dmi_lint_rule_t dmi_lint_entity_unknown_length_rule =
{
    .code              = "entity.unknown-length",
    .name              = "Length of the structure matches a version of its specification",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_entity_unknown_length
};

const dmi_lint_rule_t dmi_lint_entity_undecoded_rule =
{
    .code              = "entity.undecoded",
    .name              = "Structure has been decoded",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_entity_undecoded
};

const dmi_lint_rule_t dmi_lint_entity_newer_fields_rule =
{
    .code              = "entity.newer-fields",
    .name              = "Structure has no fields newer than the version of the entry point",
    .severity          = DMI_LINT_SEVERITY_NOTE,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_entity_newer_fields
};

const dmi_lint_rule_t dmi_lint_entity_newer_type_rule =
{
    .code              = "entity.newer-type",
    .name              = "Type of the structure is defined by the version of the entry point",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_entity_newer_type
};

const dmi_lint_rule_t dmi_lint_entity_unknown_type_rule =
{
    .code              = "entity.unknown-type",
    .name              = "Type of the structure is known",
    .severity          = DMI_LINT_SEVERITY_NOTE,
    .producer_severity = DMI_LINT_SEVERITY_WARNING,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_entity_unknown_type,
    .optional          = true
};

const dmi_lint_rule_t dmi_lint_entity_obsolete_rule =
{
    .code              = "entity.obsolete",
    .name              = "Structure is not of a type obsoleted by the specification",
    .severity          = DMI_LINT_SEVERITY_NOTE,
    .producer_severity = DMI_LINT_SEVERITY_WARNING,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_entity_obsolete,
    .optional          = true
};

//
// Types which the specification keeps for backwards compatibility only, and
// which are not to be used by new firmware.
//
static const dmi_type_t dmi_lint_obsolete_types[] =
{
    DMI_TYPE_MEMORY_CONTROLLER,
    DMI_TYPE_MEMORY_MODULE,
    DMI_TYPE_ONBOARD_DEVICE
};

//
// Version of the specification as it is printed in the issues, which falls
// back to the number itself if it cannot be formatted.
//
static char *dmi_lint_entity_version(dmi_version_t version, char *buffer, size_t size)
{
    char *text = dmi_version_format(version);

    if (text != nullptr) {
        snprintf(buffer, size, "%s", text);
        dmi_free(text);
    } else {
        snprintf(buffer, size, "0x%X", (unsigned)version);
    }

    return buffer;
}

static void dmi_lint_entity_below_minimum(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_entity_spec_t *spec = entity->spec;

    if ((spec == nullptr) or (spec->minimum_length == 0))
        return;

    if (entity->body_length < spec->minimum_length) {
        dmi_lint_issue(lint, entity, nullptr, dmi_lint_entity_offset(lint, entity),
                       "structure is %zu bytes long, but its type needs at least %zu",
                       entity->body_length, spec->minimum_length);
    }
}

static void dmi_lint_entity_unknown_length(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    // Data of an incomplete structure ends in the middle of a set of fields,
    // so its length matches no version of the specification
    if (entity->state & DMI_ENTITY_STATE_INCOMPLETE) {
        dmi_lint_issue(lint, entity, nullptr, dmi_lint_entity_offset(lint, entity),
                       "length of %zu bytes matches no version of the specification",
                       entity->body_length);
    }
}

static void dmi_lint_entity_undecoded(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    // Structures which carry no data of their own, e.g. the end-of-table one,
    // have nothing to decode
    if ((entity->spec == nullptr) or (entity->spec->handlers.decode == nullptr))
        return;

    if (entity->state & DMI_ENTITY_STATE_DECODED)
        return;

    dmi_lint_issue(lint, entity, nullptr, dmi_lint_entity_offset(lint, entity),
                   "structure has not been decoded");
}

static void dmi_lint_entity_newer_fields(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_version_t version = dmi_lint_version(lint);

    if ((entity->level == DMI_VERSION_NONE) or (entity->level <= version))
        return;

    char level[16];
    char entry[16];

    dmi_lint_issue(lint, entity, nullptr, dmi_lint_entity_offset(lint, entity),
                   "structure has fields of SMBIOS %s, while the entry point declares %s",
                   dmi_lint_entity_version(entity->level, level, sizeof(level)),
                   dmi_lint_entity_version(version, entry, sizeof(entry)));
}

static void dmi_lint_entity_newer_type(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_entity_spec_t *spec = entity->spec;
    dmi_version_t version = dmi_lint_version(lint);

    if ((spec == nullptr) or (spec->minimum_version == DMI_VERSION_NONE) or
        (spec->minimum_version <= version))
        return;

    char minimum[16];
    char entry[16];

    dmi_lint_issue(lint, entity, nullptr, dmi_lint_entity_offset(lint, entity),
                   "type is defined by SMBIOS %s, while the entry point declares %s",
                   dmi_lint_entity_version(spec->minimum_version, minimum, sizeof(minimum)),
                   dmi_lint_entity_version(version, entry, sizeof(entry)));
}

static void dmi_lint_entity_unknown_type(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    if (entity->spec != nullptr)
        return;

    dmi_type_t type = dmi_entity_type(entity);

    // OEM-specific structures are known to their vendor only, so the module
    // providing them may be disabled rather than missing
    dmi_lint_issue(lint, entity, nullptr, dmi_lint_entity_offset(lint, entity),
                   (type >= __DMI_TYPE_OEM_START)
                        ? "no enabled module describes OEM-specific type %d"
                        : "no specification describes type %d", (int)type);
}

static void dmi_lint_entity_obsolete(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_type_t type = dmi_entity_type(entity);

    for (size_t i = 0; i < countof(dmi_lint_obsolete_types); i++) {
        if (type != dmi_lint_obsolete_types[i])
            continue;

        dmi_lint_issue(lint, entity, nullptr, dmi_lint_entity_offset(lint, entity),
                       "type %d (%s) is obsolete", (int)type,
                       dmi_type_name(dmi_lint_context(lint), type));
        break;
    }
}
