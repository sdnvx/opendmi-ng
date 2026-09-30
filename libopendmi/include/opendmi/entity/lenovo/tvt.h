//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_LENOVO_TVT_H
#define OPENDMI_ENTITY_LENOVO_TVT_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_lenovo_tvt dmi_lenovo_tvt_t;

/**
 * @brief Lenovo ThinkVantage Technologies enablement (type 131).
 *
 * Tells which ThinkVantage Technologies the platform enables. The structure
 * is told by its length and its first string, `TVT-Enablement`, since Intel
 * vPro information takes type 131 in the same table.
 */
struct dmi_lenovo_tvt
{
    /**
     * @brief Version of the structure.
     */
    uint8_t version;

    /**
     * @brief Bits of the technologies, 128 of them, of which only the last
     * one is known.
     */
    dmi_binary_t features;

    /**
     * @brief Whether the diagnostics (PC-Doctor) are available, as bit 127 of
     * the features tells.
     */
    bool is_diagnostics;

    /**
     * @brief Signature, `TVT-Enablement`, which the structure is told by.
     */
    const char *signature;
};

/**
 * @brief Lenovo ThinkVantage Technologies enablement entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_lenovo_tvt_spec;

#endif // !OPENDMI_ENTITY_LENOVO_TVT_H
