//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//

#ifndef OPENDMI_ENTITY_MEMORY_DEVICE_EX_INTERNAL_H
#define OPENDMI_ENTITY_MEMORY_DEVICE_EX_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/sun/memory-device-ex.h>

// Operation handlers, see memory-device-ex-handlers.c
void dmi_sun_memory_device_ex_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MEMORY_DEVICE_EX_INTERNAL_H
