//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_CHANNEL_INTERNAL_H
#define OPENDMI_ENTITY_MEMORY_CHANNEL_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/memory-channel.h>

// Value names, see memory-channel-names.c
extern const dmi_name_set_t dmi_memory_channel_type_names;

// Operation handlers, see memory-channel-handlers.c
bool dmi_memory_channel_link(dmi_entity_t *entity);
void dmi_memory_channel_cleanup(dmi_entity_t *entity);

// Checks the lint rules of the specification perform, see memory-channel-rules.c
void dmi_memory_channel_lint_load(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MEMORY_CHANNEL_INTERNAL_H
