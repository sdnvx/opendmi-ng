//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_ONBOARD_DEVICE_H
#define OPENDMI_ENTITY_ONBOARD_DEVICE_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_onboard_device                  dmi_onboard_device_t;
typedef struct dmi_onboard_device_instance         dmi_onboard_device_instance_t;
typedef union  dmi_onboard_device_instance_details dmi_onboard_device_instance_details_t;

/**
 * @brief Onboard device types.
 */
typedef enum dmi_onboard_device_type
{
    DMI_ONBOARD_DEVICE_TYPE_UNSPEC          = 0x00, ///< Unspecified
    DMI_ONBOARD_DEVICE_TYPE_OTHER           = 0x01, ///< Other
    DMI_ONBOARD_DEVICE_TYPE_UNKNOWN         = 0x02, ///< Unknown
    DMI_ONBOARD_DEVICE_TYPE_VIDEO           = 0x03, ///< Video
    DMI_ONBOARD_DEVICE_TYPE_SCSI_CONTROLLER = 0x04, ///< SCSI Controller
    DMI_ONBOARD_DEVICE_TYPE_ETHERNET        = 0x05, ///< Ethernet
    DMI_ONBOARD_DEVICE_TYPE_TOKEN_RING      = 0x06, ///< Token Ring
    DMI_ONBOARD_DEVICE_TYPE_SOUND           = 0x07, ///< Sound
    DMI_ONBOARD_DEVICE_TYPE_PATA_CONTROLLER = 0x08, ///< PATA Controller
    DMI_ONBOARD_DEVICE_TYPE_SATA_CONTROLLER = 0x09, ///< SATA Controller
    DMI_ONBOARD_DEVICE_TYPE_SAS_CONTROLLER  = 0x0A, ///< SAS Controller
    DMI_ONBOARD_DEVICE_TYPE_WIRELESS_LAN    = 0x0B, ///< Wireless LAN
    DMI_ONBOARD_DEVICE_TYPE_BLUETOOTH       = 0x0C, ///< Bluetooth
    DMI_ONBOARD_DEVICE_TYPE_WIRELESS_WAN    = 0x0D, ///< Wireless WAN
    DMI_ONBOARD_DEVICE_TYPE_EMM_CONTROLLER  = 0x0E, ///< eMMC (Embedded multimedia controller)
    DMI_ONBOARD_DEVICE_TYPE_NVME_CONTROLLER = 0x0F, ///< NVMe controller
    DMI_ONBOARD_DEVICE_TYPE_UFS_CONTROLLER  = 0x10, ///< UFS controller
    __DMI_ONBOARD_DEVICE_TYPE_COUNT
} dmi_onboard_device_type_t;

dmi_packed_union(dmi_onboard_device_instance_details)
{
    /**
     * @brief Raw value.
     */
    dmi_byte_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Device type;
         */
        dmi_byte_t type : 7;

        /**
         * @brief Set to `true` if device is enabled.
         */
        dmi_byte_t is_enabled : 1;
    };
};

dmi_static_assert_value_union(dmi_onboard_device_instance_details);

struct dmi_onboard_device_instance
{
    /**
     * @brief Device type.
     */
    dmi_onboard_device_type_t type;

    /**
     * @brief Set to `true` if device is enabled.
     */
    bool is_enabled;

    /**
     * @brief Device description.
     */
    const char *description;
};

/**
 * @brief Onboard devices information structure (type 10, obsolette).
 *
 * The information in this structure defines the attributes of devices that are
 * onboard (soldered onto) a system element, usually the baseboard. In general,
 * an entry in this table implies that the firmware has some level of control
 * over the enabling of the associated device for use by the system.
 *
 * @note
 * Because this structure was originally defined with the Length implicitly
 * defining the number of devices present, no further fields can be added to
 * this structure without adversely affecting existing software’s ability to
 * properly parse the data. Thus, if additional fields are required for this
 * structure type, a brand-new structure must be defined to add a device
 * count field, carry over the existing fields, and add the new information.
 */
struct dmi_onboard_device
{
    /**
     * @brief Number of device instances.
     */
    size_t instance_count;

    /**
     * @brief Device instances.
     */
    dmi_onboard_device_instance_t *instances;
};

/**
 * @brief Onboard device type names.
 */
extern __dmi_api const dmi_name_set_t dmi_onboard_device_type_names;

/**
 * @brief Onboard devices information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_onboard_device_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_onboard_device_type_name(dmi_onboard_device_type_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_ONBOARD_DEVICE_H
