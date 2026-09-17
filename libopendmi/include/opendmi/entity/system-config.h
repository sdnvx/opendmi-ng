//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SYSTEM_CONFIG_H
#define OPENDMI_ENTITY_SYSTEM_CONFIG_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_system_config_opts dmi_system_config_opts_t;

/**
 * @brief System configuration options structure (type 12).
 */
struct dmi_system_config_opts
{
    /**
     * @brief Number of options.
     */
    size_t option_count;

    /**
     * @brief Option names.
     */
    const char **options;
};

/**
 * @brief System configuration options entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_system_config_opts_spec;

#endif // !OPENDMI_ENTITY_SYSTEM_CONFIG_H
