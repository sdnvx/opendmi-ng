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

#ifndef DMI_HPE_ROM_INFO_T
#   define DMI_HPE_ROM_INFO_T
    typedef struct dmi_hpe_rom_info dmi_hpe_rom_info_t;
#endif // !DMI_HPE_ROM_INFO_T

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

    /**
     * @brief String whose meaning is not established, e.g. `2.9` on a G6
     * server.
     */
    const char *unknown_string;

    /**
     * @brief Whether the version of the redundant ROM is shown, which it is
     * when a redundant ROM is installed, up to Gen11. Gen12 onwards reserve
     * the field.
     */
    bool has_redundant_rom_version;

    /**
     * @brief Whether the file name and the build date of the OEM ROM image
     * are shown, which they are when the file name is neither empty nor
     * begins with blanks.
     */
    bool has_oem_rom;

    /**
     * @brief Whether the structure holds the string whose meaning is not
     * established, which the structures shorter than 10 bytes leave out.
     */
    bool has_unknown_string;
};

/**
 * @brief HP/HPE other ROM information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_rom_info_spec;

#endif // !OPENDMI_ENTITY_HPE_ROM_INFO_H
