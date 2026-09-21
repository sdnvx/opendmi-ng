//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_RSD_CABLED_PCIE_INTERNAL_H
#define OPENDMI_ENTITY_RSD_CABLED_PCIE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/intel/rsd-cabled-pcie.h>

// Operation handlers, see rsd-cabled-pcie-handlers.c
void dmi_intel_rsd_cabled_pcie_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_RSD_CABLED_PCIE_INTERNAL_H
