//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_OEM_STRINGS_H
#define OPENDMI_ENTITY_OEM_STRINGS_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_oem_strings dmi_oem_strings_t;

/**
 * @brief OEM strings.
 */
struct dmi_oem_strings
{
    /**
     * @brief Number of strings.
     */
    size_t string_count;

    /**
     * @brief String values.
     */
    const char **strings;
};

/**
 * @brief OEM strings entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_oem_strings_spec;

#endif // !OPENDMI_ENTITY_OEM_STRINGS_H
