//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_FILTER_H
#define OPENDMI_FILTER_H

#pragma once

#include <opendmi/types.h>
#include <opendmi/module.h>
#include <opendmi/utils/vector.h>

typedef struct dmi_filter dmi_filter_t;

typedef enum dmi_filter_mask
{
    DMI_FILTER_MASK_COMMON   = 1 << 0,
    DMI_FILTER_MASK_OEM      = 1 << 1,
    DMI_FILTER_MASK_INACTIVE = 1 << 2,
    DMI_FILTER_MASK_UNKNOWN  = 1 << 3,
    DMI_FILTER_MASK_DEFAULT  = DMI_FILTER_MASK_COMMON | DMI_FILTER_MASK_OEM,
    DMI_FILTER_MASK_ALL      = DMI_FILTER_MASK_COMMON | DMI_FILTER_MASK_OEM |
                               DMI_FILTER_MASK_INACTIVE | DMI_FILTER_MASK_UNKNOWN
} dmi_filter_mask_t;

struct dmi_filter
{
    dmi_context_t *context;

    unsigned mask;
    dmi_vector_t handles;
    dmi_vector_t types;
    dmi_vector_t modules;
};

__BEGIN_DECLS

/**
 * @brief Create a filter.
 *
 * Allocates and initialises a new filter bound to @p context. The category
 * mask is set to `DMI_FILTER_MASK_ALL` and the handle, type and module lists
 * are empty, so the filter matches all entities until constraints are added.
 *
 * @param[in] context Context handle.
 *
 * @return Pointer to the new filter, or @c nullptr on allocation failure.
 */
__dmi_api dmi_filter_t *dmi_filter_create(dmi_context_t *context);

/**
 * @brief Add a handle constraint to the filter.
 *
 * Registers @p handle as one of the allowed entity handles. Once at least one
 * handle has been added, only entities whose handle appears in this list (and
 * that pass the category mask) are matched.
 *
 * @param[in,out] filter Filter to update.
 * @param[in]     handle Entity handle to allow.
 *
 * @return `true` on success, `false` if @p filter is @c nullptr or the handle
 *         could not be stored.
 */
__dmi_api bool dmi_filter_add_handle(dmi_filter_t *filter, dmi_handle_t handle);

/**
 * @brief Add a type constraint to the filter.
 *
 * Registers @p type as one of the allowed structure types. Once at least one
 * type has been added, only entities whose type appears in this list (and that
 * pass the category mask) are matched.
 *
 * @param[in,out] filter Filter to update.
 * @param[in]     type   Structure type to allow.
 *
 * @return `true` on success, `false` if @p filter is @c nullptr or the type could
 *         not be stored.
 */
__dmi_api bool dmi_filter_add_type(dmi_filter_t *filter, dmi_type_id_t type);

/**
 * @brief Add a module constraint to the filter.
 *
 * Registers @p module as one of the allowed modules. Once at least one module
 * has been added, entities decoded by the specifications of the module are
 * matched, whatever type numbers the platform places them at. Entities of
 * a module which is not enabled are not decoded by its specifications, and
 * are not matched.
 *
 * @param[in,out] filter Filter to update.
 * @param[in]     module Module to allow.
 *
 * @return `true` on success, `false` if @p filter or @p module is @c nullptr,
 *         or the module could not be stored.
 */
__dmi_api bool dmi_filter_add_module(dmi_filter_t *filter, const dmi_module_t *module);

/**
 * @brief Check whether the filter has no handle, type or module constraints.
 *
 * An empty filter matches every entity that satisfies the category mask,
 * without restricting by specific handles, types or modules.
 *
 * @param[in] filter Filter to check.
 *
 * @return `true` if the handle, type and module lists are empty, or if
 *         @p filter is @c nullptr; `false` otherwise.
 */
__dmi_api bool dmi_filter_is_empty(const dmi_filter_t *filter);

/**
 * @brief Test whether an entity satisfies the filter.
 *
 * The match is evaluated in two stages:
 *
 * 1. **Category mask** — the entity's category (common vs. OEM), inactivity,
 *    and unknown-spec state are checked against `filter->mask`. The entity is
 *    rejected if any applicable bit is absent from the mask.
 *
 * 2. **Handle/type/module lists** — if the filter is non-empty, the entity
 *    must match at least one entry in the handle list, the type list or the
 *    module list. If the filter is empty, every entity that passed the mask
 *    check is accepted.
 *
 * @param[in] filter Filter descriptor.
 * @param[in] entity Entity to test.
 *
 * @return `true` if @p entity satisfies all filter constraints, `false`
 *         otherwise or if either argument is @c nullptr.
 */
__dmi_api bool dmi_filter_match(const dmi_filter_t *filter, const dmi_entity_t *entity);

/**
 * @brief Destroy a filter.
 *
 * Releases all resources associated with @p filter, including the handle,
 * type and module constraint lists. Does nothing if @p filter is @c nullptr.
 *
 * @param[in] filter Filter to destroy.
 */
__dmi_api void dmi_filter_destroy(dmi_filter_t *filter);

__END_DECLS

#endif // !OPENDMI_FILTER_H
