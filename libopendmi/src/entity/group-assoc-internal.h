//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_GROUP_ASSOC_INTERNAL_H
#define OPENDMI_ENTITY_GROUP_ASSOC_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/group-assoc.h>

bool dmi_group_assoc_link(dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the members of a group of a well-known name are of the
 * type the group lists.
 *
 * @details Members of a group of a well-known name are decoded by the
 * specification the group lists, and the ones which are not are placed by the
 * vendor at a type number the module does not know of, or listed by mistake.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_group_assoc_lint_member(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_GROUP_ASSOC_INTERNAL_H
