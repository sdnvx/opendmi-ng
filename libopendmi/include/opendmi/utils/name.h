//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_NAME_H
#define OPENDMI_UTILS_NAME_H

#pragma once

#include <opendmi/types.h>

typedef struct dmi_name       dmi_name_t;
typedef struct dmi_name_range dmi_name_range_t;
typedef struct dmi_name_set   dmi_name_set_t;

struct dmi_name
{
    int id;
    const char *code;
    const char *name;
};

struct dmi_name_range
{
    int start_id;
    int end_id;
    const char *code;
    const char *name;
};

struct dmi_name_set
{
    /**
     * @brief Code name, used to identify the enumeration during serialization.
     */
    const char *code;

    /**
     * @brief Printable name, used to identify enumeration when printing.
     */
    const char *name;

    /**
     * @brief Short description.
     */
    const char **description;

    const dmi_name_t *names;
    const dmi_name_range_t *ranges;
};

#define DMI_NAME_NULL { -1, nullptr, nullptr }

#define DMI_NAME_UNSPEC(id)      { (id), "unspecified", "Unspecified" }
#define DMI_NAME_UNKNOWN(id)     { (id), "unknown",     "Unknown"     }
#define DMI_NAME_OTHER(id)       { (id), "other",       "Other"       }
#define DMI_NAME_NONE(id)        { (id), "none",        "None"        }
#define DMI_NAME_UNSUPPORTED(id) { (id), "unsupported", "Unsupported" }
#define DMI_NAME_RESERVED(id)    { (id), "reserved",    "Reserved"    }

#define DMI_NAME_RANGE_NULL { -1, -1, nullptr, nullptr }

__BEGIN_DECLS

/**
 * @brief Look up the machine-readable code string for an integer identifier.
 *
 * Searches @p dict first by exact entry match in `names`, then by range match
 * in `ranges`, and returns the `code` field of the first matching entry.
 *
 * @param dict Dictionary to search; must not be @c nullptr.
 * @param id   Integer identifier to look up; must be non-negative.
 * @return The code string for @p id, or @c nullptr if @p dict is @c nullptr,
 *         @p id is negative, or no matching entry is found.
 */
__dmi_api const char *dmi_code_lookup(const dmi_name_set_t *dict, int id);

/**
 * @brief Reverse look up the integer identifier for a machine-readable code
 *        string.
 *
 * Searches the `names` entries of @p dict for an entry whose `code` field
 * equals @p code and returns the corresponding integer identifier.
 *
 * @note Range entries are not searched.
 *
 * @param dict Dictionary to search; must not be @c nullptr.
 * @param code Code string to look up; must not be @c nullptr.
 * @return The integer identifier for @p code, or @c -1 if @p dict is @c nullptr,
 *         @p dict has no `names` entries, or no matching entry is found.
 */
__dmi_api int dmi_code_lookup_rev(const dmi_name_set_t *dict, const char *code);

/**
 * @brief Look up the human-readable name string for an integer identifier.
 *
 * Searches @p dict first by exact entry match in `names`, then by range match
 * in `ranges`, and returns the `name` field of the first matching entry.
 *
 * @param dict Dictionary to search; must not be @c nullptr.
 * @param id   Integer identifier to look up; must be non-negative.
 * @return The name string for @p id, or @c nullptr if @p dict is @c nullptr,
 *         @p id is negative, or no matching entry is found.
 */
__dmi_api const char *dmi_name_lookup(const dmi_name_set_t *dict, int id);

__END_DECLS

#endif // !OPENDMI_UTILS_NAME_H
