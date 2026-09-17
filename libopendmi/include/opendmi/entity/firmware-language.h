//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_FIRMWARE_LANGUAGE_H
#define OPENDMI_ENTITY_FIRMWARE_LANGUAGE_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_firmware_language       dmi_firmware_language_t;
typedef union  dmi_firmware_language_flags dmi_firmware_language_flags_t;

/**
 * @brief Firmware language flags.
 */
dmi_packed_union(dmi_firmware_language_flags)
{
    /**
     * @brief Raw value.
     */
    dmi_byte_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief If set to `true`, the current language strings use the
         * abbreviated format. Otherwise, the strings use the long format.
         */
        dmi_byte_t is_abbreviated : 1;

        /**
         * @brief Reserved for future use.
         */
        dmi_byte_t __reserved: 7;
    };
};

dmi_static_assert_value_union(dmi_firmware_language_flags);

/**
 * @brief Firmware language information structure (type 13).
 */
struct dmi_firmware_language
{
    /**
     * @brief Number of languages available.
     */
    size_t language_count;

    /**
     * @brief List of available languages.
     */
    const char **languages;

    /**
     * @brief Flags.
     */
    dmi_firmware_language_flags_t flags;

    /**
     * @brief Currently installed language.
     */
    const char *current_language;
};

/**
 * @brief Firmware language information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_firmware_language_spec;

#endif // !OPENDMI_ENTITY_FIRMWARE_LANGUAGE_H
