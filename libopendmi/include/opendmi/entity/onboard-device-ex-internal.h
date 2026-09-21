//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_ONBOARD_DEVICE_EX_INTERNAL_H
#define OPENDMI_ENTITY_ONBOARD_DEVICE_EX_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/onboard-device-ex.h>

// Checks the lint rules of the specification perform, see onboard-device-ex-rules.c
void dmi_onboard_device_ex_lint_instance(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_ONBOARD_DEVICE_EX_INTERNAL_H
