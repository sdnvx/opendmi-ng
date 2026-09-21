//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_ADDITIONAL_INFO_INTERNAL_H
#define OPENDMI_ENTITY_ADDITIONAL_INFO_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/additional-info.h>

// Operation handlers, see additional-info-handlers.c
bool dmi_additional_info_decode(dmi_entity_t *entity);
void dmi_additional_info_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_ADDITIONAL_INFO_INTERNAL_H
