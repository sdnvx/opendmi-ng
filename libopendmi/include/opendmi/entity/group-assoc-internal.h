//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_GROUP_ASSOC_INTERNAL_H
#define OPENDMI_ENTITY_GROUP_ASSOC_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/group-assoc.h>

// Operation handlers, see group-assoc-handlers.c
bool dmi_group_assoc_link(dmi_entity_t *entity);

// Checks the lint rules of the specification perform, see group-assoc-rules.c
void dmi_group_assoc_lint_member(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_GROUP_ASSOC_INTERNAL_H
