//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_ONBOARD_DEVICE_INTERNAL_H
#define OPENDMI_ENTITY_ONBOARD_DEVICE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/onboard-device.h>

// Value names, see onboard-device-names.c
extern const dmi_name_set_t dmi_onboard_device_type_names;

// Operation handlers, see onboard-device-handlers.c
void dmi_onboard_device_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_ONBOARD_DEVICE_INTERNAL_H
