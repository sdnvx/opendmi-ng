//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_CURRENT_PROBE_INTERNAL_H
#define OPENDMI_ENTITY_CURRENT_PROBE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/current-probe.h>

// Checks the lint rules of the specification perform, see current-probe-rules.c
void dmi_current_probe_lint_range(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_CURRENT_PROBE_INTERNAL_H
