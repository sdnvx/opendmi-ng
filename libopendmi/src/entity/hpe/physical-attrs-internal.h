//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_PHYSICAL_ATTRS_INTERNAL_H
#define OPENDMI_ENTITY_HPE_PHYSICAL_ATTRS_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>

#include <opendmi/entity/hpe/physical-attrs.h>

/**
 * @internal
 * @brief Derive the product number and the serial number from the raw
 * identifier.
 *
 * @details Identifier is left as `nullptr` if the bytes it is made of are not
 * printable.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_hpe_physical_attrs_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_PHYSICAL_ATTRS_INTERNAL_H
