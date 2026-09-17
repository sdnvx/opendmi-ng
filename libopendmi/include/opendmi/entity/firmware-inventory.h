//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_FIRMWARE_INVENTORY_H
#define OPENDMI_ENTITY_FIRMWARE_INVENTORY_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_firmware_inventory           dmi_firmware_inventory_t;
typedef struct dmi_firmware_inventory_data      dmi_firmware_inventory_data_t;
typedef union  dmi_firmware_inventory_features  dmi_firmware_inventory_features_t;
typedef struct dmi_firmware_inventory_component dmi_firmware_inventory_component_t;

/**
 * @brief Version number formats.
 */
typedef enum dmi_version_format
{
    /**
     * @brief The format is a free-form string that is implementation specific.
     * Example: "1.45.455b66-rev4".
     */
    DMI_VERSION_FORMAT_FREE = 0x00,

    /**
     * @brief The format is "MAJOR.MINOR", where MAJOR and MINOR are decimal
     * string representations of the numeric values of the major/minor version
     * numbers. Example: "1.45"
     */
    DMI_VERSION_FORMAT_SEMANTIC = 0x01,

    /**
     * @brief The format is a C-style hexadecimal string representation of the
     * 32-bit numeric value of the version, in the format of `"0xhhhhhhhh"`.
     * Each `h` represents a hexadecimal digit (0-f). Example: `"0x0001002d"`.
     */
    DMI_VERSION_FORMAT_HEX_32 = 0x02,

    /**
     * @brief The format is a C-style hexadecimal string representation of the
     * 64-bit numeric value of the version, in the format of `"0xhhhhhhhhhhhhhhhh"`.
     * Each `h` represents a hexadecimal digit (0-f). Example: `"0x000000010000002d"`.
     */
    DMI_VERSION_FORMAT_HEX_64 = 0x03
} dmi_version_format_t;

/**
 * @enum Firmware identifier formats.
 */
typedef enum dmi_firmware_ident_format
{
    /**
     * @brief The format is a free-form string that is implementation specific.
     * Example: "35EQP72B".
     */
    DMI_FIRMWARE_IDENT_FORMAT_FREE = 0x00,

    /**
     * @brief The format is a string representation of the UEFI ESRT FwClass
     * GUID or the UEFI Firmware Management Protocol ImageTypeId, as defined
     * by the UEFI specification. To represent the GUID, the string is
     * formatted using the 36-character UUID string format specified in
     * RFC4122: `"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"`. Each `x` represents
     * a hexadecimal digit (0-F). Example: `"1624a9df-5e13-47fc-874a-df3aff143089"`.
     */
    DMI_FIRMWARE_IDENT_FORMAT_GUID = 0x01,

    __DMI_FIRMWARE_IDENT_FORMAT_RESERVED_START        = 0x02,
    __DMI_FIRMWARE_IDENT_FORMAT_RESERVED_END          = 0x7F,
    __DMI_FIRMWARE_IDENT_FORMAT_VENDOR_SPECIFIC_START = 0x80,
    __DMI_FIRMWARE_IDENT_FORMAT_VENDOR_SPECIFIC_END   = 0xFF
} dmi_firmware_ident_format_t;

/**
 * @brief Firmware inventory states.
 */
typedef enum dmi_firmware_inventory_state
{
    /**
     * @brief Unspecified.
     */
    DMI_FIRMWARE_INVENTORY_STATE_UNSPEC = 0x00,

    /**
     * @brief Other.
     */
    DMI_FIRMWARE_INVENTORY_STATE_OTHER = 0x01,

    /**
     * @brief Unknown.
     */
    DMI_FIRMWARE_INVENTORY_STATE_UNKNOWN = 0x02,

    /**
     * @brief Disabled: this firmware component is disabled.
     */
    DMI_FIRMWARE_INVENTORY_STATE_DISABLED = 0x03,

    /**
     * @brief Enabled: this firmware component is enabled.
     */
    DMI_FIRMWARE_INVENTORY_STATE_ENABLED = 0x04,

    /**
     * @brief Absent: this firmware component is either not present or not
     * detected.
     */
    DMI_FIRMWARE_INVENTORY_STATE_ABSENT = 0x05,

    /**
     * @brief Standby offline: this firmware is enabled but awaits an external
     * action to activate it.
     */
    DMI_FIRMWARE_INVENTORY_STATE_STANDBY_OFFLINE = 0x06,

    /**
     * @brief Standby spare: this firmware is part of a redundancy set and
     * awaits a failover or other external action to activate it.
     */
    DMI_FIRMWARE_INVENTORY_STATE_STANDBY_SPARE = 0x07,

    /**
     * @brief Unavailable offline: this firmware component is present but
     * cannot be used.
     */
    DMI_FIRMWARE_INVENTORY_STATE_UNAVAIL_OFFLINE = 0x08
} dmi_firmware_inventory_state_t;

/**
 * @brief Firmware characteristics.
 */
dmi_packed_union(dmi_firmware_inventory_features)
{
    dmi_word_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Updatable: This firmware can be updated by software.
         */
        dmi_word_t is_updatable : 1;

        /**
         * @brief Write-protect: This firmware is in a write-protected state.
         */
        dmi_word_t is_write_protected : 1;

        /**
         * @brief Reserved for future user.
         */
        dmi_word_t __reserved : 14;
    };
};

dmi_static_assert_value_union(dmi_firmware_inventory_features);

/**
 * @brief Firmware inventory component.
 */
struct dmi_firmware_inventory_component
{
    /**
     * @brief Component handle.
     */
    dmi_handle_t handle;

    /**
     * @brief Component entity (may be @c nullptr in case of invalid handles etc).
     */
    dmi_entity_t *entity;
};

/**
 * @brief Firmware inventory information.
 */
struct dmi_firmware_inventory
{
    /**
     * @brief The firmware component name. Example: "BMC Firmware".
     */
    const char *name;

    /**
     * @brief The firmware version of this firmware. The format of this value
     * is defined by the fersion format.
     */
    const char *version;

    /**
     * @brief Version number format.
     */
    dmi_version_format_t version_format;

    /**
     * @brief The firmware identifier of this firmware. The format of this
     * value is defined by the firmware identifier format.
     */
    const char *ident;

    /**
     * @brief Firmware identifier format.
     */
    dmi_firmware_ident_format_t ident_format;

    /**
     * @brief The firmware release date. The date string, if supplied, follows
     * the date-time values format, as defined in DSP0266. Example:
     * "2021-05-15T04:14:33+06:00", or when the time is unknown or not
     * specified: "2021-05-15T00:00:00Z".
     */
    const char *release_date;

    /**
     * @brief Manufacturer or producer of this firmware.
     */
    const char *vendor;

    /**
     * @brief String number of the lowest version to which this firmware can
     * be rolled back to. The format of this value is defined by the version
     * format.
     */
    const char *lowest_version;

    /**
     * @brief Size of the firmware image that is currently programmed in the
     * device, in bytes. If the firmware image size is unknown, the field is
     * set to `DMI_SIZE_MAX`.
     */
    dmi_size_t image_size;

    /**
     * @brief Firmware characteristics information.
     */
    dmi_firmware_inventory_features_t features;

    /**
     * @brief Firmware state information.
     */
    dmi_firmware_inventory_state_t state;

    /**
     * @brief Defines how many associated component handles are associated with
     * this firmware.
     */
    size_t component_count;

    /**
     * @brief Lists the SMBIOS components that are associated with this
     * firmware, if any. Value of number of associated components field (n)
     * defines the count.
     *
     * @note
     * This list may contain zero or more handles to any SMBIOS structure that
     * represents a device with a firmware component. For example, this may
     * include:
     *
     * * Type 9 handle (for describing the firmware of a device in a slot).
     * * Type 17 handle (for describing the firmware of a memory device).
     * * Type 41 handle (for describing the firmware of an onboard device).
     * * Type 43 handle (for describing the firmware of a TPM device).
     */
    dmi_firmware_inventory_component_t *components;
};

/**
 * @brief Firmware inventory information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_firmware_inventory_spec;

__BEGIN_DECLS

/**
 * @brief Get version number format name.
 *
 * Returns the human-readable name of the version number format.
 *
 * @param[in] value Version format value.
 *
 * @return The version format name string, or @c nullptr if @p value is out of
 * range.
 */
__dmi_api const char *dmi_version_format_name(dmi_version_format_t value);

/**
 * @brief Get firmware identifier format name.
 *
 * Returns the human-readable name of the firmware identifier format.
 *
 * @param[in] value Firmware identifier format value.
 *
 * @return The firmware identifier format name string, or @c nullptr if @p value is
 * out of range.
 */
__dmi_api const char *dmi_firmware_ident_format_name(dmi_firmware_ident_format_t value);

/**
 * @brief Get firmware inventory state name.
 *
 * Returns the human-readable name of the firmware inventory state.
 *
 * @param[in] value Firmware inventory state value.
 *
 * @return The firmware inventory state name string, or @c nullptr if @p value is
 * out of range.
 */
__dmi_api const char *dmi_firmware_inventory_state_name(dmi_firmware_inventory_state_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_FIRMWARE_INVENTORY_H
