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

void dmi_intel_rsd_cabled_pcie_lint_start_lane(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_intel_rsd_cabled_pcie_lint_count(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_INTEL_RSD_CABLED_PCIE_INTERNAL_H
