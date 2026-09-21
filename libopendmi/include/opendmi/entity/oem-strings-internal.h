//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_OEM_STRINGS_INTERNAL_H
#define OPENDMI_ENTITY_OEM_STRINGS_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/oem-strings.h>

// Operation handlers, see oem-strings-handlers.c
bool dmi_oem_strings_derive(dmi_entity_t *entity);
void dmi_oem_strings_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_OEM_STRINGS_INTERNAL_H
