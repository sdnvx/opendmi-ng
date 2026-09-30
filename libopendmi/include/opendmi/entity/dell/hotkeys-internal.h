//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_HOTKEYS_INTERNAL_H
#define OPENDMI_ENTITY_DELL_HOTKEYS_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/dell/hotkeys.h>

void dmi_dell_hotkeys_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_DELL_HOTKEYS_INTERNAL_H
