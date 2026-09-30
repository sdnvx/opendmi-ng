//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_SYSTEM_ID_H
#define OPENDMI_ENTITY_DELL_SYSTEM_ID_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_dell_system_id dmi_dell_system_id_t;

/**
 * @brief Dell system ID record structure (type 255).
 *
 * Holds the system ID of the platform in hexadecimal digits, e.g. `0A64`,
 * which the revisions and IDs (type 208) and the OEM strings (type 11, key
 * `1`) hold too, along with an identifier beginning with `_SID`. Reverse
 * engineered from the data corpus.
 */
struct dmi_dell_system_id
{
    /**
     * @brief Identifier beginning with `_SID`, whose meaning is not
     * established.
     */
    const char *identifier;

    /**
     * @brief System ID, in hexadecimal digits.
     */
    const char *system_id;
};

/**
 * @brief Dell system ID record entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_dell_system_id_spec;

#endif // !OPENDMI_ENTITY_DELL_SYSTEM_ID_H
