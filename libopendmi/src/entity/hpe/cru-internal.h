//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_CRU_INTERNAL_H
#define OPENDMI_ENTITY_HPE_CRU_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/cru.h>

/**
 * @internal
 * @brief Compute the entry point of the CRU service and check its signature.
 *
 * @details Entry point is the sum of the base address and the offset, and the
 * service is told to be the CRU one by the `$CRU` signature.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_hpe_cru_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_CRU_INTERNAL_H
