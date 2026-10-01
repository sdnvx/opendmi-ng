//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_EXTENSION_BOARD_INTERNAL_H
#define OPENDMI_ENTITY_HPE_EXTENSION_BOARD_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/extension-board.h>

/**
 * @internal
 * @brief Names of the types of the extension boards.
 */
extern const dmi_name_set_t dmi_hpe_board_type_names;

/**
 * @internal
 * @brief Names of the positions of the risers.
 */
extern const dmi_name_set_t dmi_hpe_riser_position_names;

#endif // !OPENDMI_ENTITY_HPE_EXTENSION_BOARD_INTERNAL_H
