//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_TRUSTED_MODULE_INTERNAL_H
#define OPENDMI_ENTITY_HPE_TRUSTED_MODULE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/trusted-module.h>

extern const dmi_name_set_t dmi_hpe_tm_presence_names;
extern const dmi_name_set_t dmi_hpe_tm_disable_reason_names;
extern const dmi_name_set_t dmi_hpe_tm_type_names;
extern const dmi_name_set_t dmi_hpe_tm_mounting_names;
extern const dmi_name_set_t dmi_hpe_tm_fips_names;
extern const dmi_name_set_t dmi_hpe_tm_chip_names;
extern const dmi_name_set_t dmi_hpe_tm_error_names;

#endif // !OPENDMI_ENTITY_HPE_TRUSTED_MODULE_INTERNAL_H
