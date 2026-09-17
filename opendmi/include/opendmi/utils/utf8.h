//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_UTF8_H
#define OPENDMI_UTILS_UTF8_H

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <opendmi/defs.h>
#include <opendmi/types.h>

/**
 * @brief Character filter function.
 *
 * @param[in] code Unicode code point.
 *
 * @return `true` if the character is allowed, `false` otherwise.
 */
typedef bool dmi_utf8_filter_fn(uint32_t code);

__BEGIN_DECLS

/**
 * @brief Check if string is valid UTF-8, containing only allowed characters.
 *
 * Overlong encodings, surrogates and code points beyond U+10FFFF are not
 * valid.
 *
 * @param[in] str    NUL-terminated string.
 * @param[in] filter Character filter, or @c nullptr to allow all characters.
 *
 * @return `true` if @p str is valid UTF-8 and all its characters are allowed
 *         by @p filter, `false` otherwise.
 */
bool dmi_utf8_is_valid_ex(const char *str, dmi_utf8_filter_fn *filter);

/**
 * @brief Make valid UTF-8 copy of a string, containing only allowed
 * characters.
 *
 * Every invalid byte and every character rejected by @p filter is replaced
 * with U+FFFD replacement character.
 *
 * @param[in] context DMI context handle.
 * @param[in] str     NUL-terminated string.
 * @param[in] filter  Character filter, or @c nullptr to allow all characters.
 *                    Replacement character must be allowed by the filter.
 *
 * @return Newly allocated string, which should be freed with `dmi_free()`,
 *         or @c nullptr if out of memory.
 */
char *dmi_utf8_repair_ex(dmi_context_t *context, const char *str, dmi_utf8_filter_fn *filter);

__END_DECLS

/**
 * @brief Check if string is valid UTF-8.
 *
 * Equivalent to `dmi_utf8_is_valid_ex()` without character filter.
 */
static inline bool dmi_utf8_is_valid(const char *str)
{
    return dmi_utf8_is_valid_ex(str, nullptr);
}

/**
 * @brief Make valid UTF-8 copy of a string.
 *
 * Equivalent to `dmi_utf8_repair_ex()` without character filter.
 */
static inline char *dmi_utf8_repair(dmi_context_t *context, const char *str)
{
    return dmi_utf8_repair_ex(context, str, nullptr);
}

#endif // !OPENDMI_UTILS_UTF8_H
