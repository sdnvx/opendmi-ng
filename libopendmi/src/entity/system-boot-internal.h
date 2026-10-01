//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SYSTEM_BOOT_INTERNAL_H
#define OPENDMI_ENTITY_SYSTEM_BOOT_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/system-boot.h>

// Value names, see system-boot-names.c
extern const dmi_name_set_t dmi_system_boot_status_names;

// Operation handlers, see system-boot-handlers.c
bool dmi_system_boot_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_SYSTEM_BOOT_INTERNAL_H
