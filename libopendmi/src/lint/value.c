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
 * @brief Number of the continuation codes leading to the last bank JEDEC has
 * published, see JEP106.
 */
#define DMI_LINT_JEP106_BANKS 8

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

/**
 * @internal
 * @brief Function checking a single value of a structure.
 *
 * @param[in] lint  Check in progress.
 * @param[in] value Value being checked.
 */
typedef void dmi_lint_value_fn(dmi_lint_t *lint, const dmi_lint_value_t *value);

/**
 * @internal
 * @brief Check that the values of the enumerated fields are defined by the
 * specification.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
static void dmi_lint_value_invalid_enum(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the fields hold no values reserved by the specification.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
static void dmi_lint_value_reserved(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the bit fields have no bits reserved by the specification
 * set.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
static void dmi_lint_value_reserved_bits(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the fields encoded as binary-coded decimals hold decimal
 * digits.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
static void dmi_lint_value_bcd(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the UUIDs are set.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
static void dmi_lint_value_uuid(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the fields hold the values the specification allows them.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
static void dmi_lint_value_range(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the identification codes of JEDEC manufacturers carry
 * their parity bit.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
static void dmi_lint_value_jep106(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief State of a walk of the values a check makes.
 */
typedef struct dmi_lint_value_walk
{
    dmi_lint_t         *lint;
    const dmi_entity_t *entity;
    dmi_lint_value_fn  *handler;
} dmi_lint_value_walk_t;

/**
 * @internal
 * @brief Walk the values of a structure, descending into the nested
 * structures and the arrays, and give every value to the handler.
 *
 * @param[in] lint    Check in progress.
 * @param[in] entity  Structure being checked.
 * @param[in] handler Function checking a single value.
 */
static void dmi_lint_value_walk(dmi_lint_t *lint, const dmi_entity_t *entity, dmi_lint_value_fn *handler);

/**
 * @internal
 * @brief Give a value the walk reaches to the handler of the check.
 *
 * @param[in] context State of the walk, see `dmi_lint_value_walk_t`.
 * @param[in] node    Value the walk reaches.
 *
 * @return Always `true`, so that the walk goes on.
 */
static bool dmi_lint_value_visit(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Report an issue on a value, naming the attribute it belongs to, and
 * the index of the element if the value is one of an array.
 *
 * @param[in] lint   Check in progress.
 * @param[in] value  Value the issue is on.
 * @param[in] format Format string of the description of the issue.
 * @param[in] ...    Arguments of the format string.
 */
static void dmi_lint_value_report(
        dmi_lint_t             *lint,
        const dmi_lint_value_t *value,
        const char             *format,
        ...);

/**
 * @internal
 * @brief Tell whether a value stands for "unknown" or "unspecified".
 *
 * @details Such values are valid, whatever the specification says about the
 * rest of the range.
 *
 * @param[in] value Value being checked.
 *
 * @return `true` if the value is a special one, `false` otherwise.
 */
static bool dmi_lint_value_is_special(const dmi_lint_value_t *value);

/**
 * @internal
 * @brief Check that the value of an enumerated field is defined by the
 * specification.
 *
 * @details Enumerations which name some of the values only take the others
 * too.
 *
 * @param[in] lint  Check in progress.
 * @param[in] value Value being checked.
 */
static void dmi_lint_value_check_enum(dmi_lint_t *lint, const dmi_lint_value_t *value);

/**
 * @internal
 * @brief Check that the value of an enumerated field is not reserved by the
 * specification.
 *
 * @param[in] lint  Check in progress.
 * @param[in] value Value being checked.
 */
static void dmi_lint_value_check_reserved(dmi_lint_t *lint, const dmi_lint_value_t *value);

/**
 * @internal
 * @brief Check that a bit field has no bits reserved by the specification set.
 *
 * @param[in] lint  Check in progress.
 * @param[in] value Value being checked.
 */
static void dmi_lint_value_check_bits(dmi_lint_t *lint, const dmi_lint_value_t *value);

/**
 * @internal
 * @brief Check that a field encoded as a binary-coded decimal holds decimal
 * digits.
 *
 * @param[in] lint  Check in progress.
 * @param[in] value Value being checked.
 */
static void dmi_lint_value_check_bcd(dmi_lint_t *lint, const dmi_lint_value_t *value);

/**
 * @internal
 * @brief Check that a UUID is set and present.
 *
 * @param[in] lint  Check in progress.
 * @param[in] value Value being checked.
 */
static void dmi_lint_value_check_uuid(dmi_lint_t *lint, const dmi_lint_value_t *value);

/**
 * @internal
 * @brief Check that a value is within the limits the specification sets for
 * it.
 *
 * @details Limits are values of the field itself, so they are read the same way
 * it is.
 *
 * @param[in] lint  Check in progress.
 * @param[in] value Value being checked.
 */
static void dmi_lint_value_check_range(dmi_lint_t *lint, const dmi_lint_value_t *value);

/**
 * @internal
 * @brief Check that an identification code of a JEDEC manufacturer is a valid
 * JEP106 code.
 *
 * @details Codes of JEP106 are a number of continuation bytes and the code
 * itself, whose high bit makes the number of the set bits odd. A code without
 * it is either taken from the wrong place, or byte-swapped. Codes copied from
 * the SPD carry a parity bit in the number of continuation bytes as well,
 * e.g. 0x80 for the first bank, which is not a part of the number.
 *
 * @param[in] lint  Check in progress.
 * @param[in] value Value being checked.
 */
static void dmi_lint_value_check_jep106(dmi_lint_t *lint, const dmi_lint_value_t *value);

const dmi_lint_rule_t dmi_lint_value_invalid_enum_rule =
{
    .code   = "value.invalid-enum",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_value_invalid_enum,
    .params = {
        .name              = "Values of enumerated fields are defined by the specification",
        .reader_severity   = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_value_reserved_rule =
{
    .code   = "value.reserved",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_value_reserved,
    .params = {
        .name              = "Fields hold no values reserved by the specification",
        .reader_severity   = DMI_LINT_SEVERITY_NOTE,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_value_reserved_bits_rule =
{
    .code   = "value.reserved-bits",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_value_reserved_bits,
    .params = {
        .name              = "Bit fields have no bits reserved by the specification set",
        .reader_severity   = DMI_LINT_SEVERITY_NOTE,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_value_bcd_rule =
{
    .code   = "value.bcd",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_value_bcd,
    .params = {
        .name              = "Fields encoded as binary-coded decimals hold decimal digits",
        .reader_severity   = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_value_range_rule =
{
    .code   = "value.range",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_value_range,
    .params = {
        .name              = "Fields hold the values the specification allows them",
        .reader_severity   = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_value_jep106_rule =
{
    .code   = "value.jep106",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_value_jep106,
    .params = {
        .name              = "Identification codes of JEDEC manufacturers carry their parity bit",
        .reader_severity   = DMI_LINT_SEVERITY_WARNING,
        .producer_severity = DMI_LINT_SEVERITY_ERROR
    }
};

const dmi_lint_rule_t dmi_lint_value_uuid_rule =
{
    .code   = "value.uuid",
    .scope  = DMI_LINT_SCOPE_ENTITY,
    .check  = dmi_lint_value_uuid,
    .params = {
        .name              = "UUIDs are set",
        .reader_severity   = DMI_LINT_SEVERITY_NOTE,
        .producer_severity = DMI_LINT_SEVERITY_WARNING,
        .optional          = true
    }
};

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

static bool dmi_lint_value_is_special(const dmi_lint_value_t *value)
{
    return dmi_attribute_is_unknown(value->attr, value->value) or
           dmi_attribute_is_unspecified(value->attr, value->value);
}

static void dmi_lint_value_walk(dmi_lint_t *lint, const dmi_entity_t *entity, dmi_lint_value_fn *handler)
{
    static const dmi_attribute_visitor_t visitor = {
        .value = dmi_lint_value_visit
    };

    if (entity->spec == nullptr)
        return;

    dmi_lint_value_walk_t walk = {
        .lint    = lint,
        .entity  = entity,
        .handler = handler
    };

    dmi_attributes_walk(entity->spec->attributes, entity->info, &visitor, &walk);
}

static bool dmi_lint_value_visit(void *context, const dmi_attribute_node_t *node)
{
    const dmi_lint_value_walk_t *walk = context;

    walk->handler(walk->lint, &(dmi_lint_value_t){
        .entity = walk->entity,
        .attr   = node->attr,
        .value  = node->value,
        .index  = node->index
    });

    return true;
}

static void dmi_lint_value_check_enum(dmi_lint_t *lint, const dmi_lint_value_t *value)
{
    const dmi_attribute_t *attr = value->attr;

    if ((attr->type != DMI_ATTRIBUTE_TYPE_ENUM) or (attr->params.values == nullptr))
        return;

    // Enumerations which name some of the values only take the others too
    if (attr->params.flags & DMI_ATTRIBUTE_FLAG_OPEN)
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
         (name != nullptr) and (name->code != nullptr); name++) {
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

static void dmi_lint_value_check_range(dmi_lint_t *lint, const dmi_lint_value_t *value)
{
    const dmi_attribute_t *attr = value->attr;

    if ((attr->params.minimum == nullptr) and (attr->params.maximum == nullptr))
        return;

    if (dmi_lint_value_is_special(value))
        return;

    bool is_signed = (attr->params.flags & DMI_ATTRIBUTE_FLAG_SIGNED) != 0;

    if (is_signed) {
        intmax_t actual = dmi_attribute_get_int(attr, value->value);

        if (attr->params.minimum != nullptr) {
            intmax_t minimum = dmi_attribute_get_int(attr, attr->params.minimum);

            if (actual < minimum) {
                dmi_lint_value_report(lint, value, "value %jd is below the minimum of %jd",
                                      actual, minimum);
                return;
            }
        }

        if (attr->params.maximum != nullptr) {
            intmax_t maximum = dmi_attribute_get_int(attr, attr->params.maximum);

            if (actual > maximum)
                dmi_lint_value_report(lint, value, "value %jd is above the maximum of %jd",
                                      actual, maximum);
        }

        return;
    }

    uintmax_t actual = dmi_attribute_get_uint(attr, value->value);

    if (attr->params.minimum != nullptr) {
        uintmax_t minimum = dmi_attribute_get_uint(attr, attr->params.minimum);

        if (actual < minimum) {
            dmi_lint_value_report(lint, value, "value %ju is below the minimum of %ju",
                                  actual, minimum);
            return;
        }
    }

    if (attr->params.maximum != nullptr) {
        uintmax_t maximum = dmi_attribute_get_uint(attr, attr->params.maximum);

        if (actual > maximum)
            dmi_lint_value_report(lint, value, "value %ju is above the maximum of %ju",
                                  actual, maximum);
    }
}

static void dmi_lint_value_range(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_lint_value_walk(lint, entity, dmi_lint_value_check_range);
}

static void dmi_lint_value_check_jep106(dmi_lint_t *lint, const dmi_lint_value_t *value)
{
    const dmi_attribute_t *attr = value->attr;

    if (not (attr->params.flags & DMI_ATTRIBUTE_FLAG_JEP106))
        return;

    if (dmi_lint_value_is_special(value))
        return;

    uintmax_t code = dmi_attribute_get_uint(attr, value->value);
    if (code == 0)
        return;

    // Low byte counts the continuation codes leading to the bank of the
    // manufacturer, and JEDEC has published nine banks
    uintmax_t continuations = code & 0x7F;

    if (continuations > DMI_LINT_JEP106_BANKS) {
        dmi_lint_value_report(lint, value, "code 0x%04jX leads to bank %ju, while JEDEC has "
                              "published %d of them", code, continuations + 1,
                              DMI_LINT_JEP106_BANKS + 1);
        return;
    }

    unsigned bits = 0;

    for (uintmax_t rest = (code >> 8) & 0xFF; rest != 0; rest >>= 1)
        bits += (rest & 1);

    if ((bits % 2) != 0)
        return;

    dmi_lint_value_report(lint, value, "code 0x%04jX has an even number of bits set in its "
                          "identifier, which is no JEP106 code", code);
}

static void dmi_lint_value_jep106(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_lint_value_walk(lint, entity, dmi_lint_value_check_jep106);
}

static void dmi_lint_value_invalid_enum(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_lint_value_walk(lint, entity, dmi_lint_value_check_enum);
}

static void dmi_lint_value_reserved(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_lint_value_walk(lint, entity, dmi_lint_value_check_reserved);
}

static void dmi_lint_value_reserved_bits(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_lint_value_walk(lint, entity, dmi_lint_value_check_bits);
}

static void dmi_lint_value_bcd(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_lint_value_walk(lint, entity, dmi_lint_value_check_bcd);
}

static void dmi_lint_value_uuid(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_lint_value_walk(lint, entity, dmi_lint_value_check_uuid);
}
