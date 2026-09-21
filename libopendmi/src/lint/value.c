//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>

#include <opendmi/attribute.h>
#include <opendmi/context.h>
#include <opendmi/lint.h>
#include <opendmi/internal.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/uuid.h>

#include <opendmi/lint/value.h>

/**
 * @internal
 * @brief Code name of the entries standing for the values the specification
 * reserves.
 */
#define DMI_LINT_RESERVED_CODE "reserved"

/**
 * @internal
 * @brief Value being checked, along with the attribute describing it and the
 * structure it belongs to.
 */
typedef struct dmi_lint_value
{
    const dmi_entity_t    *entity;
    const dmi_attribute_t *attr;
    const void            *value;

    // Index of the element within the array, or SIZE_MAX for a plain value
    size_t index;
} dmi_lint_value_t;

typedef void dmi_lint_value_fn(dmi_lint_t *lint, const dmi_lint_value_t *value);

static void dmi_lint_value_invalid_enum(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_value_reserved(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_value_reserved_bits(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_value_bcd(dmi_lint_t *lint, const dmi_entity_t *entity);
static void dmi_lint_value_uuid(dmi_lint_t *lint, const dmi_entity_t *entity);

static void dmi_lint_value_walk(
        dmi_lint_t            *lint,
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attrs,
        const void            *info,
        dmi_lint_value_fn     *handler);

static void dmi_lint_value_report(
        dmi_lint_t             *lint,
        const dmi_lint_value_t *value,
        const char             *format,
        ...);

static bool dmi_lint_value_is_special(const dmi_lint_value_t *value);

const dmi_lint_rule_t dmi_lint_value_invalid_enum_rule =
{
    .code              = "value.invalid-enum",
    .name              = "Values of enumerated fields are defined by the specification",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_value_invalid_enum
};

const dmi_lint_rule_t dmi_lint_value_reserved_rule =
{
    .code              = "value.reserved",
    .name              = "Fields hold no values reserved by the specification",
    .severity          = DMI_LINT_SEVERITY_NOTE,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_value_reserved
};

const dmi_lint_rule_t dmi_lint_value_reserved_bits_rule =
{
    .code              = "value.reserved-bits",
    .name              = "Bit fields have no bits reserved by the specification set",
    .severity          = DMI_LINT_SEVERITY_NOTE,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_value_reserved_bits
};

const dmi_lint_rule_t dmi_lint_value_bcd_rule =
{
    .code              = "value.bcd",
    .name              = "Fields encoded as binary-coded decimals hold decimal digits",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_value_bcd
};

const dmi_lint_rule_t dmi_lint_value_uuid_rule =
{
    .code              = "value.uuid",
    .name              = "UUIDs are set",
    .severity          = DMI_LINT_SEVERITY_NOTE,
    .producer_severity = DMI_LINT_SEVERITY_WARNING,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_lint_value_uuid,
    .optional          = true
};

//
// Report an issue on a value, naming the attribute it belongs to, and the
// index of the element if the value is one of an array.
//
static void dmi_lint_value_report(
        dmi_lint_t             *lint,
        const dmi_lint_value_t *value,
        const char             *format,
        ...)
{
    char details[192];
    va_list args;

    va_start(args, format);
    vsnprintf(details, sizeof(details), format, args);
    va_end(args);

    size_t offset = dmi_lint_entity_offset(lint, value->entity);

    if (value->index == SIZE_MAX) {
        dmi_lint_issue(lint, value->entity, value->attr->params.code, offset, "%s", details);
    } else {
        dmi_lint_issue(lint, value->entity, value->attr->params.code, offset,
                       "item %zu: %s", value->index, details);
    }
}

//
// Values standing for "unknown" and "unspecified" are valid, whatever the
// specification says about the rest of the range.
//
static bool dmi_lint_value_is_special(const dmi_lint_value_t *value)
{
    return dmi_attribute_is_unknown(value->attr, value->value) or
           dmi_attribute_is_unspecified(value->attr, value->value);
}

//
// Walk the values of a structure, descending into the nested structures and
// the arrays, and hand every value to the handler.
//
static void dmi_lint_value_walk(
        dmi_lint_t            *lint,
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attrs,
        const void            *info,
        dmi_lint_value_fn     *handler)
{
    if ((attrs == nullptr) or (info == nullptr))
        return;

    for (const dmi_attribute_t *attr = attrs; attr->params.name != nullptr; attr++) {
        const dmi_attribute_t *resolved = dmi_attribute_resolve(attr, info);
        if (resolved == nullptr)
            continue;

        const dmi_data_t *ptr = dmi_member_ptr(info, resolved->value, dmi_data_t);

        // Arrays keep their elements apart from the structure, and an empty
        // array is not allocated at all
        if (dmi_member_is_present(resolved->counter)) {
            const dmi_data_t *element = dmi_deref(dmi_data_t *, ptr);
            size_t count = (element != nullptr) ? dmi_attribute_get_count(resolved, info) : 0;

            for (size_t i = 0; i < count; i++, element += resolved->value.size) {
                if (resolved->type == DMI_ATTRIBUTE_TYPE_STRUCT) {
                    dmi_lint_value_walk(lint, entity, resolved->params.attrs, element, handler);
                } else {
                    handler(lint, &(dmi_lint_value_t){
                        .entity = entity, .attr = resolved, .value = element, .index = i
                    });
                }
            }

            continue;
        }

        if (resolved->type == DMI_ATTRIBUTE_TYPE_STRUCT) {
            dmi_lint_value_walk(lint, entity, resolved->params.attrs, ptr, handler);
            continue;
        }

        handler(lint, &(dmi_lint_value_t){
            .entity = entity, .attr = resolved, .value = ptr, .index = SIZE_MAX
        });
    }
}

static void dmi_lint_value_check_enum(dmi_lint_t *lint, const dmi_lint_value_t *value)
{
    const dmi_attribute_t *attr = value->attr;

    if ((attr->type != DMI_ATTRIBUTE_TYPE_ENUM) or (attr->params.values == nullptr))
        return;

    if (dmi_lint_value_is_special(value))
        return;

    int id = (int)dmi_attribute_get_int(attr, value->value);

    if (dmi_code_lookup(attr->params.values, id) == nullptr) {
        dmi_lint_value_report(lint, value, "value 0x%X is defined by no version of the "
                              "specification", (unsigned)id);
    }
}

static void dmi_lint_value_check_reserved(dmi_lint_t *lint, const dmi_lint_value_t *value)
{
    const dmi_attribute_t *attr = value->attr;

    if ((attr->type != DMI_ATTRIBUTE_TYPE_ENUM) or (attr->params.values == nullptr))
        return;

    if (dmi_lint_value_is_special(value))
        return;

    int id = (int)dmi_attribute_get_int(attr, value->value);
    const char *code = dmi_code_lookup(attr->params.values, id);

    if ((code != nullptr) and (strcmp(code, DMI_LINT_RESERVED_CODE) == 0)) {
        dmi_lint_value_report(lint, value, "value 0x%X is reserved by the specification",
                              (unsigned)id);
    }
}

static void dmi_lint_value_check_bits(dmi_lint_t *lint, const dmi_lint_value_t *value)
{
    const dmi_attribute_t *attr = value->attr;

    if ((attr->type != DMI_ATTRIBUTE_TYPE_SET) or (attr->params.values == nullptr))
        return;

    uintmax_t bits = dmi_attribute_get_uint(attr, value->value);
    uintmax_t known = 0;
    int last = -1;

    // Bits of a set are named by their numbers, so the ones the dictionary
    // has no name for are the reserved ones
    for (const dmi_name_t *name = attr->params.values->names;
         (name != nullptr) and (name->id >= 0); name++) {
        if ((size_t)name->id >= sizeof(bits) * CHAR_BIT)
            continue;

        known |= (uintmax_t)1 << name->id;

        if (name->id > last)
            last = name->id;
    }

    if (last < 0)
        return;

    // Bits above the ones the dictionary describes are left to the vendor of
    // the firmware or of the system, e.g. bits 32 and higher of the firmware
    // characteristics, so only the gaps within the described range count
    uintmax_t described = ((size_t)last + 1 < sizeof(bits) * CHAR_BIT)
            ? (((uintmax_t)1 << (last + 1)) - 1) : UINTMAX_MAX;

    uintmax_t reserved = bits & described & ~known;

    if (reserved != 0) {
        dmi_lint_value_report(lint, value, "reserved bits 0x%jX are set", reserved);
    }
}

static void dmi_lint_value_check_bcd(dmi_lint_t *lint, const dmi_lint_value_t *value)
{
    const dmi_attribute_t *attr = value->attr;

    if (not (attr->params.flags & DMI_ATTRIBUTE_FLAG_BCD))
        return;

    if (dmi_lint_value_is_special(value))
        return;

    uintmax_t bcd = dmi_attribute_get_uint(attr, value->value);

    for (uintmax_t rest = bcd; rest != 0; rest >>= 4) {
        if ((rest & 0x0F) <= 9)
            continue;

        dmi_lint_value_report(lint, value, "value 0x%jX is not a binary-coded decimal", bcd);
        break;
    }
}

static void dmi_lint_value_check_uuid(dmi_lint_t *lint, const dmi_lint_value_t *value)
{
    if (value->attr->type != DMI_ATTRIBUTE_TYPE_UUID)
        return;

    const dmi_uuid_t *uuid = value->value;

    size_t zeroes = 0;
    size_t ones   = 0;

    for (size_t i = 0; i < sizeof(uuid->__value); i++) {
        zeroes += (uuid->__value[i] == 0x00) ? 1 : 0;
        ones   += (uuid->__value[i] == 0xFF) ? 1 : 0;
    }

    // The specification uses both values as markers, so they are no defect on
    // their own, but they carry no identifier either
    if (zeroes == sizeof(uuid->__value)) {
        dmi_lint_value_report(lint, value, "UUID is not set");
    } else if (ones == sizeof(uuid->__value)) {
        dmi_lint_value_report(lint, value, "UUID is not present");
    }
}

static void dmi_lint_value_invalid_enum(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_lint_value_walk(lint, entity, entity->spec ? entity->spec->attributes : nullptr,
                        entity->info, dmi_lint_value_check_enum);
}

static void dmi_lint_value_reserved(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_lint_value_walk(lint, entity, entity->spec ? entity->spec->attributes : nullptr,
                        entity->info, dmi_lint_value_check_reserved);
}

static void dmi_lint_value_reserved_bits(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_lint_value_walk(lint, entity, entity->spec ? entity->spec->attributes : nullptr,
                        entity->info, dmi_lint_value_check_bits);
}

static void dmi_lint_value_bcd(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_lint_value_walk(lint, entity, entity->spec ? entity->spec->attributes : nullptr,
                        entity->info, dmi_lint_value_check_bcd);
}

static void dmi_lint_value_uuid(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_lint_value_walk(lint, entity, entity->spec ? entity->spec->attributes : nullptr,
                        entity->info, dmi_lint_value_check_uuid);
}
