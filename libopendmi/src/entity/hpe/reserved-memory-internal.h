//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_RESERVED_MEMORY_INTERNAL_H
#define OPENDMI_ENTITY_HPE_RESERVED_MEMORY_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/reserved-memory.h>

/**
 * @internal
 * @brief Derive the sizes and the signatures of the reserved memory entries.
 *
 * @details Size of an entry is its raw size in bytes or in kilobytes, as the
 * entry says, and the signature is the text its raw bytes spell.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_hpe_reserved_memory_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_RESERVED_MEMORY_INTERNAL_H
