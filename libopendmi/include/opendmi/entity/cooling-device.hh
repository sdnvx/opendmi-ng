//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_COOLING_DEVICE_HH
#define OPENDMI_ENTITY_COOLING_DEVICE_HH

#pragma once

#include <type_traits>

namespace dmi {
    namespace capi {
#       include <opendmi/entity/cooling-device.h>
    }

    using cooling_device_type_t = capi::dmi_cooling_device_type_t;

    enum class cooling_device_type : std::underlying_type_t<cooling_device_type_t> {
        unspecified              = capi::DMI_COOLING_DEVICE_TYPE_UNSPEC,                   ///< Unspecified
        other                    = capi::DMI_COOLING_DEVICE_TYPE_OTHER,                    ///< Other
        unknown                  = capi::DMI_COOLING_DEVICE_TYPE_UNKNOWN,                  ///< Unknown
        fan                      = capi::DMI_COOLING_DEVICE_TYPE_FAN,                      ///< Fan
        centrifugal_blower       = capi::DMI_COOLING_DEVICE_TYPE_CENTRIFUGAL_BLOWER,       ///< Centrifugal blower
        chip_fan                 = capi::DMI_COOLING_DEVICE_TYPE_CHIP_FAN,                 ///< Chip fan
        cabinet_fan              = capi::DMI_COOLING_DEVICE_TYPE_CABINET_FAN,              ///< Cabinet fan
        power_supply_fan         = capi::DMI_COOLING_DEVICE_TYPE_POWER_SUPPLY_FAN,         ///< Power supply fan
        heat_pipe                = capi::DMI_COOLING_DEVICE_TYPE_HEAT_PIPE,                ///< Heat pipe
        integrated_refrigeration = capi::DMI_COOLING_DEVICE_TYPE_INTEGRATED_REFRIGERATION, ///< Integrated refrigeration
        active_cooling           = capi::DMI_COOLING_DEVICE_TYPE_ACTIVE_COOLING,           ///< Active cooling
        passive_cooling          = capi::DMI_COOLING_DEVICE_TYPE_PASSIVE_COOLING,          ///< Passive cooling
    };
}

#endif // !OPENDMI_ENTITY_COOLING_DEVICE_HH
