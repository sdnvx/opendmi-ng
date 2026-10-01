//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_APPLE_SMC_VERSION_INTERNAL_H
#define OPENDMI_ENTITY_APPLE_SMC_VERSION_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/apple/smc-version.h>

bool dmi_apple_smc_version_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_APPLE_SMC_VERSION_INTERNAL_H
