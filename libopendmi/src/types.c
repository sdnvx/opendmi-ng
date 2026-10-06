//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/types.h>

// Structure types of the SMBIOS specification
const dmi_type_t dmi_type_firmware                = { .id = DMI_TYPE_ID_FIRMWARE                };
const dmi_type_t dmi_type_system                  = { .id = DMI_TYPE_ID_SYSTEM                  };
const dmi_type_t dmi_type_baseboard               = { .id = DMI_TYPE_ID_BASEBOARD               };
const dmi_type_t dmi_type_chassis                 = { .id = DMI_TYPE_ID_CHASSIS                 };
const dmi_type_t dmi_type_processor               = { .id = DMI_TYPE_ID_PROCESSOR               };
const dmi_type_t dmi_type_memory_controller       = { .id = DMI_TYPE_ID_MEMORY_CONTROLLER       };
const dmi_type_t dmi_type_memory_module           = { .id = DMI_TYPE_ID_MEMORY_MODULE           };
const dmi_type_t dmi_type_cache                   = { .id = DMI_TYPE_ID_CACHE                   };
const dmi_type_t dmi_type_port_connector          = { .id = DMI_TYPE_ID_PORT_CONNECTOR          };
const dmi_type_t dmi_type_system_slots            = { .id = DMI_TYPE_ID_SYSTEM_SLOTS            };
const dmi_type_t dmi_type_onboard_device          = { .id = DMI_TYPE_ID_ONBOARD_DEVICE          };
const dmi_type_t dmi_type_oem_strings             = { .id = DMI_TYPE_ID_OEM_STRINGS             };
const dmi_type_t dmi_type_system_config_options   = { .id = DMI_TYPE_ID_SYSTEM_CONFIG_OPTIONS   };
const dmi_type_t dmi_type_firmware_language       = { .id = DMI_TYPE_ID_FIRMWARE_LANGUAGE       };
const dmi_type_t dmi_type_group_assoc             = { .id = DMI_TYPE_ID_GROUP_ASSOC             };
const dmi_type_t dmi_type_system_event_log        = { .id = DMI_TYPE_ID_SYSTEM_EVENT_LOG        };
const dmi_type_t dmi_type_memory_array            = { .id = DMI_TYPE_ID_MEMORY_ARRAY            };
const dmi_type_t dmi_type_memory_device           = { .id = DMI_TYPE_ID_MEMORY_DEVICE           };
const dmi_type_t dmi_type_memory_error_32         = { .id = DMI_TYPE_ID_MEMORY_ERROR_32         };
const dmi_type_t dmi_type_memory_array_addr       = { .id = DMI_TYPE_ID_MEMORY_ARRAY_ADDR       };
const dmi_type_t dmi_type_memory_device_addr      = { .id = DMI_TYPE_ID_MEMORY_DEVICE_ADDR      };
const dmi_type_t dmi_type_pointing_device         = { .id = DMI_TYPE_ID_POINTING_DEVICE         };
const dmi_type_t dmi_type_portable_battery        = { .id = DMI_TYPE_ID_PORTABLE_BATTERY        };
const dmi_type_t dmi_type_system_reset            = { .id = DMI_TYPE_ID_SYSTEM_RESET            };
const dmi_type_t dmi_type_hardware_security       = { .id = DMI_TYPE_ID_HARDWARE_SECURITY       };
const dmi_type_t dmi_type_power_controls          = { .id = DMI_TYPE_ID_POWER_CONTROLS          };
const dmi_type_t dmi_type_voltage_probe           = { .id = DMI_TYPE_ID_VOLTAGE_PROBE           };
const dmi_type_t dmi_type_cooling_device          = { .id = DMI_TYPE_ID_COOLING_DEVICE          };
const dmi_type_t dmi_type_temperature_probe       = { .id = DMI_TYPE_ID_TEMPERATURE_PROBE       };
const dmi_type_t dmi_type_current_probe           = { .id = DMI_TYPE_ID_CURRENT_PROBE           };
const dmi_type_t dmi_type_oob_remote_access       = { .id = DMI_TYPE_ID_OOB_REMOTE_ACCESS       };
const dmi_type_t dmi_type_bis_entry_point         = { .id = DMI_TYPE_ID_BIS_ENTRY_POINT         };
const dmi_type_t dmi_type_system_boot             = { .id = DMI_TYPE_ID_SYSTEM_BOOT             };
const dmi_type_t dmi_type_memory_error_64         = { .id = DMI_TYPE_ID_MEMORY_ERROR_64         };
const dmi_type_t dmi_type_mgmt_device             = { .id = DMI_TYPE_ID_MGMT_DEVICE             };
const dmi_type_t dmi_type_mgmt_device_component   = { .id = DMI_TYPE_ID_MGMT_DEVICE_COMPONENT   };
const dmi_type_t dmi_type_mgmt_device_threshold   = { .id = DMI_TYPE_ID_MGMT_DEVICE_THRESHOLD   };
const dmi_type_t dmi_type_memory_channel          = { .id = DMI_TYPE_ID_MEMORY_CHANNEL          };
const dmi_type_t dmi_type_ipmi_device             = { .id = DMI_TYPE_ID_IPMI_DEVICE             };
const dmi_type_t dmi_type_power_supply            = { .id = DMI_TYPE_ID_POWER_SUPPLY            };
const dmi_type_t dmi_type_additional_info         = { .id = DMI_TYPE_ID_ADDITIONAL_INFO         };
const dmi_type_t dmi_type_onboard_device_ex       = { .id = DMI_TYPE_ID_ONBOARD_DEVICE_EX       };
const dmi_type_t dmi_type_mgmt_controller         = { .id = DMI_TYPE_ID_MGMT_CONTROLLER         };
const dmi_type_t dmi_type_tpm_device              = { .id = DMI_TYPE_ID_TPM_DEVICE              };
const dmi_type_t dmi_type_processor_ex            = { .id = DMI_TYPE_ID_PROCESSOR_EX            };
const dmi_type_t dmi_type_firmware_inventory      = { .id = DMI_TYPE_ID_FIRMWARE_INVENTORY      };
const dmi_type_t dmi_type_string_property         = { .id = DMI_TYPE_ID_STRING_PROPERTY         };
const dmi_type_t dmi_type_inactive                = { .id = DMI_TYPE_ID_INACTIVE                };
const dmi_type_t dmi_type_end_of_table            = { .id = DMI_TYPE_ID_END_OF_TABLE            };
