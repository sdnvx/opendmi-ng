//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_ROM_INFO_H
#define OPENDMI_ENTITY_HPE_ROM_INFO_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_hpe_rom_info dmi_hpe_rom_info_t;

/**
 * @brief HP/HPE other ROM information (type 193).
 *
 * Describes the redundant system ROM and the OEM ROM image of a server.
 */
struct dmi_hpe_rom_info
{
    /**
     * @brief Whether a redundant ROM is installed.
     */
    bool is_redundant_rom;

    /**
     * @brief Version of the redundant ROM, which is its release date.
     */
    const char *redundant_rom_version;

    /**
     * @brief Version string up to Gen8, whose meaning is not documented. Its
     * dates are earlier than the ones of the ROM, which suggests the boot
     * block. Gen9 onwards reserve the byte.
     */
    const char *bootblock_version;

    /**
     * @brief File name of the OEM ROM image, blank if there is none.
     */
    const char *oem_rom_filename;

    /**
     * @brief Build date of the OEM ROM image.
     */
    const char *oem_rom_date;
};

/**
 * @brief HP/HPE other ROM information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_rom_info_spec;

#endif // !OPENDMI_ENTITY_HPE_ROM_INFO_H
