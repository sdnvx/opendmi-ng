//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_CALLING_IFACE_H
#define OPENDMI_ENTITY_DELL_CALLING_IFACE_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_dell_calling_iface_data dmi_dell_calling_iface_data_t;
typedef struct dmi_dell_calling_iface_token dmi_dell_calling_iface_token_t;

/**
 * @brief Dell calling interface token.
 */
dmi_packed_struct(dmi_dell_calling_iface_token)
{
    /**
     * @brief Token identifier.
     */
    uint16_t id;

    /**
     * @brief Location.
     */
    uint16_t location;

    /**
     * @brief Token value or string length.
     */
    uint16_t length;
};

/**
 * @brief Dell calling interface structure (type 218).
 */
dmi_packed_struct(dmi_dell_calling_iface_data)
{
    /**
     * @brief SMBIOS structure header.
     */
    dmi_header_t header;

    /**
     * @brief Command address.
     */
    uint16_t command_addr;

    /**
     * @brief Command code.
     */
    uint8_t command_code;

    /**
     * @brief Supported commands.
     */
    uint32_t supported_commands;

    /**
     * @brief Tokens.
     */
    dmi_dell_calling_iface_token_t tokens[];
};

/**
 * @brief Dell calling interface entity specification.
 */
extern const dmi_entity_spec_t dmi_dell_calling_iface_spec;

#endif // !OPENDMI_ENTITY_DELL_CALLING_IFACE_H
