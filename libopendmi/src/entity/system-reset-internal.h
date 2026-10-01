//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SYSTEM_RESET_INTERNAL_H
#define OPENDMI_ENTITY_SYSTEM_RESET_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/system-reset.h>

// Value names, see system-reset-names.c
extern const dmi_name_set_t dmi_boot_option_names;

// Checks the lint rules of the specification perform, see system-reset-rules.c
void dmi_system_reset_lint_limit(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_SYSTEM_RESET_INTERNAL_H
