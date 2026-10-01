//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_STRING_PROPERTY_INTERNAL_H
#define OPENDMI_ENTITY_STRING_PROPERTY_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/string-property.h>

/**
 * @internal
 * @brief Attach a string property to the structure it belongs to.
 *
 * @param[in,out] entity Structure being linked.
 *
 * @error DMI_ERROR_ENTITY_NOT_FOUND Parent handle is not specified, or refers
 * to no structure
 * @error DMI_ERROR_INVALID_ENTITY_TYPE Parent is a string property itself
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_string_property_link(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_STRING_PROPERTY_INTERNAL_H
