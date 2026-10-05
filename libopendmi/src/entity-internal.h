//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTERNAL_H
#define OPENDMI_ENTITY_INTERNAL_H

#pragma once

/**
 * @file
 * @internal
 * @brief Parts of an entity the registry and the linker set up, see entity.c.
 */

#include <opendmi/entity.h>

__BEGIN_DECLS

/**
 * @internal
 * @brief Check whether an entity has anything to link.
 *
 * An entity is linked when its specification has a link handler, or when any
 * of its attributes declares the member a referenced structure goes into.
 * The rest have no references to resolve, and are left unlinked.
 *
 * @param[in] entity Entity descriptor.
 *
 * @return `true` if the entity has anything to link, `false` otherwise.
 */
bool dmi_entity_is_linkable(const dmi_entity_t *entity);

/**
 * @internal
 * @brief Attach additional information entry to the structure it refers to.
 *
 * The entry value is applied to a copy of the structure body when the
 * structure is decoded, so entries must be attached before decoding. Entries
 * are applied in the order they are attached.
 *
 * @details The function is exported, though it is not a part of the API,
 * since the tests of the command line tool, which link the shared library,
 * use it.
 *
 * @param[in] entity Referenced entity descriptor.
 * @param[in] source Decoded additional information structure (type 40).
 * @param[in] index  Zero-based index of the entry in @p source.
 *
 * @return `true` on success, `false` if the entry cannot be applied.
 *
 * @error DMI_ERROR_ARGUMENT_NULL Source is `nullptr`.
 * @error DMI_ERROR_ARGUMENT_INVALID Source is not a decoded additional
 *        information structure, or has no entry with such index.
 * @error DMI_ERROR_STATE_INVALID Structure is already decoded.
 * @error DMI_ERROR_OVERLAY_INVALID Entry refers to an additional information
 *        structure, to the structure header, or beyond the structure body.
 * @error DMI_ERROR_OUT_OF_MEMORY Memory is exhausted.
 */
__dmi_api bool dmi_entity_add_overlay(dmi_entity_t *entity, const dmi_entity_t *source, size_t index);

/**
 * @internal
 * @brief Attach decoded string property to its parent structure.
 *
 * @details The function is exported, though it is not a part of the API,
 * since the tests of the command line tool, which link the shared library,
 * use it.
 *
 * @param[in] entity   Parent entity descriptor.
 * @param[in] property Decoded string property structure (type 46).
 *
 * @return The function returns `true` on success and `false` otherwise.
 */
__dmi_api bool dmi_entity_add_property(dmi_entity_t *entity, const dmi_string_property_t *property);

__END_DECLS

#endif // !OPENDMI_ENTITY_INTERNAL_H
