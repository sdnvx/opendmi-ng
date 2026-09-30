//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_COMMON_H
#define OPENDMI_ENTITY_HPE_COMMON_H

#pragma once

#include <opendmi/entity.h>

/**
 * @brief Value of a flag the structure may leave undefined.
 */
typedef enum dmi_hpe_flag
{
    DMI_HPE_FLAG_NO     = 0x00, ///< No
    DMI_HPE_FLAG_YES    = 0x01, ///< Yes
    DMI_HPE_FLAG_UNSPEC = 0xFF  ///< Not defined
} dmi_hpe_flag_t;

/**
 * @brief Encryption status of a memory module or a drive.
 */
typedef enum dmi_hpe_encryption
{
    DMI_HPE_ENCRYPTION_NONE        = 0x00, ///< Not encrypted
    DMI_HPE_ENCRYPTION_ENCRYPTED   = 0x01, ///< Encrypted
    DMI_HPE_ENCRYPTION_UNKNOWN     = 0x02, ///< Unknown
    DMI_HPE_ENCRYPTION_UNSUPPORTED = 0x03  ///< Not supported
} dmi_hpe_encryption_t;

__BEGIN_DECLS

__dmi_api const char *dmi_hpe_flag_name(dmi_hpe_flag_t value);
__dmi_api const char *dmi_hpe_encryption_name(dmi_hpe_encryption_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_HPE_COMMON_H
