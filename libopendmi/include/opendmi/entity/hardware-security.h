//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HARDWARE_SECURITY_H
#define OPENDMI_ENTITY_HARDWARE_SECURITY_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_HARDWARE_SECURITY_T
#   define DMI_HARDWARE_SECURITY_T
    typedef struct dmi_hardware_security dmi_hardware_security_t;
#endif // !DMI_HARDWARE_SECURITY_T

/**
 * @brief Hardware security status values.
 */
typedef enum dmi_hardware_security_status
{
    DMI_HARDWARE_SECURITY_STATUS_DISABLED        = 0x00, ///< Disabled
    DMI_HARDWARE_SECURITY_STATUS_ENABLED         = 0x01, ///< Enabled
    DMI_HARDWARE_SECURITY_STATUS_NOT_IMPLEMENTED = 0x02, ///< Not implemented
    DMI_HARDWARE_SECURITY_STATUS_UNKNOWN         = 0x03, ///< Unknown
} dmi_hardware_security_status_t;

/**
 * @brief Hardware security structure (type 24).
 * @since SMBIOS 2.2
 */
struct dmi_hardware_security
{
        /**
         * @brief Front panel reset status.
         */
        dmi_hardware_security_status_t front_panel_reset;

        /**
         * @brief Administrator password status.
         */
        dmi_hardware_security_status_t admin_password;

        /**
         * @brief Keyboard password status.
         */
        dmi_hardware_security_status_t keyboard_password;

        /**
         * @brief Power-on password status.
         */
        dmi_hardware_security_status_t poweron_password;
};

/**
 * @brief Hardware security entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hardware_security_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_hardware_security_status_name(dmi_hardware_security_status_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_HARDWARE_SECURITY_H
