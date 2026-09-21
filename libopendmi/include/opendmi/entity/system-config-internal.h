//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SYSTEM_CONFIG_INTERNAL_H
#define OPENDMI_ENTITY_SYSTEM_CONFIG_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/system-config.h>

// Operation handlers, see system-config-handlers.c
bool dmi_system_config_opts_derive(dmi_entity_t *entity);
void dmi_system_config_opts_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_SYSTEM_CONFIG_INTERNAL_H
