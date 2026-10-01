//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdint.h>
#include <assert.h>

#include <opendmi/attribute.h>
#include <opendmi/internal.h>

/**
 * @internal
 * @brief Walk the elements of an array or of a vector.
 *
 * @param[in] node    Place of the array in the walk.
 * @param[in] visitor Callbacks of the walk.
 * @param[in] context Context passed to the callbacks.
 *
 * @return `false` if a callback has stopped the walk, `true` otherwise.
 */
static bool dmi_attribute_walk_array(
        const dmi_attribute_node_t    *node,
        const dmi_attribute_visitor_t *visitor,
        void                          *context);

/**
 * @internal
 * @brief Walk the members of a nested structure, which is a member itself or
 * an element of an array.
 *
 * @param[in] node    Place of the structure in the walk.
 * @param[in] visitor Callbacks of the walk.
 * @param[in] context Context passed to the callbacks.
 *
 * @return `false` if a callback has stopped the walk, `true` otherwise.
 */
static bool dmi_attribute_walk_struct(
        const dmi_attribute_node_t    *node,
        const dmi_attribute_visitor_t *visitor,
        void                          *context);

/**
 * @internal
 * @brief Walk the value of a member or of an element, which is a structure
 * or a plain value.
 *
 * @param[in] node    Place of the value in the walk.
 * @param[in] visitor Callbacks of the walk.
 * @param[in] context Context passed to the callbacks.
 *
 * @return `false` if a callback has stopped the walk, `true` otherwise.
 */
static bool dmi_attribute_walk_value(
        const dmi_attribute_node_t    *node,
        const dmi_attribute_visitor_t *visitor,
        void                          *context);

/**
 * @internal
 * @brief Call an entering callback, which goes on into the value if there is
 * none.
 *
 * @param[in] enter   Entering callback, or `nullptr`.
 * @param[in] context Context passed to the callback.
 * @param[in] node    Place the walk has reached.
 *
 * @return What the walk does next.
 */
static inline dmi_attribute_walk_t dmi_attribute_enter(
        dmi_attribute_enter_fn     *enter,
        void                       *context,
        const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Call a leaving callback, which goes on if there is none.
 *
 * @param[in] leave   Leaving callback, or `nullptr`.
 * @param[in] context Context passed to the callback.
 * @param[in] node    Place the walk is leaving.
 *
 * @return `false` if the callback has stopped the walk, `true` otherwise.
 */
static inline bool dmi_attribute_leave(
        dmi_attribute_leave_fn     *leave,
        void                       *context,
        const dmi_attribute_node_t *node);

bool dmi_attributes_walk(
        const dmi_attribute_t         *attrs,
        dmi_data_t                    *info,
        const dmi_attribute_visitor_t *visitor,
        void                          *context)
{
    assert(visitor != nullptr);

    if ((attrs == nullptr) or (info == nullptr))
        return true;

    for (const dmi_attribute_t *attr = attrs; attr->type != DMI_ATTRIBUTE_TYPE_NONE; attr++) {
        if (not dmi_attribute_walk(attr, info, visitor, context))
            return false;
    }

    return true;
}

bool dmi_attribute_walk(
        const dmi_attribute_t         *attr,
        dmi_data_t                    *info,
        const dmi_attribute_visitor_t *visitor,
        void                          *context)
{
    assert(attr != nullptr);
    assert(info != nullptr);
    assert(visitor != nullptr);

    // Value of a variant attribute is described by the variant, and there is
    // no value if no variant matches
    const dmi_attribute_t *resolved = dmi_attribute_resolve(attr, info);
    if (resolved == nullptr)
        return true;

    const dmi_attribute_node_t node = {
        .member = attr,
        .attr   = resolved,
        .info   = info,
        .value  = info + resolved->value.offset,
        .index  = SIZE_MAX
    };

    switch (dmi_attribute_enter(visitor->member_start, context, &node)) {
    case DMI_ATTRIBUTE_WALK_STOP:
        return false;
    case DMI_ATTRIBUTE_WALK_SKIP:
        return true;
    default:
        break;
    }

    bool success = dmi_attribute_is_array(resolved)
                 ? dmi_attribute_walk_array(&node, visitor, context)
                 : dmi_attribute_walk_value(&node, visitor, context);

    return success and dmi_attribute_leave(visitor->member_end, context, &node);
}

static bool dmi_attribute_walk_array(
        const dmi_attribute_node_t    *node,
        const dmi_attribute_visitor_t *visitor,
        void                          *context)
{
    const dmi_attribute_t *attr = node->attr;

    // Elements of an array are kept apart from the structure, while the ones
    // of a vector are held in place, and an array holding none may not have
    // been allocated at all. Counters are members of the structure holding
    // the array
    dmi_data_t *elements = (dmi_data_t *)dmi_attribute_get_elements(attr, node->value);

    dmi_attribute_node_t array = *node;
    array.count = (elements != nullptr) ? dmi_attribute_get_count(attr, node->info) : 0;

    switch (dmi_attribute_enter(visitor->array_start, context, &array)) {
    case DMI_ATTRIBUTE_WALK_STOP:
        return false;
    case DMI_ATTRIBUTE_WALK_SKIP:
        return true;
    default:
        break;
    }

    for (size_t i = 0; i < array.count; i++) {
        dmi_attribute_node_t item = array;

        item.value = elements + (i * attr->value.size);
        item.index = i;

        dmi_attribute_walk_t walk = dmi_attribute_enter(visitor->item_start, context, &item);

        if (walk == DMI_ATTRIBUTE_WALK_STOP)
            return false;
        if (walk == DMI_ATTRIBUTE_WALK_SKIP)
            continue;

        if (not dmi_attribute_walk_value(&item, visitor, context))
            return false;

        if (not dmi_attribute_leave(visitor->item_end, context, &item))
            return false;
    }

    return dmi_attribute_leave(visitor->array_end, context, &array);
}

static bool dmi_attribute_walk_struct(
        const dmi_attribute_node_t    *node,
        const dmi_attribute_visitor_t *visitor,
        void                          *context)
{
    switch (dmi_attribute_enter(visitor->struct_start, context, node)) {
    case DMI_ATTRIBUTE_WALK_STOP:
        return false;
    case DMI_ATTRIBUTE_WALK_SKIP:
        return true;
    default:
        break;
    }

    if (not dmi_attributes_walk(node->attr->params.attrs, node->value, visitor, context))
        return false;

    return dmi_attribute_leave(visitor->struct_end, context, node);
}

static bool dmi_attribute_walk_value(
        const dmi_attribute_node_t    *node,
        const dmi_attribute_visitor_t *visitor,
        void                          *context)
{
    if (node->attr->type == DMI_ATTRIBUTE_TYPE_STRUCT)
        return dmi_attribute_walk_struct(node, visitor, context);

    return (visitor->value == nullptr) or visitor->value(context, node);
}

static inline dmi_attribute_walk_t dmi_attribute_enter(
        dmi_attribute_enter_fn     *enter,
        void                       *context,
        const dmi_attribute_node_t *node)
{
    return (enter != nullptr) ? enter(context, node) : DMI_ATTRIBUTE_WALK_CONTINUE;
}

static inline bool dmi_attribute_leave(
        dmi_attribute_leave_fn     *leave,
        void                       *context,
        const dmi_attribute_node_t *node)
{
    return (leave == nullptr) or leave(context, node);
}
