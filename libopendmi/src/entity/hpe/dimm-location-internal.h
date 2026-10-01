//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_DIMM_LOCATION_INTERNAL_H
#define OPENDMI_ENTITY_HPE_DIMM_LOCATION_INTERNAL_H

#pragma once

#include <opendmi/field.h>

#include <opendmi/entity/hpe/dimm-location.h>

/**
 * @internal
 * @brief Derive the flags telling which fields of a DIMM location are shown.
 *
 * @details Board number of `UINT8_MAX` tells a socket of the system board, and
 * the fields of the Innovation Engine are shown up to Gen11, as the platform
 * generation tells.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_hpe_dimm_location_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_DIMM_LOCATION_INTERNAL_H
