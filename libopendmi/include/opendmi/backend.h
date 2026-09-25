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
#include <opendmi/buffer.h>

typedef struct dmi_backend dmi_backend_t;

/**
 * @brief DMI backend specification.
 *
 * A backend gives the framework the bytes of a platform: where they come from
 * is its own business, and what they mean is not. The data it reads is put
 * into the buffers of the context, which owns them and frees them when the
 * context is closed, so that a backend holds nothing of the data it has read.
 */
struct dmi_backend
{
    /**
     * @brief Backend name.
     */
    char *name;

    /**
     * @brief Open the source of the data, e.g. a memory device or a file.
     *
     * Whatever the backend needs while it is open is its session, see
     * `dmi_context_state_t::session`, which `close` disposes of.
     */
    bool (*open)(dmi_context_t *context, const char *path);

    /**
     * @brief Read the SMBIOS entry point into @p buffer.
     *
     * Optional: backends which do not have access to entry point data should
     * leave it unset and fill SMBIOS version on their own when opening the
     * context.
     *
     * @param[in]  context Context being opened.
     * @param[out] buffer  Buffer to read the entry point into, which the
     *                     backend fills rather than allocates.
     *
     * @return `true` on success, `false` otherwise.
     */
    bool (*read_entry)(dmi_context_t *context, dmi_buffer_t *buffer);

    /**
     * @brief Read the SMBIOS structure table into @p buffer.
     *
     * @param[in]  context Context being opened.
     * @param[out] buffer  Buffer to read the table into, which the backend
     *                     fills rather than allocates.
     *
     * @return `true` on success, `false` otherwise.
     */
    bool (*read_table)(dmi_context_t *context, dmi_buffer_t *buffer);

    /**
     * @brief Close the source of the data and dispose of the session.
     *
     * The data which has been read belongs to the context rather than to the
     * backend, and is none of its business by this point.
     */
    bool (*close)(dmi_context_t *context);
};

/**
 * @brief Backend handle.
 */
extern __dmi_api dmi_backend_t *dmi_backend;

#endif // !OPENDMI_BACKEND_H
