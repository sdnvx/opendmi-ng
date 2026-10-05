//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_REGISTRY_INTERNAL_H
#define OPENDMI_REGISTRY_INTERNAL_H

#pragma once

#include <opendmi/registry.h>

__BEGIN_DECLS

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
 * @brief Attach additional information entries to the structures they refer
 * to.
 *
 * Decodes all additional information structures (type 40) in @p registry, and
 * attaches their entries to the referenced structures, so that entry values
 * are applied when the structures are decoded. Must be called after scanning
 * and before decoding other structures. Called on opening a context if
 * `DMI_CONTEXT_FLAG_OVERLAY` is set.
 *
 * Processing is not stopped by invalid entries, so that all of them are
 * reported to the error queue. Invalid entries are not applied, and whether
 * they are fatal is decided at the end, as in `dmi_registry_link()`.
 *
 * @param[in,out] registry Registry handle.
 *
 * @return `true` on success, `false` if any entry is invalid in strict mode,
 *         or if memory is exhausted.
 *
 * @error DMI_ERROR_ENTITY_NOT_FOUND Referenced structure is not found.
 * @error DMI_ERROR_OVERLAY_INVALID Entry refers to the structure header or
 *        beyond the structure body.
 * @error DMI_ERROR_ENTITY_DECODE_FAILED Additional information structure is
 *        malformed.
 * @error DMI_ERROR_OUT_OF_MEMORY Memory is exhausted.
 */
bool dmi_registry_overlay(dmi_registry_t *registry);

/**
 * @internal
 * @brief Decode all entities in the registry.
 *
 * Iterates over all entities registered in @p registry and invokes the
 * type-specific decode handler for each one, populating their decoded
 * field data. Sets `DMI_REGISTRY_STATUS_DECODED` on success.
 *
 * Structure boundaries are checked by `dmi_registry_scan()`, so a structure
 * that fails to decode does not affect the others. Decoding is not stopped by
 * such failure: all entities are processed, so that every malformed structure
 * in the table is reported to the error queue, and entities that fail to
 * decode are left undecoded (without decoded field data). Whether it is fatal
 * is decided at the end: unless `DMI_CONTEXT_FLAG_STRICT` is set, the failures
 * are logged as warnings, and the registry is still marked as decoded.
 *
 * Decoding is stopped immediately if memory is exhausted, regardless of the
 * mode.
 *
 * @param[in,out] registry Registry handle.
 *
 * @return `true` on success, `false` if any entity fails to decode in strict
 *         mode, or if memory is exhausted.
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
 * Linking is not stopped by a failure: all entities are processed, so that
 * every broken reference in the table is reported to the error queue. Link
 * handlers return `false` on any failure, and whether it is fatal is decided
 * here: unless `DMI_CONTEXT_FLAG_STRICT` is set, the registry is still marked
 * as linked and the errors are left in the error queue.
 *
 * @param[in,out] registry Registry handle.
 *
 * @return `true` on success, `false` if any entity fails to link in strict
 *         mode.
 */
bool dmi_registry_link(dmi_registry_t *registry);

__END_DECLS

#endif // !OPENDMI_REGISTRY_INTERNAL_H
