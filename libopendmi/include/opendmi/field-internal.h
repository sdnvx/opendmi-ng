//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_FIELD_INTERNAL_H
#define OPENDMI_FIELD_INTERNAL_H

#pragma once

/**
 * @file
 * @internal
 * @brief Definitions the decoding and the encoding of fields share, see
 * field.c, field-decode.c and field-encode.c.
 */

#include <limits.h>
#include <stdint.h>
#include <stddef.h>

#include <opendmi/entity.h>
#include <opendmi/field.h>
#include <opendmi/internal.h>

/**
 * @internal
 * @brief Largest number of the members an extended field may replace the value
 * of, which is a handful in the structures the specification defines.
 */
#define DMI_FIELD_CHOICE_MAX 8

/**
 * @internal
 * @brief Member whose plain field decides whether the extended fields keyed on
 * it carry the real values. Decoding keeps the value the plain field has read,
 * because the member no longer holds it once it has been converted, and
 * encoding the representation the plain field has chosen.
 */
typedef struct dmi_field_choice
{
    size_t    offset;
    uintmax_t when_raw;
    uintmax_t raw;
    bool      extended;
} dmi_field_choice_t;

/**
 * @internal
 * @brief Members of a list of fields the extended fields of the list are keyed on.
 */
typedef struct dmi_field_choices
{
    dmi_field_choice_t items[DMI_FIELD_CHOICE_MAX];
    unsigned           count;
} dmi_field_choices_t;

/**
 * @internal
 * @brief Offset of the member whose plain field says that an extended one
 * carries the real value, which is the member of the field itself unless the
 * specification keys one field on another.
 */
static inline size_t dmi_field_when_offset(const dmi_field_t *field)
{
    return dmi_member_is_present(field->params.when)
            ? field->params.when.offset
            : field->member.offset;
}

/**
 * @internal
 * @brief Whether a range of bits is defined in a table of the version, which
 * the ranges the specification has given the reserved bits a meaning in, or
 * has widened, are not in the tables of the other versions.
 */
static inline bool dmi_field_is_defined(const dmi_field_t *field, dmi_version_t version)
{
    if ((field->params.from != DMI_VERSION_NONE) and (version < field->params.from))
        return false;

    if ((field->params.before != DMI_VERSION_NONE) and (version >= field->params.before))
        return false;

    return true;
}

/**
 * @internal
 * @brief Whether a field carries a number, rather than a string reference or
 * bytes taken as they are.
 */
static inline bool dmi_field_is_numeric(const dmi_field_t *field)
{
    return (field->type != DMI_FIELD_TYPE_STRING) and (field->type != DMI_FIELD_TYPE_BINARY);
}

/**
 * @internal
 * @brief Whether the elements of an array are too short for the fields they
 * are known to hold, which makes them of no use: they are stepped over rather
 * than decoded, and kept as they are rather than encoded.
 */
static inline bool dmi_field_array_is_skipped(const dmi_field_t *field, size_t stride)
{
    return (field->params.stride_minimum != 0) and (stride < field->params.stride_minimum);
}

/**
 * @internal
 * @brief Largest value a member can hold, which is the one standing for
 * "unknown".
 */
static inline uintmax_t dmi_field_member_max(dmi_member_ref_t member)
{
    return (member.size >= sizeof(uintmax_t))
         ? UINTMAX_MAX
         : ((((uintmax_t)1) << (member.size * CHAR_BIT)) - 1);
}

/**
 * @internal
 * @brief Number the bytes spell, little-endian.
 */
static inline uintmax_t dmi_field_le_load(const dmi_byte_t *data, size_t length)
{
    uintmax_t number = 0;

    for (size_t i = 0; i < length; i++)
        number |= (uintmax_t)data[i] << (i * CHAR_BIT);

    return number;
}

/**
 * @internal
 * @brief Spell a number in bytes, little-endian.
 */
static inline void dmi_field_le_store(dmi_byte_t *data, size_t length, uintmax_t number)
{
    for (size_t i = 0; i < length; i++)
        data[i] = (dmi_byte_t)((number >> (i * CHAR_BIT)) & 0xFFu);
}

/**
 * @internal
 * @brief Collect the members the extended fields of a list are keyed on, one
 * entry per member however many extended fields it governs.
 */
void dmi_field_choices_collect(const dmi_field_t *fields, dmi_field_choices_t *choices);

/**
 * @internal
 * @brief Find the choice of the member at an offset, which is @c nullptr if
 * no extended field of the list is keyed on the member.
 */
dmi_field_choice_t *dmi_field_choice_find(dmi_field_choices_t *choices, size_t offset);

/**
 * @internal
 * @brief Check that the fields reach the offset a boundary declares.
 *
 * @details
 * Boundaries state where the parts of a structure begin, so that a declaration
 * which has lost or gained a field is caught where it happens. The check costs
 * one comparison and is made in every build, since a declaration which does
 * not match the data it describes reads or writes the rest of the structure as
 * nonsense.
 */
bool dmi_field_check_offset(const dmi_entity_t *entity, size_t position, const dmi_field_t *field);

/**
 * @internal
 * @brief Decode the data a field carries into its member, which is what the
 * decoder does with the data it reads, and what the encoding does with the
 * data it is about to write, to tell whether it decodes into the value the
 * structure holds.
 *
 * @details
 * Data standing for "unknown" becomes the largest value the member can hold,
 * and the rest is up to the handler of the field.
 */
bool dmi_field_apply(const dmi_field_t *field, const dmi_field_data_t *data, void *value);

/**
 * @internal
 * @brief Write a value into the member of the decoded structure, which may be
 * wider than the field it has been read from.
 */
bool dmi_field_store(const dmi_field_t *field, void *value, uintmax_t raw);

/**
 * @internal
 * @brief Write a value into a member of the decoded structure of the width the
 * member has, which is how the values the fields carry for one another, such
 * as the number of the elements an array declares, are stored.
 */
bool dmi_field_store_member(dmi_member_ref_t member, void *value, uintmax_t raw);

/**
 * @internal
 * @brief Read the value of a member of the decoded structure, of the width the
 * member has.
 */
uintmax_t dmi_field_load_member(dmi_member_ref_t member, const void *value);

/**
 * @internal
 * @brief Free the arrays of a list of fields, descending into the nested
 * structures and the elements of vectors and arrays, which may hold arrays of
 * their own.
 */
void dmi_field_release_list(const dmi_field_t *fields, dmi_data_t *info);

#endif // !OPENDMI_FIELD_INTERNAL_H
