//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_REGISTRY_H
#define OPENDMI_REGISTRY_H

#pragma once

#include <opendmi/filter.h>

/**
 * @brief Default DMI registry capacity.
 */
#define DMI_REGISTRY_CAPACITY 64

typedef struct dmi_registry       dmi_registry_t;
typedef struct dmi_registry_entry dmi_registry_entry_t;
typedef struct dmi_registry_iter  dmi_registry_iter_t;

/**
 * @brief Registry status flags.
 */
enum dmi_context_status
{
    /** SMBIOS table has been scanned and all entities are registered. */
    DMI_REGISTRY_STATUS_SCANNED   = (1 << 0),

    /** Table area or at least one structure was truncated during scanning. */
    DMI_REGISTRY_STATUS_TRUNCATED = (1 << 1),

    /** All registered entities have been decoded. */
    DMI_REGISTRY_STATUS_DECODED   = (1 << 2),

    /** Cross-references between entities have been resolved. */
    DMI_REGISTRY_STATUS_LINKED    = (1 << 3)
};

/**
 * @brief Registry descriptor.
 */
struct dmi_registry
{
    /**
     * @brief DMI context handle.
     */
    dmi_context_t *context;

    /**
     * @brief Structure index capacity.
     */
    size_t capacity;

    /**
     * @brief Structure count.
     */
    size_t count;

    /**
     * @brief Structure index.
     */
    dmi_registry_entry_t **index;

    /**
     * @brief Pointer to first entry in sequence.
     */
    dmi_registry_entry_t *head;

    /**
     * @brief Pointer to last entry in sequence.
     */
    dmi_registry_entry_t *tail;

    /**
     * @brief Status flags.
     */
    unsigned int status;
};

/**
 * @brief Registry entry descriptor.
 */
struct dmi_registry_entry
{
    /**
     * @brief Entity descriptor.
     */
    dmi_entity_t *entity;

    /**
     * @brief Pointer to next entry in the hash table cell.
     */
    dmi_registry_entry_t *next;

    /**
     * @brief Pointer to previous entry in entity sequence.
     */
    dmi_registry_entry_t *seq_prev;

    /**
     * @brief Pointer to next entry in entity sequence.
     */
    dmi_registry_entry_t *seq_next;
};

/**
 * @brief Registry iterator.
 */
struct dmi_registry_iter
{
    /**
     * @brief Registry handle.
     */
    dmi_registry_t *registry;

    /**
     * @brief Filter descriptor.
     */
    dmi_filter_t *filter;

    /**
     * @brief Current position.
     */
    dmi_registry_entry_t *position;

    /**
     * @brief Done flag.
     */
    bool is_done;
};

__BEGIN_DECLS

/**
 * @brief Create registry.
 *
 * @param[in] context  Context handle.
 * @param[in] capacity Registry hash-table capacity. Zero means default value
 *                     of `DMI_REGISTRY_CAPACITY`.
 */
dmi_registry_t *dmi_registry_create(dmi_context_t *context, size_t capacity);

/**
 * @internal
 * @brief Scan the SMBIOS table and populate the registry with entity descriptors.
 *
 * Iterates over the raw SMBIOS table data held by the registry's context,
 * creates an entity descriptor for each structure found, and inserts it into
 * the registry.
 *
 * Scanning stops at the end-of-table marker or when the table area is exhausted.
 * If the table area or a structure is truncated, `DMI_REGISTRY_STATUS_TRUNCATED`
 * is set in the registry status but the function still succeeds. Unless
 * `DMI_CONTEXT_FLAG_STRICT` is set, a structure with length shorter than its
 * header is handled the same way, since the following structures cannot be
 * located. Sets `DMI_REGISTRY_STATUS_SCANNED` on success.
 *
 * @param[in,out] registry Registry handle.
 *
 * @return `true` on success, `false` if any entity could not be created or
 *         registered.
 */
bool dmi_registry_scan(dmi_registry_t *registry);

/**
 * @internal
 * @brief Decode all entities in the registry.
 *
 * Iterates over all entities registered in @p registry and invokes the
 * type-specific decode handler for each one, populating their decoded
 * field data. Sets `DMI_REGISTRY_STATUS_DECODED` on success.
 *
 * Unless `DMI_CONTEXT_FLAG_STRICT` is set, entities that fail to decode are
 * left undecoded (without decoded field data) and skipped with a warning.
 *
 * @param[in,out] registry Registry handle.
 *
 * @return `true` on success, `false` if any entity fails to decode in strict
 *         mode.
 */
bool dmi_registry_decode(dmi_registry_t *registry);

/**
 * @internal
 * @brief Resolve cross-references between entities in the registry.
 *
 * Iterates over all entities registered in @p registry and invokes the
 * type-specific link handler for each entity that has one. Link handlers
 * resolve SMBIOS handle references to the corresponding entity pointers,
 * establishing relationships between structures. Undecoded entities are
 * skipped. Sets `DMI_REGISTRY_STATUS_LINKED` on success.
 *
 * @param[in,out] registry Registry handle.
 *
 * @return `true` on success, `false` if any entity fails to link.
 */
bool dmi_registry_link(dmi_registry_t *registry);

/**
 * @brief Get entity from registry by handle.
 *
 * @param[in] registry Registry handle.
 *
 * @param[in] handle   Entity handle. Reserved values `DMI_HANDLE_INVALID` and
 *                     `DMI_HANDLE_UNSUPPORTED` mean that the reference is not
 *                     set: `nullptr` is returned and no error is raised. Use
 *                     `dmi_registry_get_first()` to look up entity by type.
 *
 * @param[in] type     Expected structure type. Should be set to
 *                     `DMI_TYPE_INVALID` if the type is unknown.
 *
 * @param[in] optional Set to true if missing entity is not an error.
 *
 * @returns Non-owning pointer to the entity, `nullptr` if not found.
 */
dmi_entity_t *dmi_registry_get(
        dmi_registry_t *registry,
        dmi_handle_t    handle,
        dmi_type_t      type,
        bool            optional);

/**
 * @brief Get entity from registry, that matches any of the expected types.
 *
 * @param[in] registry Registry handle.
 *
 * @param[in] handle   Entity handle. Reserved values `DMI_HANDLE_INVALID` and
 *                     `DMI_HANDLE_UNSUPPORTED` mean that the reference is not
 *                     set: `nullptr` is returned and no error is raised.
 *
 * @param[in] types    Array of expected structure types, terminated by
 *                     `DMI_TYPE_INVALID`. May be set to `nullptr` to disable
 *                     type checks.
 *
 * @param[in] optional Set to true if missing entity is not an error.
 *
 * @returns Non-owning pointer to the entity, `nullptr` if not found.
 */
dmi_entity_t *dmi_registry_get_any(
        dmi_registry_t   *registry,
        dmi_handle_t      handle,
        const dmi_type_t *type,
        bool              optional);

/**
 * @brief Get the first entity of the given type from registry.
 *
 * @param[in] registry Registry handle.
 *
 * @param[in] type     Structure type, must not be `DMI_TYPE_INVALID`.
 *
 * @param[in] optional Set to true if missing entity is not an error.
 *
 * @returns Non-owning pointer to the first entity of the given type in table
 *          order, `nullptr` if not found.
 */
dmi_entity_t *dmi_registry_get_first(
        dmi_registry_t *registry,
        dmi_type_t      type,
        bool            optional);

/**
 * @brief Get registry status flags.
 *
 * @param[in] registry Registry handle.
 * @return Status flags
 */
unsigned dmi_registry_status(const dmi_registry_t *registry);

/**
 * @brief Destroy registry.
 *
 * @param[in] registry Registry handle.
 */
void dmi_registry_destroy(dmi_registry_t *registry);

/**
 * @brief Initialize a registry iterator.
 *
 * Prepares @p iter to traverse entities in @p registry in the order they were
 * scanned. If @p filter is non-`NULL`, only entities that match the filter are
 * visited; pass `NULL` to iterate over all entities.
 *
 * @param[out] iter     Iterator to initialize.
 * @param[in]  registry Registry to iterate over.
 * @param[in]  filter   Optional filter; `NULL` to disable filtering.
 *
 * @return `true` on success, `false` if @p iter or @p registry is `NULL`.
 */
bool dmi_registry_iter_init(
        dmi_registry_iter_t *iter,
        dmi_registry_t *registry,
        dmi_filter_t *filter);

/**
 * @brief Check whether the iterator has more entities to yield.
 *
 * Peeks ahead from the current position to determine whether a subsequent call
 * to `dmi_registry_iter_next`(3) would return a non-`NULL` entity. The
 * iterator position is not modified.
 *
 * @param[in] iter Iterator to query.
 *
 * @return `true` if at least one more entity is available, `false` otherwise or
 *         if @p iter is `NULL`.
 */
bool dmi_registry_iter_has_next(dmi_registry_iter_t *iter);

/**
 * @brief Advance the iterator and return the next entity.
 *
 * Moves the iterator to the next entity in the registry that satisfies the
 * filter (if any) and returns a non-owning pointer to it.
 *
 * @param[in,out] iter Iterator to advance.
 *
 * @return Non-owning pointer to the next entity, or `NULL` if there are no
 *         more entities or @p iter is `NULL`.
 */
dmi_entity_t *dmi_registry_iter_next(dmi_registry_iter_t *iter);

__END_DECLS

#endif // !OPENDMI_REGISTRY_H
