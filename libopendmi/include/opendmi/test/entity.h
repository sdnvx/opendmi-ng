//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_TEST_ENTITY_H
#define OPENDMI_TEST_ENTITY_H

#pragma once

#include <opendmi/buffer.h>
#include <opendmi/entity.h>

/**
 * @brief Create a structure over the data a test declares.
 *
 * Structures refer to the data of a buffer in place, so a test which builds
 * one out of the bytes of its own gives it a buffer to hold them, which lives
 * as long as the structure does and is destroyed along with it.
 *
 * @param[in,out] buffer Buffer to hold the data, which the caller destroys.
 * @param[in]     data   Data of the structure.
 * @param[in]     length Number of the bytes of the data.
 *
 * @return Structure on success, or @c nullptr on failure.
 */
static inline dmi_entity_t *dmi_test_entity_create(
        dmi_buffer_t *buffer,
        const void   *data,
        size_t        length)
{
    if (!dmi_buffer_assign(buffer, data, length))
        return nullptr;

    return dmi_entity_create(dmi_buffer_context(buffer), buffer, 0);
}

#endif // !OPENDMI_TEST_ENTITY_H
