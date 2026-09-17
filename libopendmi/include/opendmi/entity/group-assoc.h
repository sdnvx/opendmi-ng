//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_GROUP_ASSOC_H
#define OPENDMI_ENTITY_GROUP_ASSOC_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_group_assoc      dmi_group_assoc_t;
typedef struct dmi_group_assoc_item dmi_group_assoc_item_t;

/**
 * @brief Group associations structure (type 14).
 */
struct dmi_group_assoc
{
    /**
     * @brief String describing the group.
     */
    const char *group_name;

    /**
     * @brief Number of associated items (structures).
     */
    size_t item_count;

    /**
     * @brief Item (structure) type of this member.
     */
    dmi_group_assoc_item_t *items;
};

struct dmi_group_assoc_item
{
    /**
     * @brief Item (structure) type of this member.
     */
    dmi_type_t type;

    /**
     * @brief Handle corresponding to this structure.
     */
    dmi_handle_t handle;

    /**
     * @brief Entity corresponding to this structure.
     */
    dmi_entity_t *entity;
};

/**
 * @brief Group associations entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_group_assoc_spec;

#endif // !OPENDMI_ENTITY_GROUP_ASSOC_H
