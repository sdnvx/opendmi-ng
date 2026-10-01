//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_CHANNEL_INTERNAL_H
#define OPENDMI_ENTITY_MEMORY_CHANNEL_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/memory-channel.h>

/**
 * @internal
 * @brief Names of the types of the memory channels.
 */
extern const dmi_name_set_t dmi_memory_channel_type_names;

/**
 * @internal
 * @brief Bind the memory devices of a channel to the channel.
 *
 * @details Devices of a channel are linked by the attributes, and learn the
 * channel they belong to here, since nothing in their own data says so.
 *
 * @param[in,out] entity Structure being linked.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_memory_channel_link(dmi_entity_t *entity);

/**
 * @internal
 * @brief Check the load the devices of a channel put on it.
 *
 * @details Devices of a channel share its capacity, so the load they put on
 * it together fits the maximum it supports.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_memory_channel_lint_load(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MEMORY_CHANNEL_INTERNAL_H
