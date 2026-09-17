//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_BASE64_H
#define OPENDMI_UTILS_BASE64_H

#pragma once

#include <opendmi/types.h>

__BEGIN_DECLS

/**
 * @brief Encodes binary data into a Base64 string.
 *
 * Allocates and returns a null-terminated Base64-encoded string representing
 * the given binary buffer. The caller is responsible for freeing the returned
 * buffer.
 *
 * @param[in]  data            Pointer to the binary data to encode.
 * @param[in]  data_length     Number of bytes in @p data.
 * @param[out] poutput_length  If not @c nullptr, receives the length of the returned
 *                             string, excluding the null terminator.
 *
 * @return A newly allocated null-terminated Base64 string on success,
 *         or @c nullptr on failure (e.g., allocation error or invalid arguments).
 */
char *dmi_base64_encode(
        const dmi_data_t *data,
        size_t            data_length,
        size_t           *poutput_length);

/**
 * @brief Decodes a Base64 string into binary data.
 *
 * Allocates and returns a buffer containing the decoded binary data
 * corresponding to the given Base64-encoded string. The caller is responsible
 * for freeing the returned buffer.
 *
 * @param[in] data             Pointer to the Base64-encoded string to decode.
 * @param[in] data_length      Length of @p data in bytes, excluding any null
 *                             terminator.
 * @param[out] poutput_length  If not @c nullptr, receives the number of bytes in the
 *                             returned buffer.
 *
 * @return A newly allocated buffer containing the decoded binary data on
 *         success, or @c nullptr on failure (e.g., allocation error, invalid
 *         Base64 input, or invalid arguments).
 */
dmi_data_t *dmi_base64_decode(
        const char *data,
        size_t      data_length,
        size_t     *poutput_length);

__END_DECLS

#endif // !OPENDMI_UTILS_BASE64_H
