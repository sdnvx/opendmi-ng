//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_CHASSIS_H
#define OPENDMI_ENTITY_CHASSIS_H

#pragma once

#include <opendmi/entity/common.h>
#include <opendmi/entity/baseboard.h>

#ifndef DMI_CHASSIS_T
#   define DMI_CHASSIS_T
    typedef struct dmi_chassis           dmi_chassis_t;
#endif // !DMI_CHASSIS_T

#ifndef DMI_CHASSIS_ELEMENT_T
#   define DMI_CHASSIS_ELEMENT_T
    typedef struct dmi_chassis_element   dmi_chassis_element_t;
#endif // !DMI_CHASSIS_ELEMENT_T

/**
 * @brief System enclosure or chassis types.
 */
typedef enum dmi_chassis_type
{
    DMI_CHASSIS_TYPE_UNSPEC              = 0x00, ///< Unspecified
    DMI_CHASSIS_TYPE_OTHER               = 0x01, ///< Other
    DMI_CHASSIS_TYPE_UNKNOWN             = 0x02, ///< Unknown
    DMI_CHASSIS_TYPE_DESKTOP             = 0x03, ///< Desktop
    DMI_CHASSIS_TYPE_LOW_PROFILE_DESKTOP = 0x04, ///< Low-profile desktop
    DMI_CHASSIS_TYPE_PIZZA_BOX           = 0x05, ///< Pizza box
    DMI_CHASSIS_TYPE_MINI_TOWER          = 0x06, ///< Mini tower
    DMI_CHASSIS_TYPE_TOWER               = 0x07, ///< Tower
    DMI_CHASSIS_TYPE_PORTABLE            = 0x08, ///< Portable
    DMI_CHASSIS_TYPE_LAPTOP              = 0x09, ///< Laptop
    DMI_CHASSIS_TYPE_NOTEBOOK            = 0x0A, ///< Notebook
    DMI_CHASSIS_TYPE_HAND_HELD           = 0x0B, ///< Hand held
    DMI_CHASSIS_TYPE_DOCKING_STATION     = 0x0C, ///< Docking station
    DMI_CHASSIS_TYPE_ALL_IN_ONE          = 0x0D, ///< All-in-one
    DMI_CHASSIS_TYPE_SUB_NOTEBOOK        = 0x0E, ///< Sub-notebook
    DMI_CHASSIS_TYPE_SPACE_SAVING        = 0x0F, ///< Space-saving
    DMI_CHASSIS_TYPE_LUNCH_BOX           = 0x10, ///< Lunch box
    DMI_CHASSIS_TYPE_MAIN_SERVER         = 0x11, ///< Main server chassis
    DMI_CHASSIS_TYPE_EXPANSION           = 0x12, ///< Expansion chassis
    DMI_CHASSIS_TYPE_SUB_CHASSIS         = 0x13, ///< Sub-chassis
    DMI_CHASSIS_TYPE_BUS_EXPANSION       = 0x14, ///< Bus expansion chassis
    DMI_CHASSIS_TYPE_PERIPHERAL          = 0x15, ///< Peripheral chassis
    DMI_CHASSIS_TYPE_RAID                = 0x16, ///< RAID chassis
    DMI_CHASSIS_TYPE_RACK_MOUNT          = 0x17, ///< Rack-mount chassis
    DMI_CHASSIS_TYPE_SEALED_CASE_PC      = 0x18, ///< Sealed-case PC
    DMI_CHASSIS_TYPE_MULTI_SYSTEM        = 0x19, ///< Multi-system chassis
    DMI_CHASSIS_TYPE_COMPACT_PCI         = 0x1A, ///< Compact PCI
    DMI_CHASSIS_TYPE_ADVANCED_TCA        = 0x1B, ///< Advanced TCA
    DMI_CHASSIS_TYPE_BLADE               = 0x1C, ///< Blade
    DMI_CHASSIS_TYPE_BLADE_ENCLOSURE     = 0x1D, ///< Blade enclosure
    DMI_CHASSIS_TYPE_TABLET              = 0x1E, ///< Tablet
    DMI_CHASSIS_TYPE_CONVERTIBLE         = 0x1F, ///< Convertible
    DMI_CHASSIS_TYPE_DETACHABLE          = 0x20, ///< Detachable
    DMI_CHASSIS_TYPE_IOT_GATEWAY         = 0x21, ///< IoT gateway
    DMI_CHASSIS_TYPE_EMBEDDED_PC         = 0x22, ///< Embedded PC
    DMI_CHASSIS_TYPE_MINI_PC             = 0x23, ///< Mini PC
    DMI_CHASSIS_TYPE_STICK_PC            = 0x24, ///< Stick PC
    __DMI_CHASSIS_TYPE_COUNT
} dmi_chassis_type_t;

/**
 * @brief Physical security statuses of system enclosures or chassis.
 */
typedef enum dmi_chassis_security_status
{
    DMI_CHASSIS_SECURITY_STATUS_UNSPEC         = 0x00, ///< Unspecified
    DMI_CHASSIS_SECURITY_STATUS_OTHER          = 0x01, ///< Other
    DMI_CHASSIS_SECURITY_STATUS_UNKNOWN        = 0x02, ///< Unknown
    DMI_CHASSIS_SECURITY_STATUS_NONE           = 0x03, ///< None
    DMI_CHASSIS_SECURITY_STATUS_EXT_IF_LOCKED  = 0x04, ///< External interface locked out
    DMI_CHASSIS_SECURITY_STATUS_EXT_IF_ENABLED = 0x05, ///< External interface enabled
    __DMI_CHASSIS_SECURITY_STATUS_COUNT
} dmi_chassis_security_status_t;

/**
 * @brief Rack types, which give the unit of the rack height of a chassis.
 */
typedef enum dmi_rack_type
{
    DMI_RACK_TYPE_UNSPEC = 0x00, ///< Unspecified
    DMI_RACK_TYPE_OPEN   = 0x01, ///< Open Rack, height in OU
    __DMI_RACK_TYPE_COUNT
} dmi_rack_type_t;

/**
 * @brief System enclosure or chassis structure (type 3).
 *
 * The structure describes a mechanical enclosure of the system. A system with
 * several enclosures, such as a separate one for its peripherals, has one
 * structure for each of them.
 */
struct dmi_chassis
{
    /**
     * @brief Manufacturer name.
     */
    const char *vendor;

    /**
     * @brief Type of the enclosure.
     */
    dmi_chassis_type_t type;

    /**
     * @brief Whether the enclosure has a lock. False when there is none, or
     * when it is not known whether there is one.
     */
    bool is_lock_present;

    /**
     * @brief Version.
     */
    const char *version;

    /**
     * @brief Serial number.
     */
    const char *serial_number;

    /**
     * @brief Asset tag.
     */
    const char *asset_tag;

    /**
     * @brief State of the enclosure when it was last booted.
     */
    dmi_status_t bootup_state;

    /**
     * @brief State of the enclosure’s power supply (or supplies) when last
     * booted.
     */
    dmi_status_t power_supply_state;

    /**
     * @brief Thermal state of the enclosure when last booted.
     */
    dmi_status_t thermal_state;

    /**
     * @brief Physical security status of the enclosure when last booted.
     */
    dmi_chassis_security_status_t security_status;

    /**
     * @brief OEM- or firmware vendor-specific information.
     */
    uint32_t oem_defined;

    /**
     * @brief Height of the enclosure, in 'U's.
     *
     * A U is a standard unit of measure for the height of a rack or rack-
     * mountable component and is equal to 1.75 inches or 4.445 cm. A value of
     * `0x00` indicates that the enclosure height is unspecified. A value of
     * `0xFF` indicates that the enclosure height is specified in `rack_height`
     * field.
     */
    unsigned short height;

    /**
     * @brief Number of power cords associated with the enclosure or chassis.
     * A value of `0x00` indicates that the number is unspecified.
     */
    unsigned short power_cord_count;

    /**
     * @brief Number of contained elements.
     */
    size_t element_count;

    /**
     * @brief Size of each contained element record, in bytes.
     */
    size_t element_size;

    /**
     * @brief Contained elements, an array of `element_count` items, or
     * `nullptr` if there is none.
     */
    dmi_chassis_element_t *elements;

    /**
     * @brief SKU number.
     */
    const char *sku_number;

    /**
     * @brief Rack type, which gives the unit of `rack_height`.
     */
    dmi_rack_type_t rack_type;

    /**
     * @brief Height of the enclosure, in the unit of `rack_type`.
     */
    unsigned short rack_height;
};

/**
 * @brief Element contained in a system enclosure or chassis.
 *
 * An element is named either by the type of the SMBIOS structures describing
 * it, or by a baseboard type.
 */
struct dmi_chassis_element
{
    /**
     * @brief SMBIOS structure type, or `DMI_TYPE_ID_INVALID` if the element is
     * identified by baseboard type.
     */
    dmi_type_id_t type;

    /**
     * @brief Baseboard type, or `DMI_BASEBOARD_TYPE_UNSPEC` if the element is
     * identified by SMBIOS structure type.
     */
    dmi_baseboard_type_t board_type;

    /**
     * @brief Specifies the minimum number of the element type that can be
     * installed in the chassis for the chassis to properly operate, in the
     * range 0 to 254. The value 255 (0xFF) is reserved for future definition
     * by this specification, and is represented as `SIZE_MAX`.
     */
    size_t minimum_count;

    /**
     * @brief Specifies the maximum number of the element type that can be
     * installed in the chassis, in the range 1 to 255. The value 0 is reserved
     * for future definition by this specification, and is represented as
     * `SIZE_MAX`.
     */
    size_t maximum_count;
};

/**
 * @brief System enclosure or chassis entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_chassis_spec;

__BEGIN_DECLS

/**
 * @brief Get chassis type name.
 *
 * Returns the human-readable name of the system enclosure or chassis type.
 *
 * @param[in] value Chassis type value.
 *
 * @return The chassis type name string, or @c nullptr if @p value is out of range.
 */
__dmi_api const char *dmi_chassis_type_name(dmi_chassis_type_t value);

/**
 * @brief Get chassis security status name.
 *
 * Returns the human-readable name of the chassis physical security status.
 *
 * @param[in] value Chassis security status value.
 *
 * @return The chassis security status name string, or @c nullptr if @p value is
 * out of range.
 */
__dmi_api const char *dmi_chassis_security_status_name(dmi_chassis_security_status_t value);

/**
 * @brief Get rack type name.
 *
 * Returns the name of the rack type, as the command line tool shows it,
 * translated into the locale when the library is built with the translations.
 *
 * @param[in] value Rack type value.
 *
 * @return The name of the value, or @c nullptr if @p value has no name.
 */
__dmi_api const char *dmi_rack_type_name(dmi_rack_type_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_CHASSIS_H
