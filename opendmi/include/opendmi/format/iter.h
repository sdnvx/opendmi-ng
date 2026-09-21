//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_FORMAT_ITER_H
#define OPENDMI_FORMAT_ITER_H

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <opendmi/types.h>
#include <opendmi/attribute.h>
#include <opendmi/entity.h>
#include <opendmi/entity/string-property.h>
#include <opendmi/entity/additional-info.h>

typedef struct dmi_format_array_iter    dmi_format_array_iter_t;
typedef struct dmi_format_flag          dmi_format_flag_t;
typedef struct dmi_format_set_iter      dmi_format_set_iter_t;
typedef struct dmi_format_string_iter   dmi_format_string_iter_t;
typedef struct dmi_format_property_iter dmi_format_property_iter_t;

/**
 * @brief Iterator over elements of an array attribute.
 */
struct dmi_format_array_iter
{
    /**
     * @brief Array attribute descriptor.
     */
    const dmi_attribute_t *attr;

    /**
     * @brief Pointer to the next element.
     */
    const dmi_data_t *next;

    /**
     * @brief Number of elements.
     */
    size_t count;

    /**
     * @brief Zero-based index of the element returned last.
     */
    size_t index;
};

/**
 * @brief Flag of a set attribute.
 */
struct dmi_format_flag
{
    /**
     * @brief Bit number.
     */
    size_t id;

    /**
     * @brief Code name, used to identify the flag during serialization.
     */
    const char *code;

    /**
     * @brief Printable name.
     */
    const char *name;

    /**
     * @brief Flag value.
     */
    bool value;
};

/**
 * @brief Iterator over named flags of a set attribute.
 */
struct dmi_format_set_iter
{
    /**
     * @brief Flag names.
     */
    const dmi_name_set_t *values;

    /**
     * @brief Set width in bits.
     */
    size_t width;

    /**
     * @brief Set value.
     */
    uintmax_t mask;

    /**
     * @brief Next bit number to check.
     */
    size_t next;

    /**
     * @brief Flag returned last.
     */
    dmi_format_flag_t flag;
};

/**
 * @brief Iterator over strings of an entity.
 */
struct dmi_format_string_iter
{
    /**
     * @brief Entity descriptor.
     */
    const dmi_entity_t *entity;

    /**
     * @brief One-based index of the string returned last.
     */
    size_t index;
};

struct dmi_format_property_iter
{
    /**
     * @brief Entity descriptor.
     */
    const dmi_entity_t *entity;

    /**
     * @brief Index of the next property.
     */
    size_t next;
};

__BEGIN_DECLS

/**
 * @brief Initialize iterator over elements of an array attribute.
 *
 * @param[out] iter  Iterator.
 * @param[in]  attr  Array attribute descriptor.
 * @param[in]  info  Decoded entity data, containing the array counter.
 * @param[in]  value Pointer to the array attribute value.
 */
void dmi_format_array_iter_init(
        dmi_format_array_iter_t *iter,
        const dmi_attribute_t   *attr,
        const dmi_data_t        *info,
        const void              *value);

/**
 * @brief Get the next element of an array attribute.
 *
 * For structure arrays, the element is a pointer to the structure, otherwise
 * it is a pointer to the value. Index of the element is available as
 * `iter->index`.
 *
 * @param[in,out] iter Iterator.
 *
 * @return Pointer to the element, or @c nullptr if there are no more elements.
 */
const dmi_data_t *dmi_format_array_iter_next(dmi_format_array_iter_t *iter);

/**
 * @brief Initialize iterator over named flags of a set attribute.
 *
 * @param[out] iter  Iterator.
 * @param[in]  attr  Set attribute descriptor.
 * @param[in]  value Pointer to the set attribute value.
 */
void dmi_format_set_iter_init(
        dmi_format_set_iter_t *iter,
        const dmi_attribute_t *attr,
        const void            *value);

/**
 * @brief Initialize iterator over named flags of a bit mask.
 *
 * @param[out] iter   Iterator.
 * @param[in]  values Flag names, identified by bit numbers.
 * @param[in]  mask   Bit mask.
 * @param[in]  width  Bit mask width in bits.
 */
void dmi_format_mask_iter_init(
        dmi_format_set_iter_t *iter,
        const dmi_name_set_t  *values,
        uintmax_t              mask,
        size_t                 width);

/**
 * @brief Get the next named flag of a set attribute.
 *
 * Bits without names are skipped.
 *
 * @param[in,out] iter Iterator.
 *
 * @return Pointer to the flag, which is valid until the next call, or
 *         @c nullptr if there are no more flags.
 */
const dmi_format_flag_t *dmi_format_set_iter_next(dmi_format_set_iter_t *iter);

/**
 * @brief Initialize iterator over strings of an entity.
 *
 * @param[out] iter   Iterator.
 * @param[in]  entity Entity descriptor.
 */
void dmi_format_string_iter_init(dmi_format_string_iter_t *iter, const dmi_entity_t *entity);

/**
 * @brief Get the next raw string of an entity.
 *
 * Index of the string is available as `iter->index`.
 *
 * @param[in,out] iter Iterator.
 *
 * @return String, or @c nullptr if there are no more strings.
 */
const char *dmi_format_string_iter_next(dmi_format_string_iter_t *iter);

void dmi_format_property_iter_init(dmi_format_property_iter_t *iter, const dmi_entity_t *entity);

const dmi_string_property_t *dmi_format_property_iter_next(dmi_format_property_iter_t *iter);

/**
 * @brief Format value of additional information entry applied to an entity.
 *
 * @param[in] entity  Entity, which the entry is applied to.
 * @param[in] overlay Applied entry.
 * @param[in] pretty  Format for humans: bytes are uppercase and separated by
 *                    spaces.
 *
 * @return Allocated string, which is to be freed with `dmi_free()`, or
 *         @c nullptr on failure.
 */
char *dmi_format_overlay_value(const dmi_entity_t *entity, const dmi_entity_overlay_t *overlay, bool pretty);

/**
 * @brief Format value of an attribute for the output being written.
 *
 * Values are written by their code names, which do not change with the
 * locale, unless @p pretty asks for the text a person reads, which is what
 * the text format prints.
 *
 * The measurement unit of the attribute is part of the value only when it is
 * formatted for a person, since a machine-readable value carries the number
 * alone, and the formats which keep the unit have a place of their own for
 * it.
 *
 * @param[in] context DMI context.
 * @param[in] attr    Attribute descriptor.
 * @param[in] value   Pointer to the value.
 * @param[in] pretty  Format for humans.
 *
 * @return Allocated string, which is to be freed with `dmi_free()`, or
 *         @c nullptr on failure.
 */
char *dmi_format_attribute_value(
        dmi_context_t         *context,
        const dmi_attribute_t *attr,
        const void            *value,
        bool                   pretty);

__END_DECLS

#endif // !OPENDMI_FORMAT_ITER_H
