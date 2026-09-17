//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_VENDOR_H
#define OPENDMI_VENDOR_H

#pragma once

#include <opendmi/module.h>

typedef struct dmi_vendor_spec dmi_vendor_spec_t;

/**
 * @brief SMBIOS vendor identifiers.
 */
typedef enum dmi_vendor
{
    DMI_VENDOR_INVALID = -1, ///< Invalid
    DMI_VENDOR_OTHER,        ///< Other
    DMI_VENDOR_ACER,         ///< Acer
    DMI_VENDOR_AMI,          ///< AMI
    DMI_VENDOR_APPLE,        ///< Apple
    DMI_VENDOR_DELL,         ///< Dell
    DMI_VENDOR_HP,           ///< HP
    DMI_VENDOR_HPE,          ///< HPE
    DMI_VENDOR_IBM,          ///< IBM
    DMI_VENDOR_INTEL,        ///< Intel
    DMI_VENDOR_LENOVO,       ///< Lenovo
    __DMI_VENDOR_COUNT
} dmi_vendor_t;

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
};

#define DMI_VENDOR_NULL { .id = DMI_VENDOR_INVALID, .names = nullptr }

__BEGIN_DECLS

__dmi_api const char *dmi_vendor_name(dmi_vendor_t vendor);

__dmi_api const dmi_vendor_spec_t *dmi_vendor_detect(const char *name);

__END_DECLS

#endif // !OPENDMI_VENDOR_H
