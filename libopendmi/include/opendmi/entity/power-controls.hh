//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_POWER_CONTROLS_HH
#define OPENDMI_ENTITY_POWER_CONTROLS_HH

#pragma once

#include <optional>

namespace dmi {
    namespace capi {
#       include <opendmi/entity/power-controls.h>
    }

    class power_controls
    {
    public:
        const std::optional<short>& poweron_month;
        const std::optional<short>& poweron_day;
        const std::optional<short>& poweron_hour;
        const std::optional<short>& poweron_minute;
        const std::optional<short>& poweron_second;

        power_controls(const capi::dmi_power_controls_t *info)
            : poweron_month(m_poweron_month)
            , poweron_day(m_poweron_day)
            , poweron_hour(m_poweron_hour)
            , poweron_minute(m_poweron_minute)
            , poweron_second(m_poweron_second)
        {
            if (info->poweron_month != USHRT_MAX)
                m_poweron_month = info->poweron_month;
            if (info->poweron_day != USHRT_MAX)
                m_poweron_day = info->poweron_day;
            if (info->poweron_hour != USHRT_MAX)
                m_poweron_hour = info->poweron_hour;
            if (info->poweron_minute != USHRT_MAX)
                m_poweron_minute = info->poweron_minute;
            if (info->poweron_second != USHRT_MAX)
                m_poweron_second = info->poweron_second;
        }

    private:
        std::optional<short> m_poweron_month;
        std::optional<short> m_poweron_day;
        std::optional<short> m_poweron_hour;
        std::optional<short> m_poweron_minute;
        std::optional<short> m_poweron_second;
    };
}

#endif // !OPENDMI_ENTITY_POWER_CONTROLS_HH
