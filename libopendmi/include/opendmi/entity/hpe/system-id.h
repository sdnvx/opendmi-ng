//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_SYSTEM_ID_H
#define OPENDMI_ENTITY_HPE_SYSTEM_ID_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/utils/uuid.h>

typedef struct dmi_hpe_system_id dmi_hpe_system_id_t;

/**
 * @brief HP/HPE server system ID (type 195).
 *
 * Gives the unique ID of the system, which replaces the EISA ID of the older
 * systems, and the platform ID iLO tells the platform by.
 */
struct dmi_hpe_system_id
{
    /**
     * @brief Server system ID, e.g. `$0E110761`.
     */
    const char *system_id;

    /**
     * @brief Bytes of the platform ID, as the CPLD holds them, low byte
     * first. The order the ID is written in is not documented.
     */
    uint8_t platform_id[2];

    /**
     * @brief GUID, reserved from Gen11 onwards.
     */
    dmi_uuid_t guid;
};

/**
 * @brief HP/HPE server system ID entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_system_id_spec;

#endif // !OPENDMI_ENTITY_HPE_SYSTEM_ID_H
