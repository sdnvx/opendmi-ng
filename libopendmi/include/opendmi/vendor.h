//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_VENDOR_H
#define OPENDMI_VENDOR_H

#pragma once

#include <opendmi/types.h>

#ifndef DMI_PLATFORM_T
#   define DMI_PLATFORM_T
    typedef struct dmi_platform dmi_platform_t;
#endif // !DMI_PLATFORM_T

typedef struct dmi_vendor_spec dmi_vendor_spec_t;

/**
 * @brief SMBIOS vendor identifiers.
 */
typedef enum dmi_vendor
{
    DMI_VENDOR_INVALID = -1, ///< Invalid
    DMI_VENDOR_ANY     = 0,  ///< Any vendor, in platform conditions only
    DMI_VENDOR_OTHER,        ///< Other
    DMI_VENDOR_ACER,         ///< Acer
    DMI_VENDOR_AMD,          ///< AMD
    DMI_VENDOR_AMI,          ///< AMI
    DMI_VENDOR_APPLE,        ///< Apple
    DMI_VENDOR_CISCO,        ///< Cisco
    DMI_VENDOR_DELL,         ///< Dell
    DMI_VENDOR_HONOR,        ///< Honor
    DMI_VENDOR_HP,           ///< HP
    DMI_VENDOR_HPE,          ///< HPE
    DMI_VENDOR_HUAWEI,       ///< Huawei
    DMI_VENDOR_IBM,          ///< IBM
    DMI_VENDOR_INTEL,        ///< Intel
    DMI_VENDOR_LENOVO,       ///< Lenovo
    DMI_VENDOR_UNISYS,       ///< Unisys
    __DMI_VENDOR_COUNT
} dmi_vendor_t;

/**
 * @brief Handler which tells the family and the generation of a platform.
 *
 * Called as the context is opened for the vendor of the firmware, with the
 * vendors and the product name of the platform set. The handler sets the
 * family and the generation, if the product name tells them, and leaves them
 * unset otherwise.
 *
 * @return `false` if memory is exhausted, `true` otherwise.
 */
typedef bool dmi_vendor_detect_fn(dmi_platform_t *platform);

/**
 * @brief SMBIOS vendor specification.
 */
struct dmi_vendor_spec
{
    /**
     * @brief Vendor identifier.
     */
    dmi_vendor_t id;

    /**
     * @brief Vendor code.
     */
    const char *code;

    /**
     * @brief Vendor names list, terminated by @c nullptr.
     */
    const char **names;

    /**
     * @brief Tells the family and the generation of a platform of the vendor,
     * @c nullptr if the vendor has no families.
     */
    dmi_vendor_detect_fn *detect;
};

#define DMI_VENDOR_NULL { .id = DMI_VENDOR_INVALID, .names = nullptr }

__BEGIN_DECLS

/**
 * @brief Get the name of a vendor.
 *
 * The name is translated to the current locale, if the locale has a
 * translation of it.
 *
 * @param[in] vendor Vendor identifier.
 *
 * @return Name of the vendor, e.g. `HPE`, or `nullptr` if @p vendor has no
 *         name, e.g. `DMI_VENDOR_ANY` or `DMI_VENDOR_INVALID`.
 */
__dmi_api const char *dmi_vendor_name(dmi_vendor_t vendor);

/**
 * @brief Tell the vendor from a vendor name.
 *
 * The name is compared with the names of the known vendors, case and
 * surrounding whitespace ignored.
 *
 * @param[in] name Vendor name, as an SMBIOS structure gives it.
 *
 * @return Static specification of the vendor, or `nullptr` if @p name is
 *         `nullptr` or matches no vendor.
 */
__dmi_api const dmi_vendor_spec_t *dmi_vendor_detect(const char *name);

__END_DECLS

#endif // !OPENDMI_VENDOR_H
