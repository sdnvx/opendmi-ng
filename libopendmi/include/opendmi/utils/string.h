//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_STRING_H
#define OPENDMI_UTILS_STRING_H

#pragma once

#include <stdarg.h>
#include <opendmi/types.h>

__BEGIN_DECLS

/**
 * @brief Allocates and formats a string, similar to `sprintf()`.
 *
 * A cross-platform wrapper around `asprintf()`. Allocates a buffer large
 * enough to hold the formatted result, writes the output into it, and stores
 * a pointer to the buffer in @p *strp. The caller is responsible for freeing
 * the returned buffer.
 *
 * @param[out] strp    On success, receives a pointer to the newly allocated
 *                     formatted string. On failure, set to @c nullptr.
 * @param[in]  format  `printf`-style format string.
 * @param[in]  ...     Arguments for the format string.
 *
 * @return The number of bytes written, excluding the null terminator, on
 *         success, or a negative value on failure (e.g., allocation error or
 *         formatting error).
 */
__dmi_api int dmi_asprintf(char **strp, const char *format, ...);

/**
 * @brief Allocates and formats a string from a `va_list`, similar to
 *        `vsprintf()`.
 *
 * A cross-platform wrapper around `vasprintf()`. Equivalent to
 * `dmi_asprintf()`, but accepts a `va_list` instead of a variadic argument
 * list. Allocates a buffer large enough to hold the formatted result, writes
 * the output into it, and stores a pointer to the buffer in @p *strp. The
 * caller is responsible for freeing the returned buffer.
 *
 * @param[out] strp    On success, receives a pointer to the newly allocated
 *                     formatted string. On failure, set to @c nullptr.
 * @param[in]  format  `printf`-style format string.
 * @param[in]  args    Argument list for the format string.
 *
 * @return The number of bytes written, excluding the null terminator, on
 *         success, or a negative value on failure (e.g., allocation error or
 *         formatting error).
 */
__dmi_api int dmi_vasprintf(char **strp, const char *format, va_list args);

/**
 * @brief Converts all characters in a string to lowercase in place.
 *
 * Iterates over the null-terminated string @p str and replaces each character
 * with its lowercase equivalent using `tolower()`. The string is modified in
 * place; no new buffer is allocated. Does nothing if @p str is @c nullptr.
 *
 * @param[in,out] str  Null-terminated string to convert, or @c nullptr.
 */
__dmi_api void dmi_string_tolower(char *str);

/**
 * @brief Converts all characters in a string to uppercase in place.
 *
 * Iterates over the null-terminated string @p str and replaces each character
 * with its uppercase equivalent using `toupper()`. The string is modified in
 * place; no new buffer is allocated. Does nothing if @p str is @c nullptr.
 *
 * @param[in,out] str  Null-terminated string to convert, or @c nullptr.
 */
__dmi_api void dmi_string_toupper(char *str);

/**
 * @brief Replaces an owned string with a copy of another one.
 *
 * Copies @p value, frees the string @p pstring points to, and stores the copy
 * in its place. The string is left as it was if memory is exhausted, so that
 * a failure changes nothing.
 *
 * @param[in]     context Context whose error queue errors are raised against,
 *                        or @c nullptr to report none.
 * @param[in,out] pstring Pointer to the variable holding the string, which is
 *                        either @c nullptr or allocated with `malloc()`.
 * @param[in]     value   String to copy, or @c nullptr to free the string and
 *                        leave the variable holding @c nullptr.
 *
 * @return `true` on success, `false` otherwise.
 *
 * @error DMI_ERROR_ARGUMENT_NULL Pointer to the variable is `nullptr`
 * @error DMI_ERROR_OUT_OF_MEMORY Memory is exhausted
 */
__dmi_api bool dmi_string_set(dmi_context_t *context, char **pstring, const char *value);

/**
 * @brief Check whether a string is a placeholder of the firmware vendor.
 *
 * Firmware leaves strings such as `To Be Filled By O.E.M.` or `Default
 * string` in place of the data it has none of. They are compared regardless
 * of case.
 *
 * @param[in] text String to check, or @c nullptr.
 *
 * @return `true` if @p text is a placeholder, `false` otherwise, or if @p text
 *         is @c nullptr.
 */
__dmi_api bool dmi_string_is_placeholder(const char *text);

__END_DECLS

#endif // !OPENDMI_UTILS_STRING_H
