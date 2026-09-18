//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_STRING_PROPERTY_H
#define OPENDMI_ENTITY_STRING_PROPERTY_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_STRING_PROPERTY_T
#   define DMI_STRING_PROPERTY_T
    typedef struct dmi_string_property dmi_string_property_t;
#endif // !DMI_STRING_PROPERTY_T

/**
 * @brief String property structure (type 46).
 *
 * This structure defines a string property for another structure. This allows
 * adding string properties that are common to several structures without
 * having to modify the definitions of these structures. Multiple type 46
 * structures can add string properties to the same parent structure.
 *
 * @since SMBIOS 3.5
 */
struct dmi_string_property
{
    /**
     * @brief Property identifier.
     */
    dmi_property_t ident;

    /**
     * @brief Property value.
     */
    const char *value;

    /**
     * @brief Handle corresponding to the entity this string property
     * applies to.
     */
    dmi_handle_t parent_handle;

    /**
     * @brief The entity this string property applies to.
     */
    dmi_entity_t *parent;
};

/**
 * @brief String property entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_string_property_spec;

#endif // !OPENDMI_ENTITY_STRING_PROPERTY_H
