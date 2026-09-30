//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_MEMORY_IDS_H
#define OPENDMI_ENTITY_DELL_MEMORY_IDS_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_dell_memory_id  dmi_dell_memory_id_t;
typedef struct dmi_dell_memory_ids dmi_dell_memory_ids_t;

/**
 * @brief Identifiers of a memory module.
 *
 * The identifiers are the ones of the serial presence detect (SPD) data of
 * the module. Sockets which hold no module carry bytes of `0xFF`.
 */
struct dmi_dell_memory_id
{
    /**
     * @brief Handle of the memory device (type 17) of the socket.
     */
    dmi_handle_t handle;

    /**
     * @brief JEDEC identifier of the manufacturer of the module, of 8 bytes,
     * whose leading bytes of `0x7F` are continuation codes, e.g. `CE` for
     * Samsung, or `7F 98` for Kingston.
     */
    dmi_binary_t manufacturer;

    /**
     * @brief Serial number of the module, of 4 bytes, in the order the SPD
     * data holds them.
     */
    dmi_binary_t serial_number;
};

/**
 * @brief Dell memory module identifiers structure (type 223).
 *
 * Tells the manufacturer and the serial number of the memory modules, which
 * the memory devices (type 17) of the older systems carry no fields for.
 * Reverse engineered from the data corpus.
 */
struct dmi_dell_memory_ids
{
    /**
     * @brief Length of the identifiers of a module, which follow the handle
     * of its memory device, `12` in all known data.
     */
    uint8_t id_length;

    /**
     * @brief Number of memory modules.
     */
    size_t module_count;

    /**
     * @brief Identifiers of the memory modules.
     */
    dmi_dell_memory_id_t *modules;
};

/**
 * @brief Dell memory module identifiers entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_dell_memory_ids_spec;

#endif // !OPENDMI_ENTITY_DELL_MEMORY_IDS_H
