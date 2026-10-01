//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_BACKPLANE_INTERNAL_H
#define OPENDMI_ENTITY_HPE_BACKPLANE_INTERNAL_H

#pragma once

#include <opendmi/field.h>

#include <opendmi/entity/hpe/backplane.h>

/**
 * @internal
 * @brief Derive whether the legacy details of the backplane are shown.
 *
 * @details Numbers of the bays behind ports 0xA0 and 0xA2 and the name are
 * shown on the platforms before Gen10 Plus, and on the ones of an unknown
 * generation.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_hpe_backplane_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_BACKPLANE_INTERNAL_H
