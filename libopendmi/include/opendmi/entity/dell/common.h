//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_COMMON_H
#define OPENDMI_ENTITY_DELL_COMMON_H

#pragma once

#include <opendmi/utils/name.h>

/**
 * @brief Dell enable states.
 */
typedef enum dmi_dell_enable_state
{
    DMI_DELL_ENABLE_STATE_UNSPEC   = 0x00, ///< Unspecified
    DMI_DELL_ENABLE_STATE_OTHER    = 0x01, ///< Other
    DMI_DELL_ENABLE_STATE_UNKNOWN  = 0x02, ///< Unknown
    DMI_DELL_ENABLE_STATE_ENABLED  = 0x03, ///< Enabled
    DMI_DELL_ENABLE_STATE_DISABLED = 0x04, ///< Disabled
    __DMI_DELL_ENABLE_STATE_COUNT
} dmi_dell_enable_state_t;

/**
 * @brief Dell port security settings.
 */
typedef enum dmi_dell_port_security
{
    DMI_DELL_PORT_SECURITY_UNSPEC            = 0x00, ///< Unspecified
    DMI_DELL_PORT_SECURITY_OTHER             = 0x01, ///< Other
    DMI_DELL_PORT_SECURITY_UNKNOWN           = 0x02, ///< Unknown
    DMI_DELL_PORT_SECURITY_NONE              = 0x03, ///< None
    DMI_DELL_PORT_SECURITY_EXT_IFACE_LOCKED  = 0x04, ///< External interface locked out
    DMI_DELL_PORT_SECURITY_EXT_IFACE_ENABLED = 0x05, ///< External interface enabled
    DMI_DELL_PORT_SECURITY_BOOT_BYPASS       = 0x06, ///< Boot-bypass
} dmi_dell_port_security_t;

/**
 * @brief Dell CMOS checksum types.
 */
typedef enum dmi_dell_check_type
{
    DMI_DELL_CHECK_TYPE_WORD_CHECKSUM   = 0x00, ///< Running sum in a word
    DMI_DELL_CHECK_TYPE_BYTE_CHECKSUM   = 0x01, ///< Running sum in a byte
    DMI_DELL_CHECK_TYPE_WORD_CRC        = 0x02, ///< CRC in a word
    DMI_DELL_CHECK_TYPE_WORD_CHECKSUM_N = 0x03  ///< Negated running sum in a word
} dmi_dell_check_type_t;

/**
 * @brief Dell protected value formats.
 */
typedef enum dmi_dell_value_format
{
    DMI_DELL_VALUE_FORMAT_SCAN_CODE    = 0x00, ///< Alphanumeric, scan codes
    DMI_DELL_VALUE_FORMAT_ASCII        = 0x01, ///< Alphanumeric, ASCII
    DMI_DELL_VALUE_FORMAT_SCAN_CODE_NS = 0x02, ///< Alphanumeric, scan codes (NS)
    DMI_DELL_VALUE_FORMAT_ASCII_NS     = 0x03  ///< Alphanumeric, ASCII (NS)
} dmi_dell_value_format_t;

extern __dmi_api const dmi_name_set_t dmi_dell_enable_state_names;
extern __dmi_api const dmi_name_set_t dmi_dell_port_security_names;
extern __dmi_api const dmi_name_set_t dmi_dell_check_type_names;
extern __dmi_api const dmi_name_set_t dmi_dell_value_format_names;

__BEGIN_DECLS

__dmi_api const char *dmi_dell_enable_state_name(dmi_dell_enable_state_t value);
__dmi_api const char *dmi_dell_port_security_name(dmi_dell_port_security_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_DELL_COMMON_H
