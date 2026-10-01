//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_CONTEXT_INTERNAL_H
#define OPENDMI_CONTEXT_INTERNAL_H

#pragma once

/**
 * @file
 * @internal
 * @brief Definitions the parts of the context share, see context.c,
 * context-types.c, context-extension.c and context-save.c.
 */

#include <opendmi/context.h>
#include <opendmi/module.h>

__BEGIN_DECLS

/**
 * @internal
 * @brief Setup vendor-specific extensions.
 *
 * @details
 * Tells the vendor and the platform from the firmware and system information,
 * enables the modules of the platform, if requested, and maps the
 * specifications of the enabled modules for the platform.
 */
bool dmi_setup_extensions(dmi_context_t *context);

/**
 * @internal
 * @brief Tell the vendor from the firmware information.
 *
 * @return `false` if the vendor is not told in strict mode, `true` otherwise.
 */
bool dmi_setup_vendor(dmi_context_t *context);

/**
 * @internal
 * @brief Get the platform the structures are decoded for: the one set
 * explicitly, or the one told from the data.
 */
const dmi_platform_t *dmi_context_platform(const dmi_context_t *context);

/**
 * @internal
 * @brief Allocate a map of `DMI_TYPE_ID_MAX + 1` candidate lists.
 *
 * @details
 * Maps are some kilobytes large, too large for the stack of the Linux kernel,
 * so the ones built to be checked before they replace the map of the context
 * are allocated too.
 *
 * @return Map to be freed with `dmi_free()`, or @c nullptr if memory is
 *         exhausted.
 */
dmi_type_candidates_t *dmi_types_create(dmi_context_t *context);

/**
 * @internal
 * @brief Map specifications to types for the platform of the context.
 *
 * @details
 * Standard specifications are mapped first, followed by the specifications of
 * the enabled modules and of @p extra, which apply to the platform, and by
 * the ones of the modules which yield their types to the rest, into the types
 * left free. The map of the context is not changed, so that it stays as it was
 * on conflicts.
 *
 * @param[in]  context Context descriptor.
 * @param[in]  extra   Module which is being enabled, or @c nullptr.
 * @param[in]  report  Whether a conflict is raised as an error.
 * @param[out] map     Map of `DMI_TYPE_ID_MAX + 1` candidate lists to fill.
 *
 * @return `true` on success, `false` if two specifications are mapped to the
 *         same type.
 */
bool dmi_types_map(
        dmi_context_t         *context,
        const dmi_module_t    *extra,
        bool                   report,
        dmi_type_candidates_t *map);

/**
 * @internal
 * @brief Map specifications to types anew for the platform of the context,
 * along with @p extra, the module which is being enabled.
 *
 * @details
 * The map is built apart and replaces the map of the context only if there is
 * no conflict, so that the context is left as it is otherwise.
 *
 * @param[in] context Context descriptor.
 * @param[in] extra   Module which is being enabled, or @c nullptr.
 *
 * @return `true` on success, `false` if memory is exhausted or two
 *         specifications are mapped to the same type, which is raised as an
 *         error.
 */
bool dmi_types_rebuild(dmi_context_t *context, const dmi_module_t *extra);

__END_DECLS

#endif // !OPENDMI_CONTEXT_INTERNAL_H
