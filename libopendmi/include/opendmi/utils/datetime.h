//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_DATETIME_H
#define OPENDMI_UTILS_DATETIME_H

#pragma once

#include <opendmi/types.h>

/**
 * @brief Date value.
 */
typedef uint32_t dmi_date_t;

#define DMI_DATE(year, month, day) \
    ((dmi_date_t)((((year) & 0xFFFFu) << 16) | (((month) & 0xFFu) << 8) | ((day) & 0xFFu)))

#define DMI_DATE_NONE DMI_DATE(0, 0, 0)

/**
 * @brief Constructs a `dmi_date_t` value from individual date components.
 *
 * @param[in] year   Full calendar year (e.g., 2025).
 * @param[in] month  Month of the year (1–12).
 * @param[in] day    Day of the month (1–31).
 *
 * @return A `dmi_date_t` encoding the given date components.
 */
static inline dmi_date_t dmi_date(unsigned int year,
                                  unsigned int month,
                                  unsigned int day)
{
    return DMI_DATE(year, month, day);
}

/**
 * @brief Extracts the year component from a `dmi_date_t` value.
 *
 * @param[in] date  Date value to extract from.
 *
 * @return The year component as an unsigned integer.
 */
static inline unsigned int dmi_date_year(dmi_date_t date)
{
    return (date & 0xFFFF0000U) >> 16;
}

/**
 * @brief Extracts the month component from a `dmi_date_t` value.
 *
 * @param[in] date  Date value to extract from.
 *
 * @return The month component as an unsigned integer (1–12).
 */
static inline unsigned int dmi_date_month(dmi_date_t date)
{
    return (date & 0x00FF00U) >> 8;
}

/**
 * @brief Extracts the day component from a `dmi_date_t` value.
 *
 * @param[in] date  Date value to extract from.
 *
 * @return The day component as an unsigned integer (1–31).
 */
static inline unsigned int dmi_date_day(dmi_date_t date)
{
    return date & 0x0000FFU;
}

__BEGIN_DECLS

/**
 * @brief Parses a date string in SMBIOS format into a `dmi_date_t` value.
 *
 * Expects a date string in the format `MM/DD/YY` or `MM/DD/YYYY`, as used
 * in SMBIOS date fields. A two-digit year is interpreted as an offset from
 * 1900 (e.g., `"99"` → 1999). The month must be in the range 1–12 and the
 * day in the range 1–31; no calendar validation beyond those ranges is
 * performed.
 *
 * Returns `DMI_DATE_NONE` if the string is malformed, contains fewer or more
 * than three slash-separated tokens, a token contains non-digit characters or
 * a leading sign, a numeric value is out of the expected range, or the year
 * is neither two nor four digits long.
 *
 * @param[in] str  Null-terminated date string to parse. Must not be @c nullptr.
 *
 * @return A `dmi_date_t` encoding the parsed date, or `DMI_DATE_NONE` if the
 *         string could not be parsed.
 */
__dmi_api dmi_date_t dmi_date_parse(const char *str);

/**
 * @brief Formats a `dmi_date_t` value as an ISO 8601 date string.
 *
 * Allocates and returns a null-terminated string of the form `"YYYY-MM-DD"`,
 * with the year zero-padded to four digits and the month and day zero-padded
 * to two digits. The caller is responsible for freeing the returned buffer.
 *
 * @param[in] date  Date value to format.
 *
 * @return A newly allocated `"YYYY-MM-DD"` string on success,
 *         or @c nullptr on allocation failure.
 */
__dmi_api char *dmi_date_format(dmi_date_t date);

__END_DECLS

#endif // !OPENDMI_UTILS_DATETIME_H
