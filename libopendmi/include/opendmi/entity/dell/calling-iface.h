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

typedef struct dmi_dell_calling_iface       dmi_dell_calling_iface_t;
typedef struct dmi_dell_calling_iface_token dmi_dell_calling_iface_token_t;

/**
 * @brief Token of Dell calling interface structure.
 */
struct dmi_dell_calling_iface_token
{
    /**
     * @brief Token identifier.
     */
    uint16_t id;

    /**
     * @brief Location of the token in non-volatile storage, zero for string
     * tokens.
     */
    uint16_t location;

    /**
     * @brief Token value, or string length for string tokens.
     */
    uint16_t value;
};

/**
 * @brief Dell calling interface structure (type 218).
 */
struct dmi_dell_calling_iface
{
    /**
     * @brief I/O port, which is written to issue a command.
     */
    uint16_t cmd_io_address;

    /**
     * @brief Value, which is written to the command I/O port.
     */
    uint8_t cmd_io_code;

    /**
     * @brief Supported commands.
     */
    uint32_t supported_cmds;

    /**
     * @brief Number of tokens.
     */
    size_t token_count;

    /**
     * @brief Tokens, excluding unused ones and the end-of-table marker.
     */
    dmi_dell_calling_iface_token_t *tokens;
};

/**
 * @brief Dell calling interface entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_dell_calling_iface_spec;

#endif // !OPENDMI_ENTITY_DELL_CALLING_IFACE_H
