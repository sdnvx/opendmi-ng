//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SYSTEM_RESET_HH
#define OPENDMI_ENTITY_SYSTEM_RESET_HH

#pragma once

#include <type_traits>
#include <optional>

namespace dmi {
    namespace capi {
#       include <opendmi/entity/system-reset.h>
    }

    using boot_option_t = capi::dmi_boot_option_t;

    enum class boot_option : std::underlying_type_t<boot_option_t>
    {
        reserved         = capi::DMI_BOOT_OPTION_RESERVED,         ///< Reserved, do not use
        operating_system = capi::DMI_BOOT_OPTION_OPERATING_SYSTEM, ///< Operating system
        system_utils     = capi::DMI_BOOT_OPTION_SYSTEM_UTILS,     ///< System utilities
        avoid_reboot     = capi::DMI_BOOT_OPTION_AVOID_REBOOT,     ///< Do not reboot
    };

    class system_reset
    {
    public:
        const bool& is_enabled;
        const boot_option& boot_on_watchdog;
        const boot_option& boot_on_limit;
        const bool& has_watchdog;
        const std::optional<unsigned short>& reset_count;
        const std::optional<unsigned short>& reset_limit;
        const std::optional<unsigned short>& timer_interval;
        const std::optional<unsigned short>& timeout;

        system_reset(const capi::dmi_system_reset_t *info)
            : is_enabled(m_is_enabled)
            , boot_on_watchdog(m_boot_on_watchdog)
            , boot_on_limit(m_boot_on_limit)
            , has_watchdog(m_has_watchdog)
            , reset_count(m_reset_count)
            , reset_limit(m_reset_limit)
            , timer_interval(m_timer_interval)
            , timeout(m_timeout)
        {
            m_is_enabled       = info->is_enabled;
            m_boot_on_watchdog = static_cast<boot_option>(info->boot_on_watchdog);
            m_boot_on_limit    = static_cast<boot_option>(info->boot_on_limit);
            m_has_watchdog     = info->has_watchdog;

            if (info->reset_count != USHRT_MAX)
                m_reset_count = info->reset_count;
            if (info->reset_limit != USHRT_MAX)
                m_reset_limit = info->reset_limit;
            if (info->timer_interval != USHRT_MAX)
                m_timer_interval = info->timer_interval;
            if (info->timeout != USHRT_MAX)
                m_timeout = info->timeout;
        }

    private:
        bool m_is_enabled;
        boot_option m_boot_on_watchdog;
        boot_option m_boot_on_limit;
        bool m_has_watchdog;
        std::optional<unsigned short> m_reset_count;
        std::optional<unsigned short> m_reset_limit;
        std::optional<unsigned short> m_timer_interval;
        std::optional<unsigned short> m_timeout;
    };
}

#endif // !OPENDMI_ENTITY_SYSTEM_RESET_HH
