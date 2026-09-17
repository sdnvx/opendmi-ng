//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_FORMAT_YAML_TYPES_H
#define OPENDMI_FORMAT_YAML_TYPES_H

#pragma once

#include <stdio.h>
#include <yaml.h>

#include <opendmi/types.h>
#include <opendmi/attribute.h>
#include <opendmi/format.h>

#define YAML_BINARY_TAG "tag:yaml.org,2002:binary"

typedef struct dmi_yaml_session
{
    /**
     * @brief Context handle.
     */
    dmi_context_t *context;

    /**
     * @brief Output options.
     */
    dmi_format_options_t options;

    /**
     * @brief Output stream handle.
     */
    FILE *stream;

    /**
     * @brief YAML emitter handle.
     */
    yaml_emitter_t *emitter;
} dmi_yaml_session_t;

#endif // !OPENDMI_FORMAT_YAML_TYPES_H
