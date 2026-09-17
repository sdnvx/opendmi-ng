//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_REVISIONS_H
#define OPENDMI_ENTITY_DELL_REVISIONS_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_dell_revisions dmi_dell_revisions_t;

/**
 * @brief Dell revisions and IDs structure (type 208).
 */
struct dmi_dell_revisions
{
    /**
     * @brief Implementation version.
     */
    dmi_version_t impl_version;

    /**
     * @brief System ID.
     */
    uint16_t system_id;

    /**
     * @brief Hardware revision.
     */
    uint8_t hardware_revision;

    /**
     * @brief Manufacture date (optional).
     * @todo Use `dmi_date_t` instead of string.
     */
    const char *manufacture_date;

    /**
     * @brief First power-on date (optional).
     * @todo Use `dmi_date_t` instead of string.
     */
    const char *first_poweron_date;
};

/**
 * @brief Dell revisions and IDs entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_dell_revisions_spec;

#endif // !OPENDMI_ENTITY_DELL_REVISIONS_H
