//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_FIRMWARE_LANGUAGE_INTERNAL_H
#define OPENDMI_ENTITY_FIRMWARE_LANGUAGE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/firmware-language.h>

// Value names, see firmware-language-names.c
extern const dmi_name_set_t dmi_firmware_language_flag_names;

// Operation handlers, see firmware-language-handlers.c
bool dmi_firmware_language_derive(dmi_entity_t *entity);
void dmi_firmware_language_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_FIRMWARE_LANGUAGE_INTERNAL_H
