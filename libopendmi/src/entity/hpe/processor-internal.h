//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_PROCESSOR_INTERNAL_H
#define OPENDMI_ENTITY_HPE_PROCESSOR_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/processor.h>

/**
 * @internal
 * @brief Derive the QDF or S-Spec number from the raw one the structure
 * holds.
 *
 * @details Number is taken only when it is printable, with the spaces padding
 * it left out.
 *
 * @param[in,out] entity Entity of the decoded structure.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_hpe_processor_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_PROCESSOR_INTERNAL_H
