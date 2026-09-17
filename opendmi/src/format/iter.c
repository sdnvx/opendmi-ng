//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <limits.h>
#include <assert.h>

#include <opendmi/internal.h>
#include <opendmi/utils/name.h>
#include <opendmi/format/iter.h>

void dmi_format_array_iter_init(
        dmi_format_array_iter_t *iter,
        const dmi_attribute_t   *attr,
        const dmi_data_t        *info,
        const void              *value)
{
    assert(iter != nullptr);
    assert(attr != nullptr);
    assert(info != nullptr);
    assert(value != nullptr);

    iter->attr  = attr;
    iter->next  = dmi_deref(dmi_data_t *, value);
    iter->count = dmi_attribute_get_count(attr, info);
    iter->index = SIZE_MAX;

    // Array may be not allocated if it is empty
    if (iter->next == nullptr)
        iter->count = 0;
}

const dmi_data_t *dmi_format_array_iter_next(dmi_format_array_iter_t *iter)
{
    assert(iter != nullptr);

    // Index is incremented from SIZE_MAX to zero on the first call
    size_t index = iter->index + 1;
    if (index >= iter->count)
        return nullptr;

    const dmi_data_t *element = iter->next;

    iter->index  = index;
    iter->next  += iter->attr->value.size;

    return element;
}

void dmi_format_set_iter_init(
        dmi_format_set_iter_t *iter,
        const dmi_attribute_t *attr,
        const void            *value)
{
    assert(iter != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    dmi_format_mask_iter_init(iter, attr->params.values,
                              dmi_attribute_get_uint(attr, value),
                              attr->value.size * CHAR_BIT);
}

void dmi_format_mask_iter_init(
        dmi_format_set_iter_t *iter,
        const dmi_name_set_t  *values,
        uintmax_t              mask,
        size_t                 width)
{
    assert(iter != nullptr);

    // Bits beyond the mask type are never set
    if (width > sizeof(mask) * CHAR_BIT)
        width = sizeof(mask) * CHAR_BIT;

    iter->values = values;
    iter->width  = width;
    iter->mask   = mask;
    iter->next   = 0;
    iter->flag   = (dmi_format_flag_t){};
}

const dmi_format_flag_t *dmi_format_set_iter_next(dmi_format_set_iter_t *iter)
{
    assert(iter != nullptr);

    while (iter->next < iter->width) {
        size_t id = iter->next++;

        const char *code = dmi_code_lookup(iter->values, (int)id);
        if (code == nullptr)
            continue;

        iter->flag = (dmi_format_flag_t){
            .id    = id,
            .code  = code,
            .name  = dmi_name_lookup(iter->values, (int)id),
            .value = (iter->mask & ((uintmax_t)1 << id)) != 0
        };

        return &iter->flag;
    }

    return nullptr;
}

void dmi_format_string_iter_init(dmi_format_string_iter_t *iter, const dmi_entity_t *entity)
{
    assert(iter != nullptr);
    assert(entity != nullptr);

    iter->entity = entity;
    iter->index  = 0;
}

const char *dmi_format_string_iter_next(dmi_format_string_iter_t *iter)
{
    assert(iter != nullptr);

    if (iter->index >= iter->entity->string_count)
        return nullptr;

    // Strings within the string count are always present
    return dmi_entity_string_ex(iter->entity, ++iter->index, true);
}
