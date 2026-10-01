//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_INVENTORY_INTERNAL_H
#define OPENDMI_ENTITY_HPE_INVENTORY_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/inventory.h>

/**
 * @internal
 * @brief Derive the flags of the firmware component from its attributes.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_hpe_inventory_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_INVENTORY_INTERNAL_H
