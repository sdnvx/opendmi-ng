//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_LENOVO_TVT_INTERNAL_H
#define OPENDMI_ENTITY_LENOVO_TVT_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>

#include <opendmi/entity/lenovo/tvt.h>

/**
 * @internal
 * @brief Derive whether the diagnostics are available.
 *
 * @details Availability of the diagnostics is told by bit 127 of the features,
 * which is the last one of the 16 bytes they take.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_lenovo_tvt_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_LENOVO_TVT_INTERNAL_H
