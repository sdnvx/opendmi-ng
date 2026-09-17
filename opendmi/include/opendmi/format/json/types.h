//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_FORMAT_JSON_TYPES_H
#define OPENDMI_FORMAT_JSON_TYPES_H

#pragma once

#include <stdio.h>
#include <yajl/yajl_gen.h>

#include <opendmi/types.h>
#include <opendmi/attribute.h>
#include <opendmi/format.h>

typedef struct dmi_json_session
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
     * @brief YAJL generator handle.
     */
    struct yajl_gen_t *generator;
} dmi_json_session_t;

#endif // !OPENDMI_FORMAT_JSON_TYPES_H
