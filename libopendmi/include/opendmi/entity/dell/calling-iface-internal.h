//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//

#ifndef OPENDMI_ENTITY_CALLING_IFACE_INTERNAL_H
#define OPENDMI_ENTITY_CALLING_IFACE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/dell/calling-iface.h>

/**
 * @brief Token identifier of unused tokens.
 */
#define DMI_DELL_TOKEN_UNUSED 0x0000u

/**
 * @brief Token identifier of the end-of-table marker.
 */
#define DMI_DELL_TOKEN_EOT 0xFFFFu

/**
 * @brief Size of the token in the structure.
 */
#define DMI_DELL_CALLING_IFACE_TOKEN_SIZE 6

// Operation handlers, see calling-iface-handlers.c
extern const dmi_attribute_t dmi_dell_calling_iface_token_attrs[];
bool dmi_dell_calling_iface_decode(dmi_entity_t *entity);
void dmi_dell_calling_iface_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_CALLING_IFACE_INTERNAL_H
