//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_ROM_INFO_INTERNAL_H
#define OPENDMI_ENTITY_HPE_ROM_INFO_INTERNAL_H

#pragma once

#include <opendmi/field.h>

#include <opendmi/entity/hpe/rom-info.h>

/**
 * @internal
 * @brief Derive the flags telling which fields of the ROM information are
 * shown.
 *
 * @details Version of the redundant ROM is shown when one is installed, up to
 * Gen11. Details of the OEM ROM image are shown when its file name is neither
 * empty nor begins with blanks, which the raw string is checked for, since the
 * decoded one is trimmed.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_hpe_rom_info_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_ROM_INFO_INTERNAL_H
