//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_LENOVO_OEM_INTERNAL_H
#define OPENDMI_ENTITY_LENOVO_OEM_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/lenovo/oem.h>

bool dmi_lenovo_device_presence_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_LENOVO_OEM_INTERNAL_H
