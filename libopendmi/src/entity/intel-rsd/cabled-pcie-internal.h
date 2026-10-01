//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_RSD_CABLED_PCIE_INTERNAL_H
#define OPENDMI_ENTITY_INTEL_RSD_CABLED_PCIE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/intel-rsd/cabled-pcie.h>

/**
 * @internal
 * @brief Check that the start lane of every cable index is one of 0, 4, 8
 * and 12.
 *
 * @details A cable carries a group of four lanes, and the groups start at the
 * lanes of a x16 port which are multiples of four.
 */
void dmi_intel_rsd_cabled_pcie_lint_start_lane(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the number of the cable indices does not exceed the one
 * the specification lays out.
 *
 * @details The number is read as stored, since the decoder keeps only the
 * cable indices the structure holds.
 */
void dmi_intel_rsd_cabled_pcie_lint_count(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_INTEL_RSD_CABLED_PCIE_INTERNAL_H
