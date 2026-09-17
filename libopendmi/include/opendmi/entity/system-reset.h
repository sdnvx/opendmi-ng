//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SYSTEM_RESET_H
#define OPENDMI_ENTITY_SYSTEM_RESET_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_system_reset dmi_system_reset_t;
typedef union dmi_system_reset_caps dmi_system_reset_caps_t;

/**
 * @brief System boot options.
 */
typedef enum dmi_boot_option
{
    DMI_BOOT_OPTION_RESERVED         = 0x00, ///< Reserved, do not use
    DMI_BOOT_OPTION_OPERATING_SYSTEM = 0x01, ///< Operating system
    DMI_BOOT_OPTION_SYSTEM_UTILS     = 0x02, ///< System utilities
    DMI_BOOT_OPTION_AVOID_REBOOT     = 0x03, ///< Do not reboot
} dmi_boot_option_t;

/**
 * @brief System reset capabilities.
 */
dmi_packed_union(dmi_system_reset_caps)
{
    /**
     * @brief Raw value;
     */
    dmi_byte_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Status. Identifies whether (`true`) or not (`false`) the
         * system reset is enabled by the user.
         */
        dmi_byte_t is_enabled : 1;

        /**
         * @brief Boot option. Indicates one of the following actions to be
         * taken after a watchdog reset.
         */
        dmi_byte_t boot_on_watchdog : 2;

        /**
         * @brief Boot option on limit. Identifies one of the following system
         * actions to be taken when the reset limit is reached.
         */
        dmi_byte_t boot_on_limit : 2;

        /**
         * @brief System contains a watchdog timer: either `true` or `false`.
         */
        dmi_byte_t has_watchdog : 1;

        /**
         * @brief Reserved for future assignment by this specification, set to
         * zero.
         */
        dmi_byte_t __reserved : 2;
    };
};

dmi_static_assert_value_union(dmi_system_reset_caps);

/**
 * @brief System reset information (type 23).
 */
struct dmi_system_reset
{
    /**
     * @brief Status. Identifies whether (`true`) or not (`false`) the
     * system reset is enabled by the user.
     */
    bool is_enabled;

    /**
     * @brief Boot option. Indicates one of the following actions to be
     * taken after a watchdog reset.
     */
    dmi_boot_option_t boot_on_watchdog;

    /**
     * @brief Boot option on limit. Identifies one of the following system
     * actions to be taken when the reset limit is reached.
     */
    dmi_boot_option_t boot_on_limit;

    /**
     * @brief System contains a watchdog timer: either `true` or `false`.
     */
    bool has_watchdog;

    /**
     * @brief Number of automatic system resets since the last intentional
     * reset. A value of `USHRT_MAX` indicates unknown.
     */
    unsigned short reset_count;

    /**
     * @brief Number of consecutive times the system reset is attempted. A
     * value of `USHRT_MAX` indicates unknown.
     */
    unsigned short reset_limit;

    /**
     * @brief Number of minutes to use for the watchdog timer. If the timer is
     * not reset within this interval, the system reset timeout begins. A value
     * of `USHRT_MAX` indicates unknown.
     */
    unsigned short timer_interval;

    /**
     * @brief Number of minutes before the reboot is initiated. It is used
     * after a system power cycle, system reset (local or remote), and
     * automatic system reset. A value of `USHRT_MAX` indicates unknown.
     */
    unsigned short timeout;
};

/**
 * @brief System reset entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_system_reset_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_boot_option_name(dmi_boot_option_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_SYSTEM_RESET_H
