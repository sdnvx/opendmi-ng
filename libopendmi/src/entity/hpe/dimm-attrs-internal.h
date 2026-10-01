//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_DIMM_ATTRS_INTERNAL_H
#define OPENDMI_ENTITY_HPE_DIMM_ATTRS_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/dimm-attrs.h>

/**
 * @internal
 * @brief Derive the flags of a DIMM from its raw attributes.
 *
 * @details SmartMemory is told by the two low bits, of which values 2 and 3
 * leave it undefined. Load-reduced and standard memory flags are told by the
 * pairs of bits starting at bits 2 and 4: the lower bit says whether the flag
 * is defined, and the higher one holds its value.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_hpe_dimm_attrs_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_DIMM_ATTRS_INTERNAL_H
