//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MGMT_DEVICE_INTERNAL_H
#define OPENDMI_ENTITY_MGMT_DEVICE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/mgmt-device.h>

// Value names, see mgmt-device-names.c
extern const dmi_name_set_t dmi_mgmt_device_type_names;
extern const dmi_name_set_t dmi_mgmt_device_addr_type_names;

#endif // !OPENDMI_ENTITY_MGMT_DEVICE_INTERNAL_H
