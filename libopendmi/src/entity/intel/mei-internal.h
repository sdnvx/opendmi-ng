//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_MEI_INTERNAL_H
#define OPENDMI_ENTITY_INTEL_MEI_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/intel/mei.h>

extern const dmi_name_set_t dmi_intel_me_state_names;

/**
 * @internal
 * @brief Names of the error codes, as coreboot names them.
 */
extern const dmi_name_set_t dmi_intel_me_error_names;

extern const dmi_name_set_t dmi_intel_me_mode_names;
extern const dmi_name_set_t dmi_intel_me_sku_names;

bool dmi_intel_mei_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_INTEL_MEI_INTERNAL_H
