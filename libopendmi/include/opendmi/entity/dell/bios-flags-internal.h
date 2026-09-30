//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_BIOS_FLAGS_INTERNAL_H
#define OPENDMI_ENTITY_DELL_BIOS_FLAGS_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/dell/bios-flags.h>

bool dmi_dell_bios_flags_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_DELL_BIOS_FLAGS_INTERNAL_H
