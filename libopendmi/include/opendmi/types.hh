//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_TYPES_HH
#define OPENDMI_TYPES_HH

#pragma once

#include <type_traits>

namespace dmi {
    namespace capi {
#       include <opendmi/types.h>
    }

    enum class type : std::underlying_type_t<capi::dmi_type_t> {
        invalid                 = capi::DMI_TYPE_INVALID,                 ///< Invalid
        firmware                = capi::DMI_TYPE_FIRMWARE,                ///< Platform firmware information
        system                  = capi::DMI_TYPE_SYSTEM,                  ///< System information
        baseboard               = capi::DMI_TYPE_BASEBOARD,               ///< Baseboard or module information
        chassis                 = capi::DMI_TYPE_CHASSIS,                 ///< System enclosure or chassis
        processor               = capi::DMI_TYPE_PROCESSOR,               ///< Processor information
        memory_controller       = capi::DMI_TYPE_MEMORY_CONTROLLER,       ///< Memory controller information (obsolette)
        memory_module           = capi::DMI_TYPE_MEMORY_MODULE,           ///< Memory module information (obsolette)
        cache                   = capi::DMI_TYPE_CACHE,                   ///< Cache information
        connector               = capi::DMI_TYPE_PORT_CONNECTOR,          ///< Port connector information
        slots                   = capi::DMI_TYPE_SYSTEM_SLOTS,            ///< System slots
        onboard_device          = capi::DMI_TYPE_ONBOARD_DEVICE,          ///< Onboard devices information
        oem_strings             = capi::DMI_TYPE_OEM_STRINGS,             ///< OEM strings
        config_options          = capi::DMI_TYPE_SYSTEM_CONFIG_OPTIONS,   ///< System configuration options
        firmware_language       = capi::DMI_TYPE_FIRMWARE_LANGUAGE,       ///< Firmware language information
        group_assoc             = capi::DMI_TYPE_GROUP_ASSOC,             ///< Group associations
        system_event_log        = capi::DMI_TYPE_SYSTEM_EVENT_LOG,        ///< System event log
        memory_array            = capi::DMI_TYPE_MEMORY_ARRAY,            ///< Physical memory array
        memory_device           = capi::DMI_TYPE_MEMORY_DEVICE,           ///< Memory device
        memory_error_32         = capi::DMI_TYPE_MEMORY_ERROR_32,         ///< 32-bit memory error information
        memory_array_addr       = capi::DMI_TYPE_MEMORY_ARRAY_ADDR,       ///< Memory array mapped address
        memory_device_addr      = capi::DMI_TYPE_MEMORY_DEVICE_ADDR,      ///< Memory device mapped address
        pointing_device         = capi::DMI_TYPE_POINTING_DEVICE,         ///< Built-in pointing device
        portable_battery        = capi::DMI_TYPE_PORTABLE_BATTERY,        ///< Portable battery
        system_reset            = capi::DMI_TYPE_SYSTEM_RESET,            ///< System reset
        hardware_security       = capi::DMI_TYPE_HARDWARE_SECURITY,       ///< Hardware security
        power_controls          = capi::DMI_TYPE_POWER_CONTROLS,          ///< System power controls
        voltage_probe           = capi::DMI_TYPE_VOLTAGE_PROBE,           ///< Voltage probe
        cooling_device          = capi::DMI_TYPE_COOLING_DEVICE,          ///< Cooling device
        temperature_probe       = capi::DMI_TYPE_TEMPERATURE_PROBE,       ///< Temperature probe
        current_probe           = capi::DMI_TYPE_CURRENT_PROBE,           ///< Electrical current probe
        oob_remote_access       = capi::DMI_TYPE_OOB_REMOTE_ACCESS,       ///< Out-of-band remote access
        bis_entry_point         = capi::DMI_TYPE_BIS_ENTRY_POINT,         ///< Boot Integrity Services (BIS) entry point
        system_boot             = capi::DMI_TYPE_SYSTEM_BOOT,             ///< System boot information
        memory_error_64         = capi::DMI_TYPE_MEMORY_ERROR_64,         ///< 64-bit memory error information
        mgmt_device             = capi::DMI_TYPE_MGMT_DEVICE,             ///< Management device
        mgmt_device_component   = capi::DMI_TYPE_MGMT_DEVICE_COMPONENT,   ///< Management device component
        mgmt_device_threshold   = capi::DMI_TYPE_MGMT_DEVICE_THRESHOLD,   ///< Management device threshold data
        memory_channel          = capi::DMI_TYPE_MEMORY_CHANNEL,          ///< Memory channel
        ipmi_device             = capi::DMI_TYPE_IPMI_DEVICE,             ///< IPMI device information
        power_supply            = capi::DMI_TYPE_POWER_SUPPLY,            ///< System power supply
        additional_info         = capi::DMI_TYPE_ADDITIONAL_INFO,         ///< Additional information
        onboard_device_ex       = capi::DMI_TYPE_ONBOARD_DEVICE_EX,       ///< Onboard devices extended information
        mgmt_controller_host_if = capi::DMI_TYPE_MGMT_CONTROLLER_HOST_IF, ///< Management controller host interface
        tpm_device              = capi::DMI_TYPE_TPM_DEVICE,              ///< TPM device
        processor_ex            = capi::DMI_TYPE_PROCESSOR_EX,            ///< Processor additional information
        firmware_inventory      = capi::DMI_TYPE_FIRMWARE_INVENTORY,      ///< Firmware inventory information
        string_property         = capi::DMI_TYPE_STRING_PROPERTY,         ///< String property
        inactive                = capi::DMI_TYPE_INACTIVE,                ///< Inactive
        end_of_table            = capi::DMI_TYPE_END_OF_TABLE,            ///< End of table
    };
}

#endif // !OPENDMI_TYPES_HH
