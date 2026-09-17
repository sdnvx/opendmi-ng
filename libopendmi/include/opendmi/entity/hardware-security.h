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

typedef struct dmi_hardware_security          dmi_hardware_security_t;
typedef union  dmi_hardware_security_settings dmi_hardware_security_settings_t;

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
 * @brief Hardware security settings.
 */
dmi_packed_union(dmi_hardware_security_settings)
{
    dmi_byte_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief front panel reset status.
         */
        dmi_byte_t front_panel_reset : 2;

        /**
         * @brief Administrator password status.
         */
        dmi_byte_t admin_password : 2;

        /**
         * @brief Keyboard password status.
         */
        dmi_byte_t keyboard_password : 2;

        /**
         * @brief Power-on password status.
         */
        dmi_byte_t poweron_password : 2;
    };
};

dmi_static_assert_value_union(dmi_hardware_security_settings);

struct dmi_hardware_security
{
        /**
         * @brief front panel reset status.
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
extern const dmi_entity_spec_t dmi_hardware_security_spec;

__BEGIN_DECLS

const char *dmi_hardware_security_status_name(dmi_hardware_security_status_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_HARDWARE_SECURITY_H
