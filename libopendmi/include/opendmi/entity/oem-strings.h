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

#ifndef DMI_OEM_STRINGS_T
#   define DMI_OEM_STRINGS_T
    typedef struct dmi_oem_strings dmi_oem_strings_t;
#endif // !DMI_OEM_STRINGS_T

/**
 * @brief OEM strings structure (type 11).
 *
 * Holds free-form strings defined by the OEM, such as part numbers of
 * reference documents or contact information of the manufacturer.
 */
struct dmi_oem_strings
{
    /**
     * @brief Number of strings.
     */
    size_t string_count;

    /**
     * @brief Array of `string_count` strings, in the order of their numbers,
     * or `nullptr` if there are no strings. An element is `nullptr` if the
     * structure lacks the string.
     */
    const char **strings;
};

/**
 * @brief OEM strings entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_oem_strings_spec;

#endif // !OPENDMI_ENTITY_OEM_STRINGS_H
