//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_VERSION_H
#define OPENDMI_UTILS_VERSION_H

#pragma once

#include <opendmi/types.h>

/**
 * @brief SMBIOS version number.
 */
typedef uint32_t dmi_version_t;

typedef enum dmi_version_level
{
    DMI_VERSION_LEVEL_MAJOR,
    DMI_VERSION_LEVEL_MINOR,
    DMI_VERSION_LEVEL_REVISION
} dmi_version_level_t;

#define DMI_VERSION(major, minor, revision) \
    ((dmi_version_t)((((major) & 0xFFU) << 16) | (((minor) & 0xFFU) << 8) | ((revision) & 0xFFU)))

#define DMI_VERSION_NONE DMI_VERSION(0, 0, 0)

/**
 * @brief Construct SMBIOS version number from its components.
 *
 * @param[in] major Major version number.
 * @param[in] minor Minor version number.
 * @param[in] revision Revision version number.
 *
 * @return Version number.
 */
static inline dmi_version_t dmi_version(unsigned int major,
                                        unsigned int minor,
                                        unsigned int revision)
{
    return DMI_VERSION(major, minor, revision);
}

/**
 * @brief Get major component of SMBIOS version number.
 *
 * @param[in] version Version number.
 * @return Major version number.
 */
__dmi_const
static inline unsigned int dmi_version_major(dmi_version_t version)
{
    return (version & 0xFF0000U) >> 16;
}

/**
 * @brief Get minor component of SMBIOS version number.
 *
 * @param[in] version Version number.
 * @return Minor version number.
 */
__dmi_const
static inline unsigned int dmi_version_minor(dmi_version_t version)
{
    return (version & 0x00FF00U) >> 8;
}

/**
 * @brief Get revision component of SMBIOS version number.
 *
 * @param[in] version Version number.
 * @return Revision version number.
 */
__dmi_const
static inline unsigned int dmi_version_revision(dmi_version_t version)
{
    return version & 0x0000FFU;
}

__BEGIN_DECLS

/**
 * @brief Format SMBIOS version as a string.
 *
 * Returns a dynamically allocated string representing the version number.
 * The revision component is omitted when it is zero (e.g. "3.4"), otherwise
 * the full "major.minor.revision" form is used. The caller is responsible for
 * freeing the returned string.
 *
 * @param[in] version Version number.
 *
 * @return A newly allocated version string, or @c nullptr on allocation failure.
 */
__dmi_api char *dmi_version_format(dmi_version_t version);

/**
 * @brief Format SMBIOS version as a string with explicit detail level.
 *
 * Returns a dynamically allocated string representing the version number
 * formatted to the specified detail level:
 * - @ref DMI_VERSION_LEVEL_MAJOR — "major"
 * - @ref DMI_VERSION_LEVEL_MINOR — "major.minor"
 * - @ref DMI_VERSION_LEVEL_REVISION — "major.minor.revision"
 *
 * The caller is responsible for freeing the returned string.
 *
 * @param[in] version Version number.
 * @param[in] level   Detail level to include in the formatted string.
 *
 * @return A newly allocated version string, or @c nullptr if @p level is invalid
 *         or on allocation failure.
 */
__dmi_api char *dmi_version_format_ex(dmi_version_t version, dmi_version_level_t level);

__END_DECLS

#endif // !OPENDMI_UTILS_VERSION_H
