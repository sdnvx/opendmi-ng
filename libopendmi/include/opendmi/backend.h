//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_BACKEND_H
#define OPENDMI_BACKEND_H

#pragma once

#include <opendmi/types.h>

typedef struct dmi_backend dmi_backend_t;

/**
 * @brief DMI backend specification.
 */
struct dmi_backend
{
    /**
     * @brief Backend name.
     */
    char *name;

    bool (*open)(dmi_context_t *context, const char *path);

    /**
     * @brief Read SMBIOS entry point. Optional: backends which do not have
     * access to entry point data should leave it unset and fill SMBIOS
     * version on their own when opening the context.
     */
    dmi_data_t *(*read_entry)(dmi_context_t *context, size_t *plength);

    dmi_data_t *(*read_table)(dmi_context_t *context, size_t *plength);
    bool (*close)(dmi_context_t *context);
};

/**
 * @brief Backend handle.
 */
extern __dmi_api dmi_backend_t *dmi_backend;

#endif // !OPENDMI_BACKEND_H
